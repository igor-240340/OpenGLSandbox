/*
 * Референсная сцена для сверки работы z-буфера кастомного программного растеризатора.
 */

#include <iostream>
#include <iomanip>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertex_shader_source =
"#version 410 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec3 in_vertex_color;\n"
"layout(location = 0) out vec3 out_vertex_color;\n"
"uniform mat4 proj;\n"
"void main()\n"
"{\n"
"   gl_Position = proj * vec4(pos, 1.0);\n"
"   out_vertex_color = in_vertex_color;\n"
"}\0";

const char* fragment_shader_source =
"#version 410 core\n"
"layout(location = 0) out vec4 out_frag_color;\n"
"layout(location = 0) in vec3 in_frag_color;\n"
"void main()\n"
"{\n"
"   out_frag_color = vec4(in_frag_color, 1.0f);\n"
"}\n\0";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Depth Test", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Компиляция вершинного шейдера.
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    // Проверка статуса компиляции.
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Компиляция фрагментного шейдера.
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    // Проверка статуса компиляции.
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Линковка шейдеров.
    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // Проверка на ошибки.
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << '\n';
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    /*
    float triangles[] = {
        // Red.
         0.12858511992886f,   0.09014727201472f,   -3.062135394734f,  1.0f, 0.0f, 0.0f,
         0.0f,                0.1f,                -4.0f,             1.0f, 0.0f, 0.0f,
        -0.0017940119725222f, -0.013699806553022f, -6.567130320242f,  1.0f, 0.0f, 0.0f,

        // Blue.
         0.05348720083878f,   0.1339907277809f,    -3.684037874323f,  0.0f, 0.0f, 1.0f,
        -0.02750651338714f,  -0.03447517605658f,   -3.722437583851f,  0.0f, 0.0f, 1.0f,
         0.08517046970116f,   0.00457807936321f,   -5.049799420868f,  0.0f, 0.0f, 1.0f
    };
    */

    float triangles[] = {
        // Red.
        1.840996095964f, -1.073676172127f, -3.628196093860f, 1.0f, 0.0f, 0.0f,
        0.06124679400900f, -0.8294603178124f, -3.556538776620f, 1.0f, 0.0f, 0.0f,
        0.5524953604496f, 1.379455366808f, -7.088986515136f,  1.0f, 0.0f, 0.0f,

        // Blue.
        1.580934848009f, 0.5966194549801f, -3.802308656357f, 0.0f, 0.0f, 1.0f,
        -0.8612277692645f, 0.3735791812889f, -6.322245336638f, 0.0f, 0.0f, 1.0f,
        1.131590337138f, -1.400787745470f, -3.985780117663f, 0.0f, 0.0f, 1.0f
    };

    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

    // Position.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 10.0f);
    glUseProgram(shader_program);
    GLuint proj_loc = glGetUniformLocation(shader_program, "proj");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Рисуем.
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // DEBUG: Читаем глубину "проблемного" пиксела".
        float depth;
        glReadPixels(407, (SCR_HEIGHT - 1) - 281, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
        std::cout << std::fixed << std::setprecision(20) << "Z = " << depth << '\n';

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
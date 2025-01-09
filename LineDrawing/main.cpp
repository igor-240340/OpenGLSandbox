#include <iostream>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

constexpr unsigned int width = 800;
constexpr unsigned int height = 600;

const char* vertex_shader_source =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * vec4(pos, 1.0);\n"
"}\0";

const char* fragment_shader_source =
"#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"   color = vec4(0.0, 0.0, 0.0, 1.0);\n"
"}\n\0";

unsigned int shader_program;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Line Drawing", NULL, NULL);
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

    // Компиляция вершинного шейдера.
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

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

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Линковка шейдеров.
    shader_program = glCreateProgram();
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

    std::array<float, 6> vertices = {
        0.0f, 0.0f, 0.0f,
        100.0f, 100.0f, 0.0f
    };

    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float ortho[] = {
        2.0f / width,   0.0f,           0.0f,   0.0f,
        0.0f,           2.0f / height , 0.0f,   0.0f,
        0.0f,           0.0f,           1.0f,   0.0f,
        0.0f,           0.0f,           0.0f,   1.0f
    };

    glUseProgram(shader_program);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, false, ortho);
    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_LINES, 0, 2);
        //glDrawArrays(GL_LINE_LOOP, 0, 4);
        //glDrawArrays(GL_POINTS, 0, 2);

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
    float ortho[] = {
         2.0f / width,   0.0f,           0.0f,   0.0f,
         0.0f,           2.0f / height , 0.0f,   0.0f,
         0.0f,           0.0f,           1.0f,   0.0f,
         0.0f,           0.0f,           0.0f,   1.0f
    };
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, false, ortho);

    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
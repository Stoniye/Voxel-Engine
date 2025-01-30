//Inlcude OpenGL
#include "GL/glew.h"
#include "GLFW/glfw3.h"

//Include System Things
#include "iostream"
#include "fstream"
#include "string"
#include "sstream"

//Include Abstracts
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if(line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else if (type != ShaderType::NONE)
            ss[(int)type] << line << '\n';
    }
    return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error Handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)malloc(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "Shader failed to Compile" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 640, "Voxel Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(5);

    /* Initialize glew and test for errors */
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW Error" << std::endl;

    float positions[] = {
        // Front face
        -0.5f, -0.5f,  0.5f, // 0
         0.5f, -0.5f,  0.5f, // 1
         0.5f,  0.5f,  0.5f, // 2
        -0.5f,  0.5f,  0.5f, // 3

        // Back face
        -0.5f, -0.5f, -0.5f, // 4
         0.5f, -0.5f, -0.5f, // 5
         0.5f,  0.5f, -0.5f, // 6
        -0.5f,  0.5f, -0.5f, // 7
    };

    unsigned int indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        4, 0, 3,
        3, 7, 4,

        // Right face
        1, 5, 6,
        6, 2, 1,

        // Top face
        3, 2, 6,
        6, 7, 3,

        // Bottom face
        4, 5, 1,
        1, 0, 4
    };


    VertexBuffer vb(positions, sizeof(positions));
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    IndexBuffer ib(indices, 36);
    
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    int location = glGetUniformLocation(shader, "u_Color");
    glUniform4f(location, 0.0f, 1.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    const float fov = 30.0f;
    const float aspectRatio = 640.0f / 640.0f;
    const float near = 0.1f;
    const float far = 100.0f;

    // transform 3D coordinates into 2D screen coordinates
    float projectionMatrix[16] = {
        1.0f / (aspectRatio * tan(fov / 2.0f)), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tan(fov / 2.0f), 0.0f, 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -1.0f,
        0.0f, 0.0f, -(2.0f * far * near) / (far - near), 0.0f
    };

    int projectionLocation = glGetUniformLocation(shader, "u_Projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectionMatrix);

    float angle = 0.0f;

    while (!glfwWindowShouldClose(window))
    {   
        // clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.01f;

        // Matrix for position, scale, rotation for the cube
        float modelMatrix[16] = {
            cos(angle), 0.0f, sin(angle), 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -sin(angle), 0.0f, cos(angle), 0.0f,
            0.0f, 0.0f, -2.0f, 1.0f
        };

        int modelLocation = glGetUniformLocation(shader, "u_Model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, modelMatrix);

        // Draw Cube
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
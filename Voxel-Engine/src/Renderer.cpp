#include "Renderer.h"
#include "iostream"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Erorr] Error Code: " << error << " | " << function << " | " << file << " | Line " << line << std::endl;
        return false;
    }
    return true;
}
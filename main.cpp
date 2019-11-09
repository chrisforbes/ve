#include <cstdio>
#include <memory>
#include <err.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint load_shader(GLenum stage, char const *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
        errx(3, "Failed to open shader `%s`", filename);
    fseek(f, 0, SEEK_END);
    auto len = ftell(f);
    fseek(f, 0, SEEK_SET);
    auto data = new char[len];
    if (len != fread(data, 1, len, f))
        errx(4, "Short read shader `%s`", filename);
    fclose(f);

    auto program = glCreateShaderProgramv(stage, 1, &data);
    delete [] data;

    /* TODO: defer this to allow async compile */
    char log[4096];
    glGetProgramInfoLog(program, 4096, nullptr, log);
    if (*log)
        printf("Shader `%s`: \n%s\n", filename, log);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
        errx(5, "Failed to link `%s`", filename);
    return program;
}

void APIENTRY on_gl_error(GLenum, GLenum, GLuint, GLenum, GLsizei, char const * msg, void const *)
{
    fprintf(stderr, "GL: %s\n", msg);
}

int main() {
    if (!glfwInit())
        errx(1, "Failed to init GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    auto wnd = glfwCreateWindow(1024, 768, "ve", nullptr, nullptr);
    if (!wnd)
        errx(2, "Failed to create window");

    glfwMakeContextCurrent(wnd);
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(on_gl_error, nullptr);

    GLuint vs = load_shader(GL_VERTEX_SHADER, "data/shaders/test.vert");
    GLuint fs = load_shader(GL_FRAGMENT_SHADER, "data/shaders/test.frag");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint pipe;
    glGenProgramPipelines(1, &pipe);
    glBindProgramPipeline(pipe);

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(wnd, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, vs);
        glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, fs);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(wnd);
    }
}
#include "ve.h"
#include <cstdio>
#include <err.h>

GLuint load_shader(GLenum stage, char const *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        errx(3, "Failed to open shader `%s`", filename);
    fseek(f, 0, SEEK_END);
    auto len = ftell(f);
    fseek(f, 0, SEEK_SET);
    auto data = new char[len+1];
    if (len != fread(data, 1, len, f))
        errx(4, "Short read shader `%s`", filename);
    fclose(f);

    data[len] = '\0';
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

#pragma once

#include <GL/glew.h>

GLuint load_shader(GLenum stage, char const *filename);
GLuint load_vox(char const *filename);

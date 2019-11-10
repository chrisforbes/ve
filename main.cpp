#include <cstdio>
#include <memory>
#include <err.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

void APIENTRY on_gl_error(GLenum, GLenum, GLuint, GLenum, GLsizei, char const * msg, void const *)
{
    fprintf(stderr, "GL: %s\n", msg);
}

struct Vertex { float x, y, z, nx, ny, nz; };

namespace data {
    Vertex cube_verts[] = {
            // NZ face
            {0, 0, 0, 0,  0,  -1},
            {0, 1, 0, 0,  0,  -1},
            {1, 0, 0, 0,  0,  -1},
            {0, 1, 0, 0,  0,  -1},
            {1, 1, 0, 0,  0,  -1},
            {1, 0, 0, 0,  0,  -1},

            // PZ face
            {0, 0, 1, 0,  0,  1},
            {1, 0, 1, 0,  0,  1},
            {0, 1, 1, 0,  0,  1},
            {0, 1, 1, 0,  0,  1},
            {1, 0, 1, 0,  0,  1},
            {1, 1, 1, 0,  0,  1},

             // NY face
            {0, 0, 0, 0,  -1, 0},
            {1, 0, 0, 0,  -1, 0},
            {0, 0, 1, 0,  -1, 0},
            {0, 0, 1, 0,  -1, 0},
            {1, 0, 0, 0,  -1, 0},
            {1, 0, 1, 0,  -1, 0},

            // PY face
            {0, 1, 0, 0,  1,  0},
            {0, 1, 1, 0,  1,  0},
            {1, 1, 0, 0,  1,  0},
            {0, 1, 1, 0,  1,  0},
            {1, 1, 1, 0,  1,  0},
            {1, 1, 0, 0,  1,  0},

            // NX face
            {0, 0, 0, -1, 0,  0},
            {0, 0, 1, -1, 0,  0},
            {0, 1, 0, -1, 0,  0},
            {0, 0, 1, -1, 0,  0},
            {0, 1, 1, -1, 0,  0},
            {0, 1, 0, -1, 0,  0},

            // PX face
            {1, 0, 0, 1,  0,  0},
            {1, 1, 0, 1,  0,  0},
            {1, 0, 1, 1,  0,  0},
            {1, 0, 1, 1,  0,  0},
            {1, 1, 0, 1,  0,  0},
            {1, 1, 1, 1,  0,  0},
    };
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
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

    GLuint vs = load_shader(GL_VERTEX_SHADER, "data/shaders/test.vert");
    GLuint fs = load_shader(GL_FRAGMENT_SHADER, "data/shaders/test.frag");

    GLuint bo;
    glGenBuffers(1, &bo);
    glBindBuffer(GL_ARRAY_BUFFER, bo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data::cube_verts), data::cube_verts, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, x));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, nx));

    GLuint pipe;
    glGenProgramPipelines(1, &pipe);
    glBindProgramPipeline(pipe);

    float angle = 0;
    float elev = 0.3;
    float dist = 3;

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    GLuint voxel_tex;
    glGenTextures(1, &voxel_tex);
    glBindTexture(GL_TEXTURE_3D, voxel_tex);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R8UI, 16, 16, 16);

    char vox[16 * 16 * 16];
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            for (int k = 0; k < 16; k++)
                vox[i*16*16 + j*16 + k] = (i + j + k) % 8 == 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 16, 16, 16, GL_RED_INTEGER, GL_UNSIGNED_BYTE, vox);

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();

        angle += 0.01f;
        elev += 0.003f;

        int width, height;
        glfwGetFramebufferSize(wnd, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto target = glm::vec3(0.5f, 0.5f, 0.5f);
        auto eye = target + dist * glm::vec3(cosf(angle) * cosf(elev), sinf(elev), sinf(angle) * cosf(elev));
        auto up = glm::vec3(0,1, 0);
        auto view = glm::lookAt(eye, target, up);
        auto proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

        auto mvp = proj * view;

        glProgramUniformMatrix4fv(vs, glGetUniformLocation(vs, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glProgramUniform3fv(fs, glGetUniformLocation(fs, "camera_pos"), 1, &eye[0]);

        glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, vs);
        glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, fs);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(data::cube_verts) / sizeof(Vertex));

        glfwSwapBuffers(wnd);
    }
}
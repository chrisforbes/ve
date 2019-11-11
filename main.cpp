#include <cstdio>
#include <err.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "ve.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void APIENTRY on_gl_error(GLenum, GLenum, GLuint, GLenum severity, GLsizei, char const * msg, void const *)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
        case GL_DEBUG_SEVERITY_MEDIUM:
            fprintf(stderr, "GL: %s\n", msg);
            break;

        default:
            break;
    }
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

    unsigned int default_palette[256] = {
            0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
            0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
            0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
            0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
            0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
            0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
            0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
            0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
            0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
            0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
            0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
            0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
            0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
            0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
            0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
            0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
    };
}

enum { RM_COLOR, RM_DEPTH };
int render_mode = RM_COLOR;
char const *render_mode_list[] = { "Color", "Depth" };
float angle = 0;
float elev = 0.3;
float dist = 3;

float obj_angle;

void gui()
{
    if (ImGui::BeginMainMenuBar())
    {
        char sz[128];
        sprintf(sz, "%.3f ms | %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::BeginMenu(sz, false);
    }
    ImGui::EndMainMenuBar();

    ImGui::Begin("Debug");
    ImGui::Combo("Render mode", &render_mode, render_mode_list, 2);

    ImGui::SliderFloat("Angle", &angle, 0, 2 * M_PI);
    ImGui::SliderFloat("Elevation", &elev, -M_PI_2 * 0.9f, M_PI_2 * 0.9f);
    ImGui::SliderFloat("Distance", &dist, 0.4, 5);

    ImGui::SliderFloat("Object Angle", &obj_angle, 0, 2 * M_PI);

    ImGui::End();
}

int main() {
    if (!glfwInit())
        errx(1, "Failed to init GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, 1);
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

    GLuint fullscreen_vs = load_shader(GL_VERTEX_SHADER, "data/shaders/fullscreen.vert");
    GLuint debug_depth_fs = load_shader(GL_FRAGMENT_SHADER, "data/shaders/debug_depth.frag");

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

    glfwSwapInterval(1);

    GLuint vox = load_vox("data/vox/chr_knight.vox");

    GLuint pal;
    glCreateTextures(GL_TEXTURE_1D, 1, &pal);
    glTextureStorage1D(pal, 1, GL_SRGB8_ALPHA8, 256);
    glTextureSubImage1D(pal, 0, 0, 256, GL_RGBA, GL_UNSIGNED_BYTE, data::default_palette);

    struct {
        GLuint fbo = 0;
        GLuint colorAttach = 0;
        GLuint depthAttach = 0;
        int width = 0;
        int height = 0;

        void update(int width, int height)
        {
            if (fbo && width == this->width && height == this->height)
                return;

            this->width = width;
            this->height = height;

            if (!fbo)
                glCreateFramebuffers(1, &fbo);
            if (colorAttach)
                glDeleteTextures(1, &colorAttach);
            if (depthAttach)
                glDeleteTextures(1, &depthAttach);

            glCreateTextures(GL_TEXTURE_2D, 1, &colorAttach);
            glTextureStorage2D(colorAttach, 1, GL_SRGB8_ALPHA8, width, height);
            glCreateTextures(GL_TEXTURE_2D, 1, &depthAttach);
            glTextureStorage2D(depthAttach, 1, GL_DEPTH_COMPONENT24, width, height);

            glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, colorAttach, 0);
            glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, depthAttach, 0);
        }
    } offscreen;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(wnd, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();

        glEnable(GL_FRAMEBUFFER_SRGB);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int width, height;
        glfwGetFramebufferSize(wnd, &width, &height);
        glViewport(0, 0, width, height);

        offscreen.update(width, height);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, offscreen.fbo);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto target = glm::vec3(0);
        auto eye = target + dist * glm::vec3(cosf(angle) * cosf(elev), sinf(angle) * cosf(elev), sinf(elev));
        auto up = glm::vec3(0,0, 1);
        auto view = glm::lookAt(eye, target, up);
        auto proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
        auto vp = proj * view;

        // Move the model center to the origin (so will occupy space -0.5..0.5)
        // and rotate according to user parameter
        auto model = glm::translate(
                glm::rotate(glm::mat4(1), obj_angle, glm::vec3(0,0,1)),
                glm::vec3(-0.5f));

        // Intersection & lighting is all done in model space, transform the world-space values by the inverse
        // model matrix
        auto invModel = glm::inverse(model);
        auto modelspace_eye = (invModel * glm::vec4(eye, 1));
        auto light_dir = invModel * glm::vec4(glm::normalize(glm::vec3(0.7f,-0.3f,0.3f)), 0);

        glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, vs);
        glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, fs);

        glBindTextures(0, 1, &vox);
        glBindTextures(1, 1, &pal);

        glProgramUniformMatrix4fv(vs, glGetUniformLocation(vs, "model"), 1, GL_FALSE, &model[0][0]);
        glProgramUniformMatrix4fv(vs, glGetUniformLocation(vs, "vp"), 1, GL_FALSE, &vp[0][0]);
        glProgramUniform3fv(fs, glGetUniformLocation(fs, "camera_pos"), 1, &modelspace_eye[0]);
        glProgramUniform3fv(fs, glGetUniformLocation(fs, "light_dir"), 1, &light_dir[0]);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(data::cube_verts) / sizeof(Vertex));

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        switch(render_mode)
        {
            case RM_COLOR:
                // simple color copy pass for now
                glBlitNamedFramebuffer(offscreen.fbo, 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT,
                                       GL_NEAREST);
                break;

            case RM_DEPTH:
                glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, fullscreen_vs);
                glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, debug_depth_fs);
                glBindTextures(0, 1, &offscreen.depthAttach);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                break;
        }

        // imgui isn't properly sRGB-aware
        glDisable(GL_FRAMEBUFFER_SRGB);

        gui();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(wnd);
    }
}
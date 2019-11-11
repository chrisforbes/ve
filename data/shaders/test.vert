#version 450 core

layout(location=0) in vec3 v_pos;
layout(location=1) in vec3 v_norm;

uniform mat4 model;
uniform mat4 vp;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location=0) out vec3 pos;
layout(location=1) out vec3 norm;

void main()
{
    // modelspace position and normal directly to FS
    pos = v_pos;
    norm = v_norm;

    gl_Position = vp * (model * vec4(v_pos, 1));
}

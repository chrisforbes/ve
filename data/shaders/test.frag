#version 450 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;

layout(location=0) out vec4 o_color;

layout(binding=0) uniform usampler3D s_voxels;
uniform vec3 camera_pos;

void main()
{
    vec3 d = normalize(pos - camera_pos);

    o_color = vec4(pos, 1);
}

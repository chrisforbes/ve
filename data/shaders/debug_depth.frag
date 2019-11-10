#version 450 core

layout(binding=0) uniform sampler2D s_depth;
layout(location=0) out vec4 o_color;

void main()
{
    o_color = vec4(vec3(texelFetch(s_depth, ivec2(gl_FragCoord.xy), 0).x), 1);
}
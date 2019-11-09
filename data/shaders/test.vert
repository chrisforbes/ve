#version 450 core

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location=0) out vec3 color;

void main()
{
    gl_Position.x = gl_VertexID * 0.5 - 0.5;
    gl_Position.y = (gl_VertexID == 1) ? 0.5 : -0.5;
    gl_Position.zw = vec2(1);

    color.r = float(gl_VertexID == 0);
    color.g = float(gl_VertexID == 1);
    color.b = float(gl_VertexID == 2);
}

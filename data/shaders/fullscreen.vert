#version 450 core

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    // Single triangle covering the entire clip volume
    gl_Position = vec4(
        4 * (gl_VertexID & 1) - 1,
        4 * (gl_VertexID & 2) - 1,
        0,
        1
    );
}
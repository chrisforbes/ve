#version 450 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;

layout(location=0) out vec4 o_color;

layout(binding=0) uniform usampler3D s_voxels;
layout(binding=1) uniform sampler1D s_palette;
uniform vec3 camera_pos;
uniform vec3 light_dir;

ivec3 size = textureSize(s_voxels, 0).xyz;

float max_along_axis(float o, float d)
{
    if (d > 0)   // TODO: opt
        return abs((ceil(o) - o) / d);
    else
        return abs((floor(o) - o) / d);
}

vec4 voxel_color = vec4(1);

bool march(vec3 o, vec3 d, out vec3 n, out vec3 pp)
{
    o -= 0.01 * d;   // slight fudge -- don't want `o` to be exactly on a plane
    ivec3 p = ivec3(floor(o));
    ivec3 s = ivec3(sign(d));
    vec3 td = abs(1/d);
    vec3 tm = vec3(max_along_axis(o.x, d.x), max_along_axis(o.y, d.y), max_along_axis(o.z, d.z));
    float t = 0;

    while (t < 1.75*16)         // max cast across this thing is sqrt(3) * dim
    {
        uint value = texelFetch(s_voxels, p, 0).x;
        if (value != 0)
        {
            voxel_color = texelFetch(s_palette, int(value), 0);
            pp = o + t * d;
            return true;
        }

        // take a step
        if (tm.x < tm.y) {
            if (tm.x < tm.z) {
                p.x += s.x;
                t = tm.x;
                tm.x += td.x;
                n = vec3(-s.x, 0, 0);
            }
            else {
                p.z += s.z;
                t = tm.z;
                tm.z += td.z;
                n = vec3(0, 0, -s.z);
            }
        }
        else
        {
            if (tm.y < tm.z) {
                p.y += s.y;
                t = tm.y;
                tm.t += td.y;
                n = vec3(0, -s.y, 0);
            }
            else {
                p.z += s.z;
                t = tm.z;
                tm.z += td.z;
                n = vec3(0, 0, -s.z);
            }
        }
    }

    return false;
}

void main()
{
    vec3 n;
    vec3 pp;
    if (!march(pos * size, normalize(pos - camera_pos), n, pp))
        discard;

    o_color = voxel_color * vec4(vec3(clamp(dot(n, light_dir), 0, 1)), 1);
}

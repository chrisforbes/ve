#include "ve.h"
#include <cstdio>
#include <err.h>
#include <cstring>
#include <glm/glm.hpp>
#include <vector>

namespace
{
    struct RiffChunk {
        char type[4];
        int contentSize;
        int childrenSize;
    };

    template<typename ChunkHandler>
    void read_riff(FILE *f, ChunkHandler ch)
    {
        RiffChunk c;
        fread(&c, 1, sizeof(c), f);
        auto startOfChildren = ftell(f) + c.contentSize;
        auto endOfChildren = startOfChildren + c.childrenSize;
        ch(c);
        fseek(f, startOfChildren, SEEK_SET);
        while (ftell(f) < endOfChildren)
            read_riff(f, ch);
        fseek(f, endOfChildren, SEEK_SET);
    }
}

GLuint load_vox(char const *filename)
{
    // Load a MagicaVoxel model file (https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt)
    // into a GL texture

    auto f = fopen(filename, "rb");
    if (!f)
        errx(1, "Failed to open vox `%s`", filename);

    char magic[4];
    fread(magic, 1, sizeof(magic), f);
    if (memcmp(magic, "VOX ", 4))
        errx(2, "Bad magic in vox `%s`", filename);
    int32_t version;
    fread(&version, 1, sizeof(version), f);
    if (version != 150)
        errx( 3, "Unsupported version in vox `%s`: %d; we only handle 150", filename, version);

    glm::ivec3 size;
    std::vector<char> data;

    read_riff(f, [&](RiffChunk const &chunk){
        if (!memcmp(chunk.type, "SIZE", 4)) {
            fread(&size, 12, 1, f);
            data.resize(size.x * size.y * size.z);
        }
        else if (!memcmp(chunk.type, "XYZI", 4)) {
            int numVoxels;
            fread(&numVoxels, sizeof(numVoxels), 1, f);
            for (int n = 0; n < numVoxels; n++)
            {
                struct { char x, y, z, i; } v = {};
                fread(&v, sizeof(v), 1, f);
                data[v.x + v.y * size.x + v.z * size.x * size.y] = v.i+1;   /* zero == nothing here */
            }
        }
    });

    fclose(f);

    GLuint tex;
    glCreateTextures(GL_TEXTURE_3D, 1, &tex);
    glTextureStorage3D(tex, 1, GL_R8UI, size.x, size.y, size.z);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage3D(tex, 0, 0, 0, 0, size.x, size.y, size.z, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data.data());
    return tex;
}
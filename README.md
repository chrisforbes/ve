ve -- a voxel environment
=========================

build
-----

Dependencies:
* cmake
* glm
* glew
* glfw3
* driver supporting OpenGL 4.5 core profile


```
$ cmake -GNinja -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug
$ cmake --build build
$ build/ve
```
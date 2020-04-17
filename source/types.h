/**
 * 
 */
#pragma once

#include <glm/glm.hpp>
#include <GL/gl.h>
#include "3dftex.h"

typedef struct {
  float x, y, z;                /* object space coordinates */
  float nx, ny, nz;             /* object space vertex normal for lighting */
  float s, t;                   /* pre-glide-ified texture coordinates */
} Vert;

typedef struct {
  int v[3];                     /* vertex indices into array of vertes for face */
  int mat_index;                /* material index */
  int aa_edge_flags;
} Face;

typedef struct
{
    Gu3dfInfo* texinfo;
    GLuint tex;
} Texture;


typedef glm::vec3 Vector[3];
typedef glm::mat4 Matrix;
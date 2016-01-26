#include "ClothSim.h"
#include <cstdint>

const float GRAV_ACCEL = 9.8f;
const float PI = 3.1415926536f;

// Some parameters
const float4 wind_force(0.0f, -5.0f, 40.0f, 0.0f);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tris_fill(ClothMeshData &mdata) {
  /* // for GL_TRIANGLE_STRIP
  int vert_tex_n = (mdata.vert_n_side[0] - 1) * 2 * mdata.vert_n_side[1];
  mdata.vert_tex_idx.resize(vert_tex_n);

  // vertices in order such an order that lets us draw textures by triangles
  int cur1 = 0;
  int cur2 = mdata.vert_n_side[1];
  int v_set = 0;
  for (int i = 0; i < mdata.vert_n_side[0] - 1; ++i) {
    for (int j = 0; j < mdata.vert_n_side[1]; ++j) {
      mdata.vert_tex_idx[v_set++] = cur1;
      mdata.vert_tex_idx[v_set++] = cur2;
      //mdata.vert_tex_idx.push_back(cur1);
      //mdata.vert_tex_idx.push_back(cur2);
      ++cur1;
      ++cur2;
    }
  }*/

  mdata.vert_tex_idx.clear();
  int cur1 = 0;
  int cur2 = mdata.vert_n_side[1];
  for (int i = 0; i < (mdata.vert_n_side[0] - 1); ++i) {
    for (int j = 0; j < (mdata.vert_n_side[1] - 1); ++j) {
      mdata.vert_tex_idx.push_back(cur1);
      mdata.vert_tex_idx.push_back(cur2);
      mdata.vert_tex_idx.push_back(cur1 + 1);
      mdata.vert_tex_idx.push_back(cur1 + 1);
      mdata.vert_tex_idx.push_back(cur2);
      mdata.vert_tex_idx.push_back(cur2 + 1);
      ++cur1;
      ++cur2;
    }
    ++cur1;
    ++cur2;
  }
}

inline double dist(float4 &a, float4 &b) {
  return sqrt((a.x - b.x) * (a.x - b.x) +
    (a.y - b.y) * (a.y - b.y) +
    (a.z - b.z) * (a.z - b.z));
}

ClothMeshData CreateTest2Vertices()
{
  ClothMeshData mdata;

  mdata.vertPos0.resize(2);
  mdata.vertVel0.resize(2);
  //mdata.vertPos1.resize(2);
  //mdata.vertVel1.resize(2);
  
  mdata.vertForces.resize(2);
  mdata.vertMassInv.resize(2);

  mdata.edgeIndices.resize(2);
  mdata.edgeHardness.resize(mdata.edgeIndices.size()/2);
  mdata.edgeLen.resize(mdata.edgeIndices.size()/2);

  mdata.vertPos0[0] = float4(0, 0.4, 0, 1); // WARNING, we must always set w = 1 for positions; 
  mdata.vertPos0[1] = float4(0, 0.1, 0, 1); // this is essential for further vertex transform in shader

  mdata.vertVel0[0] = float4(0, 0, 0, 0);
  mdata.vertVel0[1] = float4(0, 0, 0, 0);

  //mdata.vertPos1 = mdata.vertPos0;
  //mdata.vertVel1 = mdata.vertVel0;

  mdata.vertMassInv[0] = 1.0f/1e20f; // we can model static vertices as a vertices with very big mass; didn't say this is good, but simple :)
  mdata.vertMassInv[1] = 1.0f;

  mdata.edgeIndices[0] = 0;
  mdata.edgeIndices[1] = 1;

  mdata.edgeHardness[0]   = 1.0f;
  mdata.edgeLen[0] = 0.2f;

  mdata.g_wind = float4(0, 0, 0, 0);


  // you can use any intermediate mesh representation or load data to GPU (in VBOs) here immediately.                              <<===== !!!!!!!!!!!!!!!!!!

  // create graphics mesh; SimpleMesh uses GLUS Shape to store geometry; 
  // we copy data to GLUS Shape, and then these data will be copyed later from GLUS shape to GPU 
  //
  mdata.pMesh = std::make_shared<SimpleMesh>();

  GLUSshape& shape = mdata.pMesh->m_glusShape;

  shape.numberVertices = mdata.vertPos0.size();
  shape.numberIndices  = mdata.edgeIndices.size();

  shape.vertices  = (GLUSfloat*)malloc(4 * shape.numberVertices * sizeof(GLUSfloat));
  shape.indices   = (GLUSuint*) malloc(shape.numberIndices * sizeof(GLUSuint));

  memcpy(shape.vertices, &mdata.vertPos0[0], sizeof(float) * 4 * shape.numberVertices);
  memcpy(shape.indices, &mdata.edgeIndices[0], sizeof(int) * shape.numberIndices);

  // for tri mesh you will need normals, texCoords and different indices
  // 

  return mdata;
}

ClothMeshData CreateVertices() {
  ClothMeshData mdata;

  int vert_n_side[2] = { 10, 40 };
  double vert_x_bnd[] = { -0.25, 0.25 };
  double vert_y_bnd[] = { -0.2, 0.5 };
  int vert_n = vert_n_side[0] * vert_n_side[1];

  //mdata.vertPos0.resize(vert_n);
  //mdata.vertVel0.resize(vert_n);
  //mdata.vertPos1.resize(vert_n);
  //mdata.vertVel1.resize(vert_n);
  mdata.vert_n_side.resize(2);
  mdata.vert_n_side[0] = vert_n_side[0];
  mdata.vert_n_side[1] = vert_n_side[1];

  mdata.vertPos0.clear();
  mdata.vertVel0.clear();

  double cur_pos[] = { vert_x_bnd[0], vert_y_bnd[0] };
  double vert_step[] = { (vert_x_bnd[1] - vert_x_bnd[0]) / vert_n_side[0],
    (vert_y_bnd[1] - vert_y_bnd[0]) / vert_n_side[1] };
  int vert_idx = 0;
  for (int i = 0; i < vert_n_side[0]; ++i) {
    cur_pos[1] = vert_y_bnd[0];
    for (int j = 0; j < vert_n_side[1]; ++j) {
      mdata.vertPos0.push_back(float4(cur_pos[0], cur_pos[1], 0, 1));
      mdata.vertVel0.push_back(float4(0.0f, 0.0f, 0.0f, 0.0f));
      mdata.vertMassInv.push_back(1e1);
      cur_pos[1] += vert_step[1];
      ++vert_idx;
    }
    cur_pos[0] += vert_step[0];
  }

  // adding springs
  mdata.vertForces.resize(vert_n);

  // structural (green) springs - between each pair of hor/ver adjacent
  vert_idx = 0;
  int a, b;
  for (int i = 0; i < vert_n_side[0]; ++i) {
    for (int j = 0; j < vert_n_side[1]; ++j) {
      if (i > 0) {
        // horizontal
        a = vert_idx - vert_n_side[1];
        b = vert_idx;
        mdata.edgeIndices.push_back(a);
        mdata.edgeIndices.push_back(b);
        mdata.edgeHardness.push_back(100);
        mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
      }
      if (j > 0) {
        // vertical
        a = vert_idx - 1;
        b = vert_idx;
        mdata.edgeIndices.push_back(a);
        mdata.edgeIndices.push_back(b);
        mdata.edgeHardness.push_back(4000);
        mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
      }
      ++vert_idx;
    }
  }
  // shear springs
  vert_idx = vert_n_side[1];
  for (int i = 1; i < vert_n_side[0]; ++i) {
    for (int j = 0; j < vert_n_side[1]; ++j) {
      if (j > 0) {
        a = vert_idx - vert_n_side[1] - 1;
        b = vert_idx;
        mdata.edgeIndices.push_back(a);
        mdata.edgeIndices.push_back(b);
        mdata.edgeHardness.push_back(100);
        mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
      }
      if (j < vert_n_side[1] - 1) {
        a = vert_idx - vert_n_side[1] + 1;
        b = vert_idx;
        mdata.edgeIndices.push_back(vert_idx - vert_n_side[1] + 1);
        mdata.edgeIndices.push_back(vert_idx);
        mdata.edgeHardness.push_back(100);
        mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
      }
      ++vert_idx;
    }
  }

  // bending springs
  vert_idx = 0;
  // for left and right column
  for (int i = 0; i + 2 < vert_n_side[0]; i += 2) {
    // left
    a = vert_idx;
    b = vert_idx + 2 * vert_n_side[1];
    mdata.edgeIndices.push_back(a);
    mdata.edgeIndices.push_back(b);
    mdata.edgeHardness.push_back(100);
    mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
    // right
    a = vert_idx + vert_n_side[1] - 1;
    b = vert_idx + vert_n_side[1] - 1 + 2 * vert_n_side[1];
    mdata.edgeIndices.push_back(a);
    mdata.edgeIndices.push_back(b);
    mdata.edgeHardness.push_back(100);
    mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
    vert_idx += 2 * vert_n_side[1];
  }
  // for top and bottom row
  vert_idx = 0;
  for (int j = 0; j + 2 < vert_n_side[1]; j += 2) {
    // top
    a = j;
    b = j + 2;
    mdata.edgeIndices.push_back(a);
    mdata.edgeIndices.push_back(b);
    mdata.edgeHardness.push_back(100);
    mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
    // bottom
    a = vert_n - vert_n_side[1] + 1 + j;
    b = vert_n - vert_n_side[1] + 1 + j + 2;
    mdata.edgeIndices.push_back(a);
    mdata.edgeIndices.push_back(b);
    mdata.edgeHardness.push_back(100);
    mdata.edgeLen.push_back(dist(mdata.vertPos0[a], mdata.vertPos0[b]));
  }

  mdata.g_wind = wind_force;
  mdata.vertForces.resize(mdata.edgeHardness.size());

  mdata.vertNormals.resize(vert_n);
  for (int i = 0; i < vert_n; i++) {
    mdata.vertNormals[i] = float3(0.0f, 0.0f, 1.0f);
  }

  // you can use any intermediate mesh representation or load data to GPU (in VBOs) here immediately.                              <<===== !!!!!!!!!!!!!!!!!!

  // create graphics mesh; SimpleMesh uses GLUS Shape to store geometry; 
  // we copy data to GLUS Shape, and then these data will be copyed later from GLUS shape to GPU 
  //
  tris_fill(mdata);


  // you can use any intermediate mesh representation or load data to GPU (in VBOs) here immediately.                              <<===== !!!!!!!!!!!!!!!!!!

  // create graphics mesh; SimpleMesh uses GLUS Shape to store geometry; 
  // we copy data to GLUS Shape, and then these data will be copyed later from GLUS shape to GPU 
  //
  mdata.pMesh = std::make_shared<SimpleMesh>();
  mdata.pTris = std::make_shared<SimpleMesh>();

  GLUSshape& shape_wire = mdata.pMesh->m_glusShape;   // shape for wire mode, no textures
  GLUSshape& shape_tris = mdata.pTris->m_glusShape;   // shape for triangles mode, no textures

  shape_wire.numberVertices = mdata.vertPos0.size();
  shape_wire.numberIndices = mdata.edgeIndices.size();

  shape_wire.vertices = (GLUSfloat*)malloc(4 * shape_wire.numberVertices * sizeof(GLUSfloat));
  shape_wire.indices = (GLUSuint*)malloc(shape_wire.numberIndices * sizeof(GLUSuint));

  memcpy(shape_wire.vertices, &mdata.vertPos0[0], sizeof(float) * 4 * shape_wire.numberVertices);
  memcpy(shape_wire.indices, &mdata.edgeIndices[0], sizeof(int) * shape_wire.numberIndices);

  // for tri mesh you will need normals, texCoords and different indices
  // 
  shape_tris.numberVertices = mdata.vertPos0.size();
  shape_tris.numberIndices = int(mdata.vert_tex_idx.size());

  shape_tris.vertices = (GLUSfloat*)malloc(4 * shape_tris.numberVertices * sizeof(GLUSfloat));
  shape_tris.indices = (GLUSuint*)malloc(shape_tris.numberIndices * sizeof(GLUSuint));

  memcpy(shape_tris.vertices, &mdata.vertPos0[0], sizeof(float) * 4 * shape_tris.numberVertices);
  memcpy(shape_tris.indices, &mdata.vert_tex_idx[0], sizeof(float) * shape_tris.numberIndices);

  // texture coordinates
  mdata.texCoords.resize(2 * vert_n);
  for (int i = 0; i < vert_n_side[0]; ++i) {
    for (int j = 0; j < vert_n_side[1]; ++j) {
      // saves texture coords in range [0, texRes[0] - 1] x [0, texRes[1] - 1]
      mdata.texCoords[2 * (vert_n_side[1] * i + j) + 0] = float(vert_n_side[0] - 1 - i) / (vert_n_side[0] - 1);
      mdata.texCoords[2 * (vert_n_side[1] * i + j) + 1] = float(j) / (vert_n_side[1] - 1);
    }
  }

  if (shape_tris.texCoords != nullptr) {
    free(shape_tris.texCoords);
  }
  shape_tris.texCoords = (GLUSfloat*)malloc(2 * vert_n * sizeof(GLUSfloat));
  memcpy(shape_tris.texCoords, &mdata.texCoords[0], 2 * vert_n * sizeof(float));

  // normals
  RecalculateNormals(&mdata);

  // tangents
  shape_tris.tangents = nullptr;

  return mdata;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ClothMeshData::updatePositionsGPU()
{
  if (pMesh == nullptr) {
    return;
  }

  // copy current vertex positions to positions VBO
  glBindBuffer(GL_ARRAY_BUFFER, pMesh->m_vertexPosBufferObject);                                      CHECK_GL_ERRORS;
  glBufferData(GL_ARRAY_BUFFER, pMesh->m_glusShape.numberVertices * 4 * sizeof(GLfloat),
    (GLfloat*)pMesh->m_glusShape.vertices, GL_STREAM_DRAW);  CHECK_GL_ERRORS;

  if (pTris == nullptr) {
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, pTris->m_vertexPosBufferObject);                                      CHECK_GL_ERRORS;
  glBufferData(GL_ARRAY_BUFFER, pTris->m_glusShape.numberVertices * 4 * sizeof(GLfloat),
    (GLfloat*)pTris->m_glusShape.vertices, GL_STREAM_DRAW);  CHECK_GL_ERRORS;
}

void ClothMeshData::updateNormalsGPU()
{
  if (pTris == nullptr || this->vertNormals.size() == 0) {
    return;
  }

  // copy current recalculated normals to appropriate VBO on GPU
  glBindBuffer(GL_ARRAY_BUFFER, pTris->m_vertexNormBufferObject);                                      CHECK_GL_ERRORS;
  glBufferData(GL_ARRAY_BUFFER, pTris->m_glusShape.numberVertices * 3 * sizeof(GLfloat),
    (GLfloat*)pTris->m_glusShape.normals, GL_STREAM_DRAW);
  //(GLfloat*)pTris->m_glusShape.normals, GL_STATIC_DRAW);                                             CHECK_GL_ERRORS;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SimStep(ClothMeshData* pMesh, float delta_t, float elapsedTimeFromStart)
{
  // accumulate forces first
  //
  for (size_t i = 0; i < pMesh->vertForces.size(); i++) // clear all forces
    pMesh->vertForces[i] = float4(0, 0, 0, 0);

  float delta_l;
  float4 dir, force;
  for (int connectId = 0; connectId < int(pMesh->connectionNumber()); connectId++)
  {
    int a = pMesh->edgeIndices[2 * connectId + 0];
    int b = pMesh->edgeIndices[2 * connectId + 1];
    delta_l = dist(pMesh->vertPos0[a], pMesh->vertPos0[b]) - pMesh->edgeLen[connectId];
    dir = pMesh->vertPos0[a] - pMesh->vertPos0[b];
    dir /= sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    force = dir * pMesh->edgeHardness[connectId] * delta_l;
    pMesh->vertForces[a] -= force;
    pMesh->vertForces[b] += force;
  }

  // adding gravity and wind
  for (int i = 0; i < int(pMesh->vertexNumber()); i++) {
    pMesh->vertForces[i] += GRAV_ACCEL / pMesh->vertMassInv[i] * float4(0.0f, -1.0f, 0.0f, 0.0f);
    //pMesh->vertForces[i] += pMesh->g_wind * cos(elapsedTimeFromStart * 2 * PI * 1.0 / (1000.0 * delta_t));
    pMesh->vertForces[i] += pMesh->g_wind * cos(elapsedTimeFromStart * 2 * PI * 1.0 / (300.0 * delta_t)) * 
      int(elapsedTimeFromStart < (450.0 * delta_t));
    //pMesh->vertForces[i] += pMesh->g_wind;
  }

  // update positions and velocity
  //
  float4 accel, delta_v;
  for (int i = 0; i < int(pMesh->vertexNumber()); i++)
  {
    if ((i + 1) % pMesh->vert_n_side[1] != 0) {   // upper row should be freezed
      accel = pMesh->vertForces[i] * pMesh->vertMassInv[i];
      delta_v = accel * delta_t;
      pMesh->vertPos0[i] += (pMesh->vertVel0[i] + accel * delta_t / 2.0) * delta_t;
      //pMesh->vertPos0[i] /= pMesh->vertPos0[i].w;
      pMesh->vertPos0[i].w = 1;
      pMesh->vertVel0[i] += delta_v;
      // lowering speed
      pMesh->vertVel0[i] *= 0.9;
      //pMesh->vertVel0[i] /= pMesh->vertVel0[i].w;
      pMesh->vertVel0[i].w = 1;
    }
  }

  GLUSshape &shape_mesh = pMesh->pMesh->m_glusShape;
  //GLUSshape &shape_tris = pMesh->pTris->m_glusShape;
  if (shape_mesh.vertices == nullptr) {
    shape_mesh.vertices = (GLUSfloat*)malloc(4 * shape_mesh.numberVertices * sizeof(GLUSfloat));
  }
  memcpy(shape_mesh.vertices, &pMesh->vertPos0[0], sizeof(float) * 4 * shape_mesh.numberVertices);
  
  GLUSshape &shape_tris = pMesh->pTris->m_glusShape;
  if (shape_tris.vertices == nullptr) {
    shape_tris.vertices = (GLUSfloat*)malloc(4 * shape_tris.numberVertices * sizeof(GLUSfloat));
  }
  memcpy(shape_tris.vertices, &pMesh->vertPos0[0], sizeof(float) * 4 * shape_tris.numberVertices);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool valid_vertex(int row, int col, std::vector<int> &vert_n_side) {
  return ((0 <= row) && (row < vert_n_side[0]) && (0 <= col) && (col < vert_n_side[1]));
}

void RecalculateNormals(ClothMeshData* pMesh)
{
  int vert_n = pMesh->vertexNumber();
  GLUSshape& shape_tris = pMesh->pTris->m_glusShape;
  if (shape_tris.normals == nullptr) {
    shape_tris.normals = (GLUSfloat*)malloc(vert_n * sizeof(GLUSfloat) * 3);
  }

  int di[] = { 1, 0, -1, 0, 1 };
  int dj[] = { 0, 1, 0, -1, 0 };
  int adj_num;
  int vert_row, vert_col;
  float3 mean_normal(0.0f, 0.0f, 0.0f);
  float4 vec1, vec2;
  float3 vec_cross;

  for (int i = 0; i < vert_n; ++i) {
    adj_num = 0;
    mean_normal = float3(0.0f, 0.0f, 0.0f);
    vert_row = i / pMesh->vert_n_side[1];
    vert_col = i % pMesh->vert_n_side[1];
    for (int k = 1; k < 5; ++k) {
      if (valid_vertex(vert_row + di[k - 1], vert_col + dj[k - 1], pMesh->vert_n_side) &&
        valid_vertex(vert_row + di[k], vert_col + dj[k], pMesh->vert_n_side)) {

        vec1 = pMesh->vertPos0[(vert_row + di[k - 1]) * pMesh->vert_n_side[1] + (vert_col + dj[k - 1])] - pMesh->vertPos0[i];
        vec2 = pMesh->vertPos0[(vert_row + di[k]) * pMesh->vert_n_side[1] + (vert_col + dj[k])] - pMesh->vertPos0[i];
        vec_cross = cross(float3(vec1.x, vec1.y, vec1.z),
          float3(vec2.x, vec2.y, vec2.z));
        vec_cross /= length(vec_cross);
        mean_normal += vec_cross;
        ++adj_num;
      }
    }
    mean_normal /= float(adj_num);
    mean_normal /= length(mean_normal);
    //mean_normal /= mean_normal.w;

    pMesh->vertNormals[i] = mean_normal;
  }
  memcpy(shape_tris.normals, &pMesh->vertNormals[0], sizeof(float) * 3 * shape_tris.numberVertices);
  //
}


#pragma once

#include <GL/glew.h>

#include "GL/glus.h"
#include "../vsgl3/glHelper.h"

#include <memory>
#include <vector>


struct ClothMeshData
{
  ClothMeshData()  {}

  std::shared_ptr<SimpleMesh>   pMesh; // mesh to draw points and lines - i.e. springs; 
  std::shared_ptr<SimpleMesh>   pTris;   // auxilary index buffer and VAO to render triangles. Pease don't copy VBOs. :)

  std::vector<int> vert_n_side;

  std::vector<float4> vertPos0;        // per vertex position; use float4 for pos because SimpleMesh and glusShape use float4 vert pos.
  std::vector<float4> vertVel0;        // per vertex velocity; use float4 also to simplyfy things

  std::vector<float4> vertForces;      // it is possible allocate and free forces each sim step, but we prever to hold it here;
  std::vector<float>  vertMassInv;     // per vertex inverse mass;
  std::vector<float3> vertNormals;     // this is only for graphics; don't need it for sim;

  std::vector<int>    edgeIndices;     // indices of connected verts (with springs); double of connection number size;
  std::vector<float>  edgeHardness;    // Hooke coeff for springs 
  std::vector<float>  edgeLen;  // 
  std::vector<float4> faceNormals;     // this is only for graphics; don't need it for sim;
  std::vector<float> texCoords;

  std::vector<unsigned> vert_tex_idx;

  inline size_t vertexNumber()     const { return vertPos0.size(); }
  inline size_t connectionNumber() const { return edgeIndices.size() / 2; }

  void initGPUData(GLuint a_programId_1, GLuint a_programId_2, GLuint a_programId_3)   {
    pMesh->CreateGPUData(a_programId_1);
    pTris->CreateGPUData(a_programId_3);
    //pTris->CreateGPUData(a_programId_3);
  }
  void updatePositionsGPU();
  void updateNormalsGPU();

  int m_sizeX;
  int m_sizeY;
  float4 g_wind;
};


ClothMeshData CreateTest2Vertices();
ClothMeshData CreateVertices();

void SimStep(ClothMeshData* pMesh, float delta_t, float elapsedTimeFromStart);
void RecalculateNormals(ClothMeshData* pMesh);

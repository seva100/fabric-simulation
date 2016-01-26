//////////////////////////////////////////////////////////////////
// glHelper.h Author: Vladimir Frolov, 2011, Graphics & Media Lab.
//////////////////////////////////////////////////////////////////

#include "glHelper.h"
#include <sstream>

SimpleMesh::SimpleMesh()
{
  m_vertexPosBufferObject = 0;
  m_vertexPosLocation = 0;
  m_vertexNormBufferObject = 0;
  m_vertexNormLocation = 0;
  m_vertexTexCoordsBufferObject = 0;
  m_vertexTexCoordsLocation = 0;
  m_indexBufferObject = 0;
  m_vertexArrayObject = 0;

  memset(&m_glusShape, 0, sizeof(GLUSshape));
}

SimpleMesh::SimpleMesh(GLuint a_programId, int a_slicesNum, int a_meshType, float a_halfSize)
{
  if(a_meshType == SPHERE)
    glusCreateSpheref(&m_glusShape, a_halfSize, a_slicesNum);
  else if(a_meshType == CUBE)
    glusCreateCubef(&m_glusShape, a_halfSize);
  else if(a_meshType == CUBE_OPEN)
    glusCreateCubeOpenf(&m_glusShape, a_halfSize);
  else if(a_meshType == PLANE)
  {
    if(a_slicesNum <= 2)
    {
      glusCreatePlanef(&m_glusShape, a_halfSize);
      for(unsigned int i=0;i<2*m_glusShape.numberVertices;i++)
        m_glusShape.texCoords[i] *= 2.0f;
    }
    else
      glusCreatePlaneSlicedf(&m_glusShape, a_halfSize, a_slicesNum);
  }
  else if(a_meshType == TORUS)
    glusCreateTorusf(&m_glusShape, 0.5f*a_halfSize, a_halfSize, a_slicesNum, 32*a_slicesNum);
  else
    glusCreateCubef(&m_glusShape, 1.0f);


  CreateGPUData(a_programId);
 
}

SimpleMesh::~SimpleMesh()
{
  DestroyGPUData();
  glusDestroyShapef(&m_glusShape);
}


void SimpleMesh::CreateGPUData(GLuint a_programId)
{
  DestroyGPUData();

  m_vertexPosBufferObject = 0;

  m_vertexPosLocation = glGetAttribLocation(a_programId, "vertex");
  m_vertexNormLocation = glGetAttribLocation(a_programId, "normal");
  m_vertexTexCoordsLocation = glGetAttribLocation(a_programId, "texCoord");

  GLint maxVertexAttributes = 0;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);

  // create buffers a,d fill them with data

  // vertex positions, must always present in m_glusShape
  //
  glGenBuffers(1, &m_vertexPosBufferObject);                                                   CHECK_GL_ERRORS;
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexPosBufferObject);                                      CHECK_GL_ERRORS;
  glBufferData(GL_ARRAY_BUFFER, m_glusShape.numberVertices * 4 * sizeof(GLfloat), (GLfloat*)m_glusShape.vertices, GL_STATIC_DRAW);  CHECK_GL_ERRORS;

  // vertex normals, could absent in m_glusShape
  //
  if (m_glusShape.normals != nullptr)
  {
    glGenBuffers(1, &m_vertexNormBufferObject);                                                   CHECK_GL_ERRORS;
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexNormBufferObject);                                      CHECK_GL_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, m_glusShape.numberVertices * 3 * sizeof(GLfloat), (GLfloat*)m_glusShape.normals, GL_STATIC_DRAW);  CHECK_GL_ERRORS;
  }

  // vertex texture coordinates, could absent in m_glusShape
  //
  if (m_glusShape.texCoords != nullptr)
  {
    glGenBuffers(1, &m_vertexTexCoordsBufferObject);                                                   CHECK_GL_ERRORS;
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexTexCoordsBufferObject);                                      CHECK_GL_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, m_glusShape.numberVertices * 2 * sizeof(GLfloat), (GLfloat*)m_glusShape.texCoords, GL_STATIC_DRAW);  CHECK_GL_ERRORS;
  }

  // index buffer, must always present in m_glusShape
  //
  glGenBuffers(1, &m_indexBufferObject);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_glusShape.numberIndices * sizeof(GLuint), (GLuint*)m_glusShape.indices, GL_STATIC_DRAW);
  

  // create VAO and bind each buffer to appropriate "pointer", called vertex array attribute
  //
  glGenVertexArrays(1, &m_vertexArrayObject);                                               CHECK_GL_ERRORS;
  glBindVertexArray(m_vertexArrayObject);                                                   CHECK_GL_ERRORS;

  if (m_vertexPosLocation < GLuint(maxVertexAttributes))
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexPosBufferObject);                    CHECK_GL_ERRORS;
    glEnableVertexAttribArray(m_vertexPosLocation);                            CHECK_GL_ERRORS;
    glVertexAttribPointer(m_vertexPosLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);   CHECK_GL_ERRORS;
  }

  if (m_vertexNormLocation < GLuint(maxVertexAttributes) && (m_glusShape.normals != nullptr))
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexNormBufferObject);                    CHECK_GL_ERRORS;
    glEnableVertexAttribArray(m_vertexNormLocation);                            CHECK_GL_ERRORS;
    glVertexAttribPointer(m_vertexNormLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);   CHECK_GL_ERRORS;
  }

  if (m_vertexTexCoordsLocation < GLuint(maxVertexAttributes) && (m_glusShape.texCoords != nullptr))
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexTexCoordsBufferObject);                    CHECK_GL_ERRORS;
    glEnableVertexAttribArray(m_vertexTexCoordsLocation);                            CHECK_GL_ERRORS;
    glVertexAttribPointer(m_vertexTexCoordsLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);   CHECK_GL_ERRORS;
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);  CHECK_GL_ERRORS;

  glBindVertexArray(0); // unbind VAO

  // don't these need CPU data anymore
  //
  free(m_glusShape.vertices);  m_glusShape.vertices  = nullptr;
  free(m_glusShape.normals);   m_glusShape.normals   = nullptr;
  free(m_glusShape.texCoords); m_glusShape.texCoords = nullptr;
  free(m_glusShape.tangents);  m_glusShape.tangents  = nullptr;
  free(m_glusShape.indices);   m_glusShape.indices   = nullptr;

}


void SimpleMesh::DestroyGPUData()
{
  if (m_vertexPosBufferObject)
  {
    glDeleteBuffers(1, &m_vertexPosBufferObject);
    m_vertexPosBufferObject = 0;
  }

  if (m_vertexNormBufferObject)
  {
    glDeleteVertexArrays(1, &m_vertexNormBufferObject);
    m_vertexNormBufferObject = 0;
  }

  if (m_vertexTexCoordsBufferObject)
  {
    glDeleteVertexArrays(1, &m_vertexTexCoordsBufferObject);
    m_vertexTexCoordsBufferObject = 0;
  }

  if (m_indexBufferObject)
  {
    glDeleteVertexArrays(1, &m_indexBufferObject);
    m_indexBufferObject = 0;
  }

  if (m_vertexArrayObject)
  {
    glDeleteVertexArrays(1, &m_vertexArrayObject);
    m_vertexArrayObject = 0;
  }
}


void SimpleMesh::Draw(GLuint a_primType)
{
  glBindVertexArray(m_vertexArrayObject);                                     CHECK_GL_ERRORS;
  glDrawElements(a_primType, m_glusShape.numberIndices, GL_UNSIGNED_INT, 0);  CHECK_GL_ERRORS;
}

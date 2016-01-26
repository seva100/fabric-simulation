/////////////////////////////////////////////////////////////////
// main.cpp Author: Vladimir Frolov, 2011, Graphics & Media Lab.
/////////////////////////////////////////////////////////////////

#include <GL/glew.h>

#include "GL/glus.h"
#include "../vsgl3/glHelper.h"

#include "ClothSim.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

// Parameters
const float time_step = 0.001f;

// cloth object space transform
//
const float  clothScale = 9.0f;
const float  clothTranslateY = 1.5f;

///////////////////////////////////////////////////////////////////////////
ShaderProgram g_program;
ShaderProgram g_programRed;
ShaderProgram g_programNormals;
ShaderProgram g_programCloth;

struct MyInput
{
  MyInput() 
  {
    cam_rot[0] = cam_rot[1] = cam_rot[2] = cam_rot[3] = 0.f;
    mx = my = 0;
    rdown = ldown = false;
    cam_dist = 20.0f;
  }

  int mx;
  int my;
  bool rdown;
  bool ldown;
  float cam_rot[4];
  float cam_pos[4];

  float cam_dist;

}input;

FullScreenQuad* g_pFullScreenQuad = nullptr;
int g_width  = 0;
int g_height = 0;


SimpleMesh* g_pLandMesh = nullptr;
Texture2D*  g_pClothTex = nullptr;
Texture2D*  g_pLandTex = nullptr;

float4x4    g_projectionMatrix;
float3      g_camPos(0,5,20);
float3      g_curCamPos = g_camPos;
float3      g_sunPos = float3(3.0f, 6.0f, 5.0f);


// cloth sim
//
ClothMeshData g_cloth;
int  g_clothShaderId = 3;
bool g_drawShadowMap = false;
bool make_wind = false;

void RequreExtentions() // check custom extentions here
{
  CHECK_GL_ERRORS;

  std::cout << "GPU Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "GPU Name  : " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "GL_VER    : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL_VER  : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  GlusHelperRequireExt h;
  h.require("GL_EXT_texture_filter_anisotropic");
}

/**
* Function for initialization.
*/
GLUSboolean init(GLUSvoid)
{
  try 
  {
    RequreExtentions();

    // Load the source of the vertex shader. GLUS loader corrupts shaders.
    // Thats why we do that with our class ShaderProgram
    //
    g_program       = ShaderProgram("../src/Vertex.vert", "../src/Fragment.frag"); // shaders to draw land
    g_programNormals = ShaderProgram("../src/Vertex.vert", "../src/Normals.frag");
    g_programCloth  = ShaderProgram("../src/Vertex.vert", "../src/Realistic.frag");
    g_programRed    = ShaderProgram("../src/Vertex.vert", "../src/Red.frag");      // shaders to draw cloth in wire frame mode

    g_pFullScreenQuad  = new FullScreenQuad();
    g_pLandMesh        = new SimpleMesh(g_program.program, 20, SimpleMesh::PLANE);

    g_pClothTex        = new Texture2D("../data/sponza_fabric_green_diff.tga");
    g_pLandTex = new Texture2D("../data/texture1.bmp");

    g_cloth = CreateVertices();

    // copy geometry data to GPU; use shader program to create vao inside; 
    // !!!!!!!!!!!!!!!!!!!!!!!!!!! DEBUG THIS CAREFULLY !!!!!!!!!!!!!!!!!!!!!!! <<============= !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // please remember about vertex shader attributes opt and invalid locations.
    //
    g_cloth.initGPUData(g_programRed.program, g_programNormals.program, g_programCloth.program); 
    
    //g_cloth = CreateQuad(16, 16, 1.0f, g_programRed.program, g_programCloth2.program);

    return GLUS_TRUE;
  }
  catch(std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
  catch(...)
  {
    std::cerr << "Unexpected Exception (init)!" << std::endl;
    exit(-1);
  }
}

GLUSvoid reshape(GLUSuint width, GLUSuint height)
{
	glViewport(0, 0, width, height);
  g_width  = width;
  g_height = height;

  glusPerspectivef(g_projectionMatrix.L(), 45.0f, (GLfloat) width / (GLfloat) height, 1.0f, 100.0f);  // Calculate the projection matrix
}

GLUSvoid mouse(GLUSboolean pressed, GLUSuint button, GLUSuint x, GLUSuint y)
{
  if(!pressed)
    return;

  if (button & 1)// left button
  {
    input.ldown=true;		
    input.mx=x;			
    input.my=y;
  }

  if (button & 4)	// right button
  {
    input.rdown=true;
    input.mx=x;
    input.my=y;
  }
}

GLUSvoid mouseMove(GLUSuint button, GLUSint x, GLUSint y)
{
  if(button & 1)		// left button
  {
    int x1 = x;
    int y1 = y;

    input.cam_rot[0] += 0.25f*(y1-input.my);	// change rotation
    input.cam_rot[1] += 0.25f*(x1-input.mx);

    input.mx=x;
    input.my=y;
  }
}

GLUSvoid keyboard(GLUSboolean pressed, GLUSuint key)
{

  switch(key)
  {
  case 'w':
  case 'W':
    input.cam_dist -= 0.1f;
    break;

  case 's':
  case 'S':
    input.cam_dist += 0.1f;
    break;

  case 'a':
  case 'A':
   
    break;

  case 'd':
  case 'D':

    break;


  case '1':
  case '!':
    g_clothShaderId = 1;
    g_drawShadowMap = false;
    break;

  case '2':
  case '@':
    g_clothShaderId = 2;
    g_drawShadowMap = false;
    break;

  case '3':
  case '#':
    g_clothShaderId = 3;
    g_drawShadowMap = false;
    break;

  case '4':
  case '$':
    g_drawShadowMap = true;
    break;

  case 'p':
  case 'P':
    make_wind = true;
    break;

  }

}

GLUSboolean update(GLUSfloat time)
{
  try 
  {
    static float elaspedTimeFromStart = 0;
    elaspedTimeFromStart += 10 * time;
    static float elapsedInProgram = 0;
    if (make_wind) {
      elapsedInProgram = 0;
      make_wind = false;
    }
    else {
      elapsedInProgram += time_step;
    }

    g_camPos.z = input.cam_dist;


    float4x4 model;
    float4x4 modelView;
    glusLoadIdentityf(model.L()); 
    glusRotateRzRyRxf(model.L(), input.cam_rot[0], input.cam_rot[1], 0.0f);
    glusLookAtf(modelView.L(), g_camPos.x, g_camPos.y, g_camPos.z, 
                               0.0f, 0.0f, 0.0f, 
                               0.0f, 1.0f, 0.0f);                           // ... and the view matrix ...

    glusMultMatrixf(modelView.L(), modelView.L(), model.L()); 	            // ... to get the final model view matrix

    // we will also correct camera position
    float3 old_camPos = g_curCamPos;
    /*float4x4 invModel;
    float4x4 invModelView;
    glusLoadIdentityf(invModel.L());
    glusRotateRzRyRxf(invModel.L(), -input.cam_rot[0], -input.cam_rot[1], 0.0f);
    glusLookAtf(invModelView.L(), g_camPos.x, g_camPos.y, g_camPos.z,
      0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f);
    glusMultMatrixf(invModelView.L(), invModelView.L(), invModel.L());
    glusMultVectorf((GLUSfloat *)&g_curCamPos, (invModelView).L(), (GLUSfloat *)&g_camPos);*/

    float4x4 move_matrix;
    glusRotatef(move_matrix.L(), -input.cam_rot[0], 1.0f, 0.0f, 0.0f);
    glusRotatef(move_matrix.L(), -input.cam_rot[1], 0.0f, 1.0f, 0.0f);
    glusMultVectorf((GLUSfloat *)&g_curCamPos, move_matrix.L(), (GLUSfloat *)&g_camPos);

    //if (old_camPos.x != g_curCamPos.x || old_camPos.y != g_curCamPos.y || old_camPos.z != g_curCamPos.z) {
    //  std::cout << g_curCamPos.x << ' ' << g_curCamPos.y << ' ' << g_curCamPos.z << std::endl;
    //}

    float4x4 rotationMatrix, scaleMatrix, translateMatrix;
    float4x4 transformMatrix1, transformMatrix2;

    // make our program current
    //
    glViewport(0, 0, g_width, g_height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);  

    // cloth
    //

    SimStep(&g_cloth, time_step, elapsedInProgram);
    g_cloth.updatePositionsGPU();

    RecalculateNormals(&g_cloth);
    g_cloth.updateNormalsGPU();


    rotationMatrix.identity();
    scaleMatrix.identity();
    translateMatrix.identity();
    transformMatrix1.identity();
    transformMatrix2.identity();

    glusTranslatef(translateMatrix.L(), 0, clothTranslateY, 0);
    glusScalef(scaleMatrix.L(), clothScale, clothScale, clothScale);
    glusMultMatrixf(transformMatrix1.L(), rotationMatrix.L(), scaleMatrix.L());
    glusMultMatrixf(transformMatrix2.L(), translateMatrix.L(), transformMatrix1.L());

    GLuint landProgramId = g_program.program;
    GLuint redProgramId = g_programRed.program;
    GLuint normalsProgramId = g_programNormals.program;
    GLuint clothProgramId = g_programCloth.program;
    
    if (g_clothShaderId == 1) {
      glUseProgram(redProgramId);   CHECK_GL_ERRORS;
      setUniform(redProgramId, "modelViewMatrix", modelView);   CHECK_GL_ERRORS;
      setUniform(redProgramId, "projectionMatrix", g_projectionMatrix);    CHECK_GL_ERRORS; // set matrix we have calculated in "reshape" funtion
      setUniform(redProgramId, "objectMatrix", transformMatrix2);   CHECK_GL_ERRORS;

      bindTexture(landProgramId, 0, "diffuseTexture", g_pLandTex->GetColorTexId());     CHECK_GL_ERRORS;

      glUniform1i(glGetUniformLocation(landProgramId, "texSampler"), 0);  CHECK_GL_ERRORS;
    }
    else if (g_clothShaderId == 2) {
      glUseProgram(normalsProgramId);   CHECK_GL_ERRORS;
      setUniform(redProgramId, "modelViewMatrix", modelView);   CHECK_GL_ERRORS;
      setUniform(redProgramId, "projectionMatrix", g_projectionMatrix);    CHECK_GL_ERRORS; // set matrix we have calculated in "reshape" funtion
      setUniform(redProgramId, "objectMatrix", transformMatrix2);   CHECK_GL_ERRORS;

      bindTexture(landProgramId, 0, "diffuseTexture", g_pLandTex->GetColorTexId());     CHECK_GL_ERRORS;

      glUniform1i(glGetUniformLocation(landProgramId, "texSampler"), 0);  CHECK_GL_ERRORS;
    }
    else if (g_clothShaderId == 3) {
      glUseProgram(clothProgramId);   CHECK_GL_ERRORS;
      setUniform(clothProgramId, "modelViewMatrix", modelView);   CHECK_GL_ERRORS;
      setUniform(clothProgramId, "projectionMatrix", g_projectionMatrix);    CHECK_GL_ERRORS; // set matrix we have calculated in "reshape" funtion
      setUniform(clothProgramId, "objectMatrix", transformMatrix2);   CHECK_GL_ERRORS;

      setUniform(clothProgramId, "g_sunPos", g_sunPos);   CHECK_GL_ERRORS;
      setUniform(clothProgramId, "camPos", g_curCamPos);   CHECK_GL_ERRORS;

      bindTexture(landProgramId, 0, "diffuseTexture", g_pLandTex->GetColorTexId());     CHECK_GL_ERRORS;
      bindTexture(clothProgramId, 1, "diffuseTexture", g_pClothTex->GetColorTexId());   CHECK_GL_ERRORS;

      glUniform1i(glGetUniformLocation(landProgramId, "texSampler"), 0);
      glUniform1i(glGetUniformLocation(clothProgramId, "texSampler"), 1);   CHECK_GL_ERRORS;
    }

    //bindTexture(landProgramId, 1, "diffuseTexture", g_pLandTex->GetColorTexId());     CHECK_GL_ERRORS;
    //glUniform1i(glGetUniformLocation(landProgramId, "texSampler"), 1);
    
    //if (g_clothShaderId == 3) {
    //glUniform1i(glGetUniformLocation(redProgramId, "texSampler"), 1);
    //glUniform1i(glGetUniformLocation(normalsProgramId, "texSampler"), 2);
    
    //}
    
    if (g_clothShaderId == 1) {
      glPointSize(4.0f); // well, we want to draw vertices as bold points )
      g_cloth.pMesh->Draw(GL_POINTS); // draw vertices
      g_cloth.pMesh->Draw(GL_LINES);  // draw connections
    }
    else if (g_clothShaderId == 2) {
      g_cloth.pTris->Draw(GL_TRIANGLES);
    }
    else if (g_clothShaderId == 3) {
      g_cloth.pTris->Draw(GL_TRIANGLES);
    }

    // \\ cloth

    // draw land
    //
    rotationMatrix.identity();
    scaleMatrix.identity();
    translateMatrix.identity();
    transformMatrix1.identity();
    transformMatrix2.identity();

    glusScalef(scaleMatrix.L(), 15, 15, 15);
    glusTranslatef(translateMatrix.L(), 0, -5, 0);
    glusMultMatrixf(transformMatrix1.L(), rotationMatrix.L(), scaleMatrix.L());
    glusMultMatrixf(transformMatrix2.L(), translateMatrix.L(), transformMatrix1.L());

    glUseProgram(g_program.program);

    setUniform(g_program.program, "modelViewMatrix", modelView);
    setUniform(g_program.program, "projectionMatrix", g_projectionMatrix);  // set matrix we have calculated in "reshape" funtion
    setUniform(g_program.program, "objectMatrix", transformMatrix2);
    setUniform(g_program.program, "camPos", g_curCamPos);

    g_pLandMesh->Draw();
 

    return GLUS_TRUE;
  }
  catch(std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
  catch(...)
  {
    std::cerr << "Unexpected Exception(render)!" << std::endl;
    exit(-1);
  }
}

/**
 * Function to clean up things.
 */
void shutdown(void)
{
  delete g_pFullScreenQuad; g_pFullScreenQuad = nullptr;
  delete g_pLandMesh;       g_pLandMesh = nullptr;
  delete g_pClothTex;       g_pClothTex = nullptr;

}

/**
 * Main entry point.
 */
int main(int argc, char* argv[])
{
	glusInitFunc(init);
	glusReshapeFunc(reshape);
	glusUpdateFunc(update);
	glusTerminateFunc(shutdown);
  glusMouseFunc(mouse);
  glusMouseMoveFunc(mouseMove);
  glusKeyFunc(keyboard);

	glusPrepareContext(3, 0, GLUS_FORWARD_COMPATIBLE_BIT);

	if (!glusCreateWindow("cloth sim", 1024, 768, GLUS_FALSE))
	{
		printf("Could not create window!");
		return -1;
	}

	// Init GLEW
	glewExperimental = GL_TRUE;
  GLenum err=glewInit();
  if(err!=GLEW_OK)
  {
    sprintf("glewInitError", "Error: %s\n", glewGetErrorString(err));
    return -1;
  }
  glGetError(); // flush error state variable, caused by glew errors
  

	// Only continue, if OpenGL 3.3 is supported.
	if (!glewIsSupported("GL_VERSION_3_0"))
	{
		printf("OpenGL 3.0 not supported.");

		glusDestroyWindow();
		return -1;
	}

	glusRun();

	return 0;
}


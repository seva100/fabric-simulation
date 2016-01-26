#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 objectMatrix;

uniform vec3 g_sunPos;
uniform vec3 camPos;

in vec4 vertex;
in vec3 normal;
in vec2 texCoord;

out vec3 fragmentWorldPos;
out vec3 fragmentNormal;
out vec2 fragmentTexCoord;

out vec3 fragmentSunView;
out vec3 fragmentNormalView;
out vec3 fragmentVecToSun;
out vec3 fragmentVecToCam;

void main(void)
{
  vec4 worldPos    = objectMatrix*vertex;
  vec4 viewPos     = modelViewMatrix*worldPos;
  
  //fragmentWorldPos = worldPos.xyz;
  fragmentWorldPos = vec3(worldPos);
  fragmentNormal   = normalize(mat3(objectMatrix)*normal);
  fragmentTexCoord = texCoord;

  fragmentSunView    = mat3(modelViewMatrix)*g_sunPos;
  fragmentNormalView = mat3(modelViewMatrix)*fragmentNormal;

  gl_Position        = projectionMatrix*viewPos;
  fragmentVecToSun = g_sunPos - fragmentWorldPos;
  fragmentVecToCam = camPos - fragmentWorldPos;
}


#version 330

in vec3 fragmentNormal;
in vec2 fragmentTexCoord;

out vec4 fragColor;
out vec3 fragNormal;

uniform sampler2D texSampler;

void main(void)
{
  vec4 texColor = texture(texSampler, fragmentTexCoord);
  fragColor = vec4(1, 0, 0, 1);
  fragNormal = fragmentNormal;
}
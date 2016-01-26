#version 330

in vec3 fragmentNormal;
in vec2 fragmentTexCoord;

out vec4 fragColor;
out vec3 fragNormal;

uniform sampler2D texSampler;

void main(void)
{
  vec4 texColor = texture(texSampler, fragmentTexCoord);
  vec3 normalColor = fragmentNormal / 2.0 + 0.5;
  fragColor = vec4(normalColor, 1);
  fragNormal = fragmentNormal;
}
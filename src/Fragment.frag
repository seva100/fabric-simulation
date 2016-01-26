#version 330

in vec3 fragmentNormal;
in vec2 fragmentTexCoord;
in vec3 fragmentVecToSun;
in vec3 fragmentVecToCam;

out vec4 fragColor;

uniform sampler2D texSampler;

void main(void)
{
  vec4 ambient_color = vec4(1, 1, 1, 0);

  //fragColor = vec4(0,1,0,1);
  fragColor = texture(texSampler, fragmentTexCoord);
  /*
  // Lambert: I = max(0, (n, l))
  //float intensity = dot(normalize(fragmentNormal), normalize(fragmentVecToSun));
  float intensity = dot(normalize(vec3(0, 1, 0)), normalize(fragmentVecToSun));
  if (intensity < 0) {
	intensity = 0;
  }
  */
  // Fong: 
  // r = 2n(n, v) - v,
  // L0 = ka * La + Li * (kd * (l, n) + ks (n, v)^kl)
  // (formula from CG lectures)
  const float ka = 0.1;
  const float kd = 0.5;
  const float kl = 1;
  const float ks = 0.5;

  vec3 v = normalize(fragmentVecToCam);
  vec3 n = vec3(0, 1, 0);
  vec3 r = normalize(2 * n * dot(n, v) - v);
  vec3 l = normalize(fragmentVecToSun);

  fragColor = ka * ambient_color + fragColor * (kd * dot(l, n)  + ks * pow(max(0, dot(v, n)), kl));
  fragColor = clamp(fragColor, 0, 1);
}






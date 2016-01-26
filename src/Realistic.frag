#version 330

in vec3 fragmentNormal;
in vec2 fragmentTexCoord;
in vec3 fragmentVecToSun;
in vec3 fragmentVecToCam;

out vec4 fragColor;
out vec3 fragNormal;

uniform sampler2D texSampler;

void main(void)
{
	vec4 ambient_color = vec4(1, 1, 1, 0);

	fragColor = texture(texSampler, fragmentTexCoord);
	/*
	// Lambert: I = max(0, (n, l))
	float intensity = dot(normalize(fragNormal), normalize(fragmentVecToSun));
	if (intensity < 0) {
	intensity = 0;
	}
	*/
	  
	// Fong: 
	// r = 2n(n, v) - v,
	// L0 = ka * La + Li * (kd * (l, n) + ks (v, n)^kl)
	// (formula from CG lectures)
	const float ka = 0.1;
	const float kd = 0.2;
	const float kl = 0.5;
	const float ks = 0.7;

	vec3 v = normalize(fragmentVecToCam);
	vec3 n = normalize(fragmentNormal);
	//vec3 r = normalize(2 * n * dot(n, v) - v);
	//vec3 r = reflect(-v, n);
	vec3 l = normalize(fragmentVecToSun);

	fragColor = ka * ambient_color + fragColor * (kd * max(dot(l, n), 0)  + ks * pow(max(0, dot(v, n)), kl));
	fragColor = clamp(fragColor, 0, 1);

	fragNormal = fragmentNormal;
}
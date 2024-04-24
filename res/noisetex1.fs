#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;           // Texture coordinates (sampler2D)
varying vec4 fragColor;              // Tint color

// Uniform inputs
uniform vec2 resolution;
uniform vec2 pointer;
uniform sampler2D texture0;
uniform float elapsed;

mat2 rotate(float angle) {
	return mat2(
		cos(angle), -sin(angle),
		sin(angle), cos(angle)
	);
}

vec4 noisetex0(vec2 uv) {
	return texture2D(texture0, uv);
}

vec4 noisetex1(vec2 uv, float t) {
	vec4 rand0 = noisetex0(uv);
	vec4 rand = noisetex0(rand0.xy + t * 0.001);
	vec4 color = rand;

	color.a = 1.0;
	color.r *= 1.0;
	color.g *= 1.0;
	color.b *= 1.0;

	return color;
}

float cross(vec2 uv, float width)  {
	return step(abs(uv.x), width) + step(abs(uv.y), width);
}

vec2 rotate_uv_center( vec2 uv , float rotation) {
	vec2 center = vec2(0.5, 0.5);
	float sf = resolution.x / resolution.y;

	uv -= center; // shifting into center
	uv.x *= sf; // making rotation round
	uv *= rotate(rotation);

	uv.x /= sf; // making coords square

	return uv;
}

vec4 pattern0( vec2 uv ) {
	uv = rotate_uv_center(uv, elapsed * cos(elapsed * 0.01) * 0.01);

	uv *= 0.1; // scaling

	return noisetex1(uv, elapsed);
}

vec4 pattern1( vec2 uv, float t, float scale) {

	// noise texture itself
	vec2 uv1 = rotate_uv_center(uv, -t);
	uv1 *= scale;
	vec4 color = noisetex1(uv1, t);

	// cross mask
	float sf = resolution.x / resolution.y;
	vec2 center = vec2(0.5, 0.5);
	vec2 uv2 = uv;
	uv2 -= center;
	uv2.x *= sf;
	float distance = length(uv2);

	uv2 *= rotate(t);

	float cross_mask = 0.0;
	int iterations = 10;
	for(int i = 0; i < iterations; i++) {
		float c = cross(uv2, 0.01 * pow(distance + 1.0, 3.0)) * (1.0 - float(i) / float(iterations));
		cross_mask = max(c, cross_mask);
		uv2 *= rotate(-0.1 * sign(t));
	}

	color *= cross_mask;

	//return vec4(distance, distance, distance, 1.0);
	//return vec4(cross_mask, cross_mask, cross_mask, 1.0);

	return color;
}

vec4 pattern2( vec2 uv ) {
	float scale = 10.0;
	float sf = resolution.x / resolution.y;
	vec2 uv_fract = fract(uv * scale);
	vec2 uv_scaled = ceil(uv * scale) / scale;
	vec4 rand0 = noisetex0(uv_scaled);
	return pattern1(uv_fract, elapsed * (rand0.x - 0.5), 1.0 / scale * 0.5);
}

void main() {
	vec2 uv = fragTexCoord;
	gl_FragColor = pattern2(uv) * fragColor;
}

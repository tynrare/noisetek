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
	return texture2D(texture0, fract(uv));
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

/*
* Draws noise around center
*/
vec4 pattern0( vec2 uv ) {
	uv = rotate_uv_center(uv, elapsed * cos(elapsed * 0.01) * 0.01);

	uv *= 0.1; // scaling

	return noisetex1(uv, elapsed);
}

void main() {
	vec2 uv = fragTexCoord;
	gl_FragColor = pattern0(uv) * fragColor;
}

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
	vec2 uvd = uv;

	vec4 t0 = texture2D(texture0, fract(uv));
	return t0;

/*
	float xf = sin(abs(uvd.x) * 100.0);
	uvd.x =  pow(uvd.x, xf * 0.1);
	float yf = abs(uvd.y);
	//uvd.y =  pow(uvd.y, yf * 0.1);

	uv.x += t0.r * uvd.x;
	uv.y += t0.g * uvd.y;
	
	return vec4(uvd.x, 0.0, 0.0, 1.0);
	//vec4 t1 = texture2D(texture0, fract(uv));
	//return t1;
	*/
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
	uv -= center; // shifting into center
	uv *= rotate(rotation);

	uv += center;
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

float scale = 1.0;
float xcenter = 512.0;
float ycenter = 512.0;
void main() {
	vec2 uv = fragTexCoord;
	uv.y = 1.0 - uv.y;

	vec4 p = texture2D(texture0, fract(uv));
	vec4 color = p;
	gl_FragColor = color;
	//uv += vec2(0.5, 0.5);
	//uv /= scale;
	//uv.x -= xcenter / resolution.x;
	//uv.y -= ycenter / resolution.y;
	//vec2 uvr = uv * rotate(elapsed * 0.01);
	//vec2 uvr = rotate_uv_center(uv, elapsed * 0.1);
	//vec2 uvr = uv;

	//gl_FragColor = noisetex0(uvr) * fragColor;
}

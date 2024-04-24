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

void main() {
	vec2 uv = fragTexCoord;
	uv = fract(uv);
	vec4 color = texture2D(texture0, uv);
	color.r = 0.0;
	color.g = pow(color.g, 0.1);
	color.b = pow(color.b, 0.1);
	color.a = 1.0;
	gl_FragColor = color * fragColor;
}

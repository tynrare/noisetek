#include "shader_autoreload.h"

#ifndef NOISETEX_H
#define NOISETEX_H

typedef struct NoiseTexState {
  Texture2D texture;
  RenderTexture render_texture;
	ShaderAutoReloadState ar_shader;
	float scale;
} NoiseTexState;

Texture2D NoiseTexGenerate(int width, int height);
NoiseTexState *noisetex_init();
void noisetex_step(NoiseTexState *state);
void noisetex_dispose(NoiseTexState *state);

#endif

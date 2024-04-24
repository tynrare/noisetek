#include "shader_autoreload.h"

#ifndef ANYSHADER_H
#define ANYSHADER_H

typedef struct AnyshaderState {
	const char *filename;
  Texture2D texture;
  RenderTexture render_texture;
	ShaderAutoReloadState ar_shader;
	int file_index;
} AnyshaderState;

AnyshaderState *anyshader_init(const char *filename, Texture2D texture);
void anyshader_next_file(AnyshaderState *state);
void anyshader_step(AnyshaderState *state);
void anyshader_dispose(AnyshaderState *state);

#endif

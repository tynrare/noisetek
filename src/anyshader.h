#include "shader_autoreload.h"

#ifndef ANYSHADER_H
#define ANYSHADER_H


typedef enum ANYSHADER_MODES {
	// Draw shader fullscreen
  ANYSHADER_MODE_FULLSCREEN = 0,
	// Draw shader in small window
  ANYSHADER_MODE_THUMBNAIL,
	// Do not draw shader, just update
  ANYSHADER_MODE_HIDDEN,
	// Do not draw shader and do not update
  ANYSHADER_MODE_SUSPEND,
	// Skip shader completely. Unimplemented
  ANYSHADER_MODE_DISABLED,
	// ...
  __ANYSHADER_MODES_COUNT
} ANYSHADER_MODES;

typedef struct AnyshaderState {
	const char *filename;
  Texture2D texture;
  RenderTexture render_texture;
	ShaderAutoReloadState ar_shader;
	ANYSHADER_MODES mode;
	int file_index;
} AnyshaderState;

AnyshaderState *anyshader_init(const char *filename);
void anyshader_next_file(AnyshaderState *state);
void anyshader_step(AnyshaderState *state, Texture2D texture);
void anyshader_dispose(AnyshaderState *state);

#endif

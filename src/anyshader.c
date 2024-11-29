#include "anyshader.h"
#include "root.h"
#include <raylib.h>

static const char *_anyshader_filename(const char *name, int index) {
	return TextFormat(RES_PATH "%s%d.fs", name, index);
}

static const char *anyshader_filename(AnyshaderState *state) {
	const int file_index = state->file_index;
	const char *file_name = state->filename;
	const char *filepath = _anyshader_filename(file_name, file_index);

	return filepath;
}
static void anyshader_load(AnyshaderState *state) {
	const char *filepath = anyshader_filename(state);
  shader_ar_init(&state->ar_shader, filepath);
}

AnyshaderState *anyshader_init(const char *filename) {
  AnyshaderState *state = MemAlloc(sizeof(AnyshaderState));

  state->render_texture =
      LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  state->texture = state->render_texture.texture;

	state->file_index = 0;
	state->filename = filename;
	anyshader_load(state);


  return state;
}

void anyshader_next_file(AnyshaderState *state) {
	int next_index = state->file_index + 1;
	const char *filepath = _anyshader_filename(state->filename, next_index);
	next_index = FileExists(filepath) ? next_index : 0;

	if (next_index == state->file_index) {
		return;
	}

	state->file_index = next_index;

  UnloadShader(state->ar_shader.shader);
	anyshader_load(state);
}

void anyshader_step(AnyshaderState *state, Texture2D texture) {
  shader_ar_step(&state->ar_shader);

	BeginTextureMode(state->render_texture);
  BeginShaderMode(state->ar_shader.shader);

		Rectangle rec = {0, 0, GetScreenWidth(), GetScreenHeight()};
		Vector2 pos = {0, 0};
		const Texture2D t = texture;
		const int w = t.width;
		const int h = t.height;
    Rectangle rsource = {0.0, 0.0, w, h};
    Rectangle rdest = rsource;
		DrawTexture(t, 0, 0, WHITE);
		//DrawTexturePro(t, rsource, rdest, (Vector2){0.0, 0.0}, 0.0, WHITE);
		//DrawTextureRec(t, rec, pos, WHITE);
  EndShaderMode();
	EndTextureMode();
}

void anyshader_dispose(AnyshaderState *state) {
  if (!state) {
    return;
  }
  UnloadShader(state->ar_shader.shader);
  UnloadTexture(state->texture);
  UnloadRenderTexture(state->render_texture);
}

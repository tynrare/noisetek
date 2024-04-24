#include "noise_tex.h"
#include "root.h"
#include <raylib.h>

static const char *shader_file_path = RES_PATH "noisetex%d.fs";


Texture2D NoiseTexGenerate(int width, int height) {
  int w = width;
  int h = height;
  Color *pixels = (Color *)MemAlloc(w * h * sizeof(Color));

  for (int x = 0; x < w * h; x += 1) {
    Color *p = &pixels[x];
    p->r = GetRandomValue(0, 0xff);
    p->g = GetRandomValue(0, 0xff);
    p->b = GetRandomValue(0, 0xff);
    p->a = GetRandomValue(0, 0xff);
  }

  Image image = {.data = pixels, // We can assign pixels directly to data
                 .width = w,
                 .height = h,
                 .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
                 .mipmaps = 1};
  Texture2D texture = LoadTextureFromImage(image);
  UnloadImage(image); // Unload CPU (RAM) image data (pixels)

  return texture;
}

static void noisetex_load_shader(NoiseTexState *state) {
	const int file_index = state->file_index;
	const char *filepath = TextFormat(shader_file_path, file_index);
  shader_ar_init(&state->ar_shader, filepath);
}

NoiseTexState *noisetex_init() {
  NoiseTexState *state = MemAlloc(sizeof(NoiseTexState));


  state->render_texture =
      LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  state->texture = NoiseTexGenerate(GetScreenWidth(), GetScreenHeight());

	state->file_index = 0;
	noisetex_load_shader(state);

	noisetex_step(state);

  return state;
}

void noisetex_next_file(NoiseTexState *state) {
	const int next_index = state->file_index + 1;
	const char *filepath = TextFormat(shader_file_path, next_index);
	state->file_index = FileExists(filepath) ? next_index : 0;

  UnloadShader(state->ar_shader.shader);
	noisetex_load_shader(state);
}

void noisetex_step(NoiseTexState *state) {
  shader_ar_step(&state->ar_shader);

	BeginTextureMode(state->render_texture);
  BeginShaderMode(state->ar_shader.shader);
		// squire mode
		// Rectangle rec = {0, 0, GetScreenHeight(), GetScreenHeight()};
		// Vector2 pos = {(float)(GetScreenWidth() - GetScreenHeight()) / 2, 0};
		// fullscreen mode
		Rectangle rec = {0, 0, GetScreenWidth(), GetScreenHeight()};
		Vector2 pos = {0, 0};
		DrawTextureRec(state->texture, rec, pos, WHITE);
  EndShaderMode();
	EndTextureMode();
}

void noisetex_dispose(NoiseTexState *state) {
  if (!state) {
    return;
  }
  UnloadShader(state->ar_shader.shader);
  UnloadTexture(state->texture);
  UnloadRenderTexture(state->render_texture);
}

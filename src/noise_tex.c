#include "noise_tex.h"
#include "root.h"
#include <raylib.h>

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

NoiseTexState *noisetex_init() {
  NoiseTexState *state = MemAlloc(sizeof(NoiseTexState));

  shader_ar_init(&state->ar_shader, RES_PATH "noisetex0.fs");

  state->render_texture =
      LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  state->scale = 1.0;

  state->texture = NoiseTexGenerate(GetScreenWidth(), GetScreenHeight());

  return state;
}

void noisetex_step(NoiseTexState *state) {
  shader_ar_step(&state->ar_shader);
  state->scale += GetMouseWheelMove();

  BeginShaderMode(state->ar_shader.shader);
		// squire mode
		// Rectangle rec = {0, 0, GetScreenHeight(), GetScreenHeight()};
		// Vector2 pos = {(float)(GetScreenWidth() - GetScreenHeight()) / 2, 0};
		// fullscreen mode
		Rectangle rec = {0, 0, GetScreenWidth(), GetScreenHeight()};
		Vector2 pos = {0, 0};
		DrawTextureRec(state->texture, rec, pos, WHITE);
  EndShaderMode();
}

void noisetex_dispose(NoiseTexState *state) {
  if (!state) {
    return;
  }
  UnloadShader(state->ar_shader.shader);
  UnloadTexture(state->texture);
  UnloadRenderTexture(state->render_texture);
}

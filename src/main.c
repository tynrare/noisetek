// @tynroar

#include "anyshader.h"
#include "noise_tex.h"
#include "root.h"
#include <raylib.h>
#include <stdio.h>

bool active = false;

static const int VIEWPORT_W = 0x320;
static const int VIEWPORT_H = 0x1c2;

static int viewport_w = VIEWPORT_W;
static int viewport_h = VIEWPORT_H;

int active_case = 0;
#define ANYSHADERS_TOTAL 3
int anyshaders_pipe_length = ANYSHADERS_TOTAL;
const char *anyshaders_pipe[] = {"noisetex", "anyshader", "anyshader"};
AnyshaderState *anyshaders[ANYSHADERS_TOTAL];

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// ---

bool _draw_help = true;
bool _draw_stack = true;

// ---

void anyshader_draw(AnyshaderState *state, bool flip_h, int index) {
  Texture2D *rtt = &state->render_texture.texture;
  float w = rtt->width;
  float h = rtt->height;
  Rectangle rsource = {0.0, 0.0, rtt->width, rtt->height};
  Rectangle rdest = rsource;

  if (flip_h) {
    rsource.height *= -1;
  }
  if (state->mode == ANYSHADER_MODE_THUMBNAIL) {
    rdest = (Rectangle){8.0 + index * 32, 8.0, w / 2 - 8.0, h / 2 - 8.0};
  }
  DrawTexturePro(*rtt, rsource, rdest, (Vector2){0.0, 0.0}, 0.0, WHITE);
}

void draw() {
  ClearBackground(BLACK);

  // Render all shaders into texture
  // Render fullscreen shader instantly
  for (int i = 0; i < anyshaders_pipe_length; i++) {
    AnyshaderState *state = anyshaders[i];

    // suspended states do not require update
    if (state->mode != ANYSHADER_MODE_SUSPEND) {
      anyshader_step(state);
    }

    // do not render further if state requested fullscreen
    if (state->mode == ANYSHADER_MODE_FULLSCREEN) {
      anyshader_draw(state, !(i % 2), i);
      break;
    }
  }

  // render thumbnails
  for (int i = 0; i < anyshaders_pipe_length; i++) {
    AnyshaderState *state = anyshaders[i];
    if (state->mode >= ANYSHADER_MODE_HIDDEN) {
      continue;
    }
    if (state->mode == ANYSHADER_MODE_FULLSCREEN) {
      break;
    }

    if (state->mode == ANYSHADER_MODE_THUMBNAIL) {
      anyshader_draw(state, !(i % 2), i);
    }
  }
}

void dispose() {
  for (int i = 0; i < anyshaders_pipe_length; i++) {
    anyshader_dispose(anyshaders[i]);
  }
}

void init() {
  dispose();

  const int w = viewport_w;
  const int h = viewport_h;
  Texture2D noisetex = NoiseTexGenerate(w, h);
  anyshaders[0] = anyshader_init("noisetex", noisetex);
  for (int i = 1; i < anyshaders_pipe_length; i++) {
    const char *name = anyshaders_pipe[i];
    const Texture2D texture = anyshaders[i - 1]->render_texture.texture;
    anyshaders[i] = anyshader_init(name, texture);
  }
}

static void _toggle_thumbs(bool hide) {
	for (int i = 0; i < anyshaders_pipe_length; i++) {
		AnyshaderState *state = anyshaders[i];
		if (state->mode == ANYSHADER_MODE_FULLSCREEN) {
			break;
		} else if (state->mode == ANYSHADER_MODE_HIDDEN && hide) {
			state->mode = ANYSHADER_MODE_THUMBNAIL;
		} else if (state->mode == ANYSHADER_MODE_THUMBNAIL && !hide) {
			state->mode = ANYSHADER_MODE_HIDDEN;
		}
	}
}

void inputs() {
  if (IsKeyPressed(KEY_H)) {
    _draw_help = !_draw_help;
  }

  if (IsKeyPressed(KEY_I)) {
    _draw_stack = !_draw_stack;
  }

	if (IsKeyPressed(KEY_A)) {
    _draw_help = !_draw_help;
    _draw_stack = _draw_help;
		_toggle_thumbs(_draw_stack);
	}

  if (IsKeyPressed(KEY_N)) {
    anyshaders[active_case]->mode = ANYSHADER_MODE_THUMBNAIL;
    active_case = (active_case + 1) % ANYSHADERS_TOTAL;
    anyshaders[active_case]->mode = ANYSHADER_MODE_FULLSCREEN;
  }
  if (IsKeyPressed(KEY_X)) {
    anyshader_next_file(anyshaders[active_case]);
  }

  if (IsKeyPressed(KEY_C)) {
		for (int i = 0; i < anyshaders_pipe_length; i++) {
			AnyshaderState *state = anyshaders[i];
			if (state->mode == ANYSHADER_MODE_FULLSCREEN) {
				break;
			} else if (state->mode == ANYSHADER_MODE_HIDDEN) {
				state->mode = ANYSHADER_MODE_THUMBNAIL;
			} else if (state->mode == ANYSHADER_MODE_THUMBNAIL) {
				state->mode = ANYSHADER_MODE_HIDDEN;
			}
		}
  }

  /*
if (IsKeyPressed(KEY_ONE)) {
noisetek_mode = NOISETEK_MODE_FULLSCREEN;
} else if (IsKeyPressed(KEY_TWO)) {
noisetek_mode = NOISETEK_MODE_THUMBNAIL;
} else if (IsKeyPressed(KEY_THREE)) {
noisetek_mode = NOISETEK_MODE_HIDDEN;
} else if (IsKeyPressed(KEY_FOUR)) {
noisetek_mode = NOISETEK_MODE_SUSPEND;
}
  */
}

static long resize_timestamp = -1;
static const float resize_threshold = 0.3;
static Vector2 requested_viewport = {VIEWPORT_W, VIEWPORT_H};
void equilizer() {
  const int vw = GetScreenWidth();
  const int vh = GetScreenHeight();

  const long now = GetTime();

  // thresholds resizing
  if (requested_viewport.x != vw || requested_viewport.y != vh) {
    requested_viewport.x = vw;
    requested_viewport.y = vh;

    // first resize triggers intantly (important in web build)
    if (resize_timestamp > 0) {
      resize_timestamp = now;
      return;
    }
  }

  // reinits after riseze stops
  const bool resized =
      requested_viewport.x != viewport_w || requested_viewport.y != viewport_h;
  if (resized && now - resize_timestamp > resize_threshold) {
    resize_timestamp = now;
    viewport_w = vw;
    viewport_h = vh;
    init();
  }
}

void draw_help_text() {

  // Draw help info
	const int fontsize = 20;
  if (_draw_help) {
    const char *msg0 = "[H]: Help toggle";
    const char *msg1 = "[I]: Info toggle";
    const char *msg2 = "[C]: Thumbs toggle";
    const char *msg3 = "[A]: All toggle";
    const char *msg4 = "[N]: Cycle pipe";
    const char *msg5 = "[X]: Cycle file";
    DrawRectangle(4, 4, 200, 200, Fade(BLACK, 0.7));
    DrawText(msg0, 8, 8, fontsize, WHITE);
    DrawText(msg1, 8, 8 + (fontsize + 4) * 1, fontsize, WHITE);
    DrawText(msg2, 8, 8 + (fontsize + 4) * 2, fontsize, WHITE);
    DrawText(msg3, 8, 8 + (fontsize + 4) * 3, fontsize, WHITE);
    DrawText(msg4, 8, 8 + (fontsize + 4) * 4, fontsize, WHITE);
    DrawText(msg5, 8, 8 + (fontsize + 4) * 5, fontsize, WHITE);
  }

  if (_draw_stack) {
    const int h = 200;
    const int ho = viewport_h - 4 - h;
    DrawRectangle(4, ho, 200, h, Fade(BLACK, 0.7));

		char b[64];
    for (int i = 0; i < anyshaders_pipe_length; i++) {
			const int y = ho + 4 + (fontsize + 4) * i;
			const Color color = i == active_case ? RED : WHITE;
      DrawRectangle(8, y + 2, 16, 16, color);
			const AnyshaderState *state = anyshaders[i];
			const char *name = state->filename;
			sprintf(b, "%s #%d", name, state->file_index);
			DrawText(b, 8 + 16 + 4, y, fontsize, WHITE);
    }
  }
}

void step(void) {
  equilizer();

  inputs();

  BeginDrawing();
  draw();
  draw_help_text();
  EndDrawing();
}

void loop() {
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(step, 0, 1);
#else

  while (!WindowShouldClose()) {
    step();
  }
#endif
}

int main(void) {
  const int seed = 2;
  const int max = 255;

  InitWindow(viewport_w, viewport_h, "noisetek");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);
  SetRandomSeed(seed);

  init();

  active = true;
  loop();

  dispose();
  CloseWindow();

  return 0;
}

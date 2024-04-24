// @tynroar

#include <raylib.h>
#include "noise_tex.h"

bool active = false;

static const int VIEWPORT_W = 0x320;
static const int VIEWPORT_H = 0x1c2;

static int viewport_w = VIEWPORT_W;
static int viewport_h = VIEWPORT_H;

typedef enum SHOWCASE_MODES {
	SHOWCASE_MODE_NOISE_TEX = 0,
	__SHOWCASE_MODES_COUNT
} SHOWCASE_MODES;

NoiseTexState *noisetex_state = NULL;
SHOWCASE_MODES mode = SHOWCASE_MODE_NOISE_TEX;

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif



void draw() {
	ClearBackground(BLACK);
  switch (mode) {
  default:
  case SHOWCASE_MODE_NOISE_TEX:
    noisetex_step(noisetex_state);
    break;
  }

  if (IsKeyPressed(KEY_N)) {
    mode = (mode + 1) % __SHOWCASE_MODES_COUNT;
  }
}

void dispose() {
	noisetex_dispose(noisetex_state);
}

void init() {
	dispose();

	// lvl 0
  noisetex_state = noisetex_init();
}

static long resize_timestamp = -1;
static const float resize_threshold = 0.3;
static Vector2 requested_viewport = { VIEWPORT_W, VIEWPORT_H };
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
	const bool resized = requested_viewport.x != viewport_w || requested_viewport.y != viewport_h;
	if (resized && now - resize_timestamp > resize_threshold) {
		resize_timestamp = now;
		viewport_w = vw;
		viewport_h = vh;
		init();
	}
}

void step(void) {
	equilizer();

	BeginDrawing();
		draw();
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

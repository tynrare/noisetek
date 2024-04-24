// @tynroar

#include "noise_tex.h"
#include "root.h"
#include <raylib.h>

bool active = false;

static const int VIEWPORT_W = 0x320;
static const int VIEWPORT_H = 0x1c2;

static int viewport_w = VIEWPORT_W;
static int viewport_h = VIEWPORT_H;

typedef enum SHOWCASE_MODES {
  SHOWCASE_MODE_ROOT = 0,
  __SHOWCASE_MODES_COUNT
} SHOWCASE_MODES;

typedef enum NOISETEK_MODES {
  NOISETEK_MODE_FULLSCREEN = 0,
  NOISETEK_MODE_THUMBNAIL,
  NOISETEK_MODE_HIDDEN,
  NOISETEK_MODE_SUSPEND,
  __NOISETEK_MODES_COUNT
} NOISETEK_MODES;

NoiseTexState *noisetex_state = NULL;
SHOWCASE_MODES showcase_mode = SHOWCASE_MODE_ROOT;
NOISETEK_MODES noisetek_mode = NOISETEK_MODE_FULLSCREEN;

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// ---

bool _draw_help = true;

// ---

void draw() {
  ClearBackground(BLACK);

  // Do not update noise in SUSTEND
  if (noisetek_mode != NOISETEK_MODE_SUSPEND) {
    noisetex_step(noisetex_state);
  }

  if (noisetek_mode != NOISETEK_MODE_FULLSCREEN) {
    switch (showcase_mode) {
    default:
    case SHOWCASE_MODE_ROOT:
      break;
    }
  }

  // Draw noise at screen in fullscreen and thumbnail modes
  if (noisetek_mode == NOISETEK_MODE_THUMBNAIL ||
      noisetek_mode == NOISETEK_MODE_FULLSCREEN) {
    Texture2D *rtt = &noisetex_state->render_texture.texture;
    float w = rtt->width;
    float h = rtt->height;
    Rectangle rsource = {0.0, 0.0, rtt->width, rtt->height};
    Rectangle rdest = rsource;
    if (noisetek_mode == NOISETEK_MODE_THUMBNAIL) {
      rdest = (Rectangle){8.0, 8.0, w / 2 - 8.0, h / 2 - 8.0};
    }
    DrawTexturePro(*rtt, rsource, rdest, (Vector2){0.0, 0.0}, 0.0, WHITE);
  }
}

void dispose() { noisetex_dispose(noisetex_state); }

void init() {
  dispose();

  // lvl 0
  noisetex_state = noisetex_init();
}

void inputs() {
  if (IsKeyPressed(KEY_H)) {
    _draw_help = !_draw_help;
  }
  if (IsKeyPressed(KEY_R)) {
    showcase_mode = (showcase_mode + 1) % __SHOWCASE_MODES_COUNT;
  }
  if (IsKeyPressed(KEY_N)) {
		noisetex_next_file(noisetex_state);
	}

  if (IsKeyPressed(KEY_ONE)) {
    noisetek_mode = NOISETEK_MODE_FULLSCREEN;
  } else if (IsKeyPressed(KEY_TWO)) {
    noisetek_mode = NOISETEK_MODE_THUMBNAIL;
  } else if (IsKeyPressed(KEY_THREE)) {
    noisetek_mode = NOISETEK_MODE_HIDDEN;
  } else if (IsKeyPressed(KEY_FOUR)) {
    noisetek_mode = NOISETEK_MODE_SUSPEND;
  }
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
  if (!_draw_help) {
    return;
  }

  const int fontsize = 20;
  const char *msg1 = "[H]elp toggle";
  const char *msg2 = "[R]otate cases";
  const char *msg_a0 = "[N]oisetek mode:";
  const char *msg3 = "[1] fullscreen";
  const char *msg4 = "[2] thumbnail";
  const char *msg5 = "[3] hidden";
  const char *msg6 = "[4] suspend";
  const char *msg7 = "---";
  DrawRectangle(4, 4, 200, 256, Fade(BLACK, 0.7));
  DrawText(msg1, 8, 8, fontsize, WHITE);
  DrawText(msg2, 8, 8 + (fontsize + 4) * 1, fontsize, WHITE);
  DrawText(msg_a0, 8, 8 + (fontsize + 4) * 2, fontsize, WHITE);
  DrawText(msg3, 16, 8 + (fontsize + 4) * 3, fontsize, WHITE);
  DrawText(msg4, 16, 8 + (fontsize + 4) * 4, fontsize, WHITE);
  DrawText(msg5, 16, 8 + (fontsize + 4) * 5, fontsize, WHITE);
  DrawText(msg6, 16, 8 + (fontsize + 4) * 6, fontsize, WHITE);
  DrawText(msg7, 8, 8 + (fontsize + 4) * 7, fontsize, WHITE);
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

#include "noise_tex.h"
#include "root.h"
#include <raylib.h>

#define LOAD_SSAA_NOISE


Texture2D NoiseTexGenerate(int width, int height) {
  int w = width;
  int h = height;
#ifndef LOAD_SSAA_NOISE
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
#else
  Texture2D texture = LoadTexture(RES_PATH "saa.png");
#endif

  return texture;
}

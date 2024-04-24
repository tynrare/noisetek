#include "shader_autoreload.h"
#include <raylib.h>
#include <rlgl.h>

void shader_ar_init(ShaderAutoReloadState *state, const char *filename) {
  state->fragShaderFileName = filename;
  state->fragShaderFileModTime = GetFileModTime(state->fragShaderFileName);

  // Load raymarching shader
  // NOTE: Defining 0 (NULL) for vertex shader forces usage of internal default
  // vertex shader
  state->shader = LoadShader(0, state->fragShaderFileName);

  // Get shader locations for required uniforms
  state->resolutionLoc = GetShaderLocation(state->shader, "resolution");
  state->mouseLoc = GetShaderLocation(state->shader, "pointer");
  state->timeLoc = GetShaderLocation(state->shader, "elapsed");

  float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
  SetShaderValue(state->shader, state->resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

  state->totalTime = GetTime();
  state->shaderAutoReloading = true;
}

ShaderAutoReloadStatus shader_ar_step(ShaderAutoReloadState *state) {
  state->totalTime = GetTime();
	Vector2 mouse = GetMousePosition();
	float mousePos[2] = {mouse.x, mouse.y};

	if (IsKeyPressed(KEY_A))
		state->shaderAutoReloading = !state->shaderAutoReloading;

	// Hot shader reloading
	if (state->shaderAutoReloading || (IsKeyPressed(KEY_R))) {
		long currentFragShaderModTime = GetFileModTime(state->fragShaderFileName);

		// Check if shader file has been modified
		if (currentFragShaderModTime != state->fragShaderFileModTime) {
			// Try reloading updated shader
			Shader updatedShader = LoadShader(0, state->fragShaderFileName);

			if (updatedShader.id !=
					rlGetShaderIdDefault()) // It was correctly loaded
			{
				UnloadShader(state->shader);
				state->shader = updatedShader;

				// Get shader locations for required uniforms
				state->resolutionLoc = GetShaderLocation(state->shader, "resolution");
				state->mouseLoc = GetShaderLocation(state->shader, "pointer");
				state->timeLoc = GetShaderLocation(state->shader, "elapsed");

				// Reset required uniforms
				float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
				SetShaderValue(state->shader, state->resolutionLoc, resolution,
											 SHADER_UNIFORM_VEC2);
			}

			state->fragShaderFileModTime = currentFragShaderModTime;

			return SHADER_AR_STATUS_RELOADED;
		}
	}

	// Set shader required uniform values
	SetShaderValue(state->shader, state->timeLoc, &state->totalTime, SHADER_UNIFORM_FLOAT);
	SetShaderValue(state->shader, state->mouseLoc, mousePos, SHADER_UNIFORM_VEC2);

	return SHADER_AR_STATUS_DEFAULT;
}

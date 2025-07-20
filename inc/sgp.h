/*
 * sgp.h - Sega Genesis Platform Abstraction Layer (header-only)
 *
 * MIT License
 *
 * Copyright (c) 2025 Spencer Vaughn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Purpose: Header-only platform abstraction layer for Sega Genesis development
 *          using SGDK. Provides ergonomic, high-performance input and camera
 *          helpers for Genesis games and demos.
 */
#ifndef SGP_H
#define SGP_H

#include <genesis.h>

#define BUTTON_NONE 0x0000

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
/**
 * @brief Input state for joypads.
 */
typedef struct {
    u16 joy1_state;      ///< Current state of joypad 1
    u16 joy2_state;      ///< Current state of joypad 2
    u16 joy1_previous;   ///< Previous state of joypad 1
    u16 joy2_previous;   ///< Previous state of joypad 2
} input;

/**
 * @brief Camera state for smooth scrolling and transformations.
 *
 * Uses fixed-point math for precision on the Genesis.
 */
typedef struct {
    fix16 x;      ///< Camera X position (fixed-point)
    fix16 y;      ///< Camera Y position (fixed-point)
    fix16 zoom;   ///< Camera zoom (fixed-point, 1.0 = default)
    fix16 rotation; ///< Camera rotation (fixed-point radians)
} camera;

/**
 * @brief Platform-wide state for input and camera.
 *
 * Holds current and previous joypad states for both controllers,
 * as well as camera position, zoom, and rotation in fixed-point.
 */
typedef struct {
    input input;         ///< Input state
    camera camera;       ///< Camera state
} SGP;

/**
 * @brief 2D camera structure for fixed-point math.
 *
 * Used for smooth scrolling and camera transforms.
 */
typedef struct {
    fix16 offset_x;   ///< Camera offset X
    fix16 offset_y;   ///< Camera offset Y
    fix16 target_x;   ///< Camera target X
    fix16 target_y;   ///< Camera target Y
    fix16 rotation;   ///< Camera rotation (fixed-point radians)
    fix16 zoom;       ///< Camera zoom (fixed-point, 1.0 = default)
} SGPCamera;

//----------------------------------------------------------------------------------
// Input Functions
//----------------------------------------------------------------------------------

/**
 * @brief Global platform state (must be defined in one .c file).
 */
extern SGP sgp;

/**
 * @brief Polls and updates the current and previous state for both controllers.
 *
 * Call this once per frame before reading input.
 */
static inline void SGP_PollInput(void) {
    sgp.input.joy1_previous = sgp.input.joy1_state;
    sgp.input.joy2_previous = sgp.input.joy2_state;
    sgp.input.joy1_state = JOY_readJoypad(JOY_1);
    sgp.input.joy2_state = JOY_readJoypad(JOY_2);
}

/**
 * @brief Returns true if the specified button(s) were just pressed (edge detection) for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask (e.g. BUTTON_A | BUTTON_B)
 */
static inline bool SGP_ButtonPressed(u16 joy, u16 button) {
    static u16* const state_ptrs[2] = { &sgp.input.joy1_state, &sgp.input.joy2_state };
    static u16* const prev_ptrs[2]  = { &sgp.input.joy1_previous, &sgp.input.joy2_previous };
    u16 state = *state_ptrs[joy & 1];
    u16 prev  = *prev_ptrs[joy & 1];
    return ((state & button) & ~(prev & button)) != 0;
}

/**
 * @brief Returns true if the specified button(s) were just released for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonReleased(u16 joy, u16 button) {
    static u16* const state_ptrs[2] = { &sgp.input.joy1_state, &sgp.input.joy2_state };
    static u16* const prev_ptrs[2]  = { &sgp.input.joy1_previous, &sgp.input.joy2_previous };
    u16 state = *state_ptrs[joy & 1];
    u16 prev  = *prev_ptrs[joy & 1];
    return ((~state & button) & (prev & button)) != 0;
}

/**
 * @brief Returns true if the specified button(s) are currently held down for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonDown(u16 joy, u16 button) {
    static u16* const state_ptrs[2] = { &sgp.input.joy1_state, &sgp.input.joy2_state };
    u16 state = *state_ptrs[joy & 1];
    return (state & button) != 0;
}

//----------------------------------------------------------------------------------
// Camera Functions (Fixed Point for Genesis)
//----------------------------------------------------------------------------------

/**
 * @brief Initializes a Camera struct with default values and a given target position.
 * @param cam Pointer to Camera struct
 * @param x   Target X position
 * @param y   Target Y position
 */
static inline void SGP_CameraInit(SGPCamera* cam, fix16 x, fix16 y) {
    cam->offset_x = FIX16(0);
    cam->offset_y = FIX16(0);
    cam->target_x = x;
    cam->target_y = y;
    cam->rotation = FIX16(0);
    cam->zoom = FIX16(1);
}

/**
 * @brief Smoothly moves the camera target toward a new position using a smoothing factor.
 * @param cam Pointer to Camera struct
 * @param target_x Target X
 * @param target_y Target Y
 * @param smooth Smoothing factor (0-1 in fix16)
 */
static inline void SGP_CameraSmoothFollow(SGPCamera* cam, fix16 target_x, fix16 target_y, fix16 smooth) {
    cam->target_x = cam->target_x + fix16Mul((target_x - cam->target_x), smooth);
    cam->target_y = cam->target_y + fix16Mul((target_y - cam->target_y), smooth);
}

/**
 * @brief Clamps the camera target position within the specified bounds.
 * @param cam Pointer to SGPCamera struct
 * @param min_x Minimum X
 * @param min_y Minimum Y
 * @param max_x Maximum X
 * @param max_y Maximum Y
 */
static inline void SGP_CameraClamp(SGPCamera* cam, fix16 min_x, fix16 min_y, fix16 max_x, fix16 max_y) {
    if (cam->target_x < min_x) cam->target_x = min_x;
    if (cam->target_x > max_x) cam->target_x = max_x;
    if (cam->target_y < min_y) cam->target_y = min_y;
    if (cam->target_y > max_y) cam->target_y = max_y;
}

//----------------------------------------------------------------------------------
// Platform Initialization
//----------------------------------------------------------------------------------

/**
 * @brief Initializes default Sega Genesis Platform Abstraction Layer (SGPal) state.
 * Call once at startup.
 */
static inline void SGP_init(void) {
    sgp.input.joy1_state = 0;
    sgp.input.joy2_state = 0;
    sgp.input.joy1_previous = 0;
    sgp.input.joy2_previous = 0;
    sgp.camera.x = FIX16(160);
    sgp.camera.y = FIX16(112);
    sgp.camera.zoom = FIX16(1);
    sgp.camera.rotation = FIX16(0);
}

#endif // SGP_H

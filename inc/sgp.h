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
 *          helpers for Genesis development.
 */
#ifndef SGP_H
#define SGP_H

#include <genesis.h>

#define BUTTON_NONE 0x0000
#define VDP_SPRITE_OFFSET 0x80 // Offset for sprite coordinates in VDP

// Each metatile is 16x16 pixels, so 128x128 pixels block is 8x8 metatiles
static inline int __inPx(int x) { return x << 7; }

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
/**
 * @brief 2D camera structure for fixed-point math.
 *
 * Used for smooth scrolling and camera transforms.
 */
typedef struct {
    fix16 offset_x;   ///< Camera offset X
    fix32 offset_y;   ///< Camera offset Y
    fix32* target_x;   ///< Camera target X
    fix32* target_y;   ///< Camera target Y
    u8 type;          ///< Camera type (e.g. CAMERA_SMOOTH)
    int current_x;            ///< Camera X position (integer for MAP_scrollTo)
    int current_y;            ///< Camera Y position (integer for MAP_scrollTo)
    Sprite* sprite;
    s16 sprite_width;  ///< Width of the sprite being followed
    s16 sprite_height; ///< Height of the sprite being followed
    bool active;
} SGPCamera;

typedef struct {
    Map* current;
    Map* previous;
    u16 height;
    u16 width;
} SGPMap;

/**
 * @brief Platform-wide state for input and camera.
 *
 * Holds current and previous joypad states for both controllers,
 * as well as camera position, zoom, and rotation in fixed-point.
 */
typedef struct {
    input input;         ///< Input state
    SGPCamera camera;    ///< Camera state
    SGPMap map;          ///< Current map state
} SGP;

/**
 * @brief Global platform state (must be defined in one .c file).
 */
extern SGP sgp;

static bool showDebug = FALSE;
static inline void toggleDebug(void) {
    showDebug = showDebug ? FALSE : TRUE;
}
static inline void SGP_DebugPrint(const char* text, int x, int y) {
    if (showDebug) {
        VDP_drawText(text, x, y);
    } else {
        VDP_clearTextArea(x, y, 32, 8); // Clear the debug text if not showing
    }
}

//----------------------------------------------------------------------------------
// Input Functions
//----------------------------------------------------------------------------------

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
typedef struct {
    Sprite* sprite;  ///< Sprite to follow
    fix32* target_x_ptr; ///< Target X position (fixed-point)
    fix32* target_y_ptr; ///< Target Y Position (fixed-point)
    int sprite_width;  ///< Width of the sprite being followed
    int sprite_height; ///< Height of the sprite being followed
} SGPCameraTarget;

/**
 * @brief Initializes a Camera struct with default values and a given target position.
 * @param cam Pointer to Camera struct
 * @param current_x   Target X position
 * @param current_y   Target Y position
 */
static inline void SGP_CameraInit(const Map* map) {
    sgp.map.current = map;
    sgp.map.height = __inPx(map->h);
    sgp.map.width = __inPx(map->w);
    sgp.camera.active = TRUE;
}

static inline void SGP_ClampPositionToMapBounds(fix32* x, fix32* y, s16 width, s16 height) {
    int pos_x = F32_toInt(*x);
    int pos_y = F32_toInt(*y);

    // Clamp position to map bounds (entity always visible on map, accounting for sprite dimensions)
    if (pos_x < 0) *x = FIX32(0);
    if (pos_x > sgp.map.width - 1 - width) *x = FIX32(sgp.map.width - 1 - width);
    if (pos_y < 0) *y = FIX32(0);
    if (pos_y > sgp.map.height - height) *y = FIX32(sgp.map.height - height);
}

static inline void SGP_CameraFollowTarget(SGPCameraTarget* target) {
    if (!sgp.camera.active) {
        return; // Camera not active, skip following
    }
    int target_x_map = F32_toInt(*target->target_x_ptr);
    int target_y_map = F32_toInt(*target->target_y_ptr);

    // Center camera on target, but clamp camera to map bounds
    int new_camera_x = target_x_map - (screenWidth / 2) + (target->sprite_width / 2);
    int new_camera_y = target_y_map - (screenHeight / 2) + (target->sprite_height / 2);

    if (new_camera_x < 0) new_camera_x = 0;
    if (new_camera_x > sgp.map.width - screenWidth) new_camera_x = sgp.map.width - screenWidth;
    if (new_camera_y < 0) new_camera_y = 0;
    if (new_camera_y > sgp.map.height - screenHeight) new_camera_y = sgp.map.height - screenHeight;

    sgp.camera.current_x = new_camera_x;
    sgp.camera.current_y = new_camera_y;

    MAP_scrollTo(sgp.map.current, new_camera_x, new_camera_y);
    if (target->sprite) {
        SPR_setPosition(target->sprite,
            target_x_map - new_camera_x,
            target_y_map - new_camera_y);
    }
}

static inline void SGP_activateCamera(void) {
    sgp.camera.active = TRUE;
}
static inline void SGP_deactivateCamera(void) {
    sgp.camera.active = FALSE;
}

static inline bool SGP_isCameraActive(void) {
    return sgp.camera.active;
}

static inline void SGP_UpdateCameraPosition(int x, int y) {
    if (sgp.camera.active) {
        return; // if active, camera tracks target
    }
    sgp.camera.current_x = x;
    sgp.camera.current_y = y;
    MAP_scrollTo(sgp.map.current, x, y);
}

static inline void SGP_ShakeCamera(int duration, int intensity) {
    SGP_deactivateCamera(); // Disable camera tracking during shake
    for (int i = 0; i < duration; i++) {
        int shake_x = (i % 2 == 0) ? intensity : -intensity;
        sgp.camera.current_x += shake_x;
        SGP_UpdateCameraPosition(sgp.camera.current_x, sgp.camera.current_y);
        SYS_doVBlankProcess(); // Wait for VBlank to apply shake
        sgp.camera.current_x -= shake_x; // Restore position for next frame
    }
    SGP_activateCamera(); // Re-enable camera tracking after shake
}

/**
 * @brief Initializes default Sega Genesis Platform Abstraction Layer (SGP) state.
 * Call once at startup.
 */
static inline void SGP_init(void) {
    sgp.input.joy1_state = 0;
    sgp.input.joy2_state = 0;
    sgp.input.joy1_previous = 0;
    sgp.input.joy2_previous = 0;
    sgp.camera.offset_x = 0;
    sgp.camera.offset_y = 0;
    sgp.camera.target_x = 0;
    sgp.camera.target_y = 0;
    sgp.camera.current_x = 0;
    sgp.camera.current_y = 0;
}

#endif // SGP_H

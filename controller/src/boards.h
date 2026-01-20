#pragma once

// Board configuration header
// Select board by building the appropriate PlatformIO environment:
//   - pio run -e board_a1s_mux      (A1S with MUX for aux buttons)
//   - pio run -e board_a1s_simple   (A1S without MUX)
//
// Or select from the PlatformIO environment dropdown in VSCode

// =============================================================================
// Default values (can be overridden by board-specific defines)
// =============================================================================

// ES8388 Codec I2C pins
#ifndef I2C_SDA
#define I2C_SDA             33
#endif
#ifndef I2C_SCL
#define I2C_SCL             32
#endif

// I2S pins for ES8388 codec
#ifndef I2S_MCLK
#define I2S_MCLK            0
#endif
#ifndef I2S_BCLK
#define I2S_BCLK            27
#endif
#ifndef I2S_LRC
#define I2S_LRC             25
#endif
#ifndef I2S_DOUT
#define I2S_DOUT            26
#endif

// Amplifier enable pin
#ifndef GPIO_PA_EN
#define GPIO_PA_EN          21
#endif

// SD Card pins (directly wired 1-bit mode)
#ifndef SD_MMC_CMD
#define SD_MMC_CMD          15
#endif
#ifndef SD_MMC_CLK
#define SD_MMC_CLK          14
#endif
#ifndef SD_MMC_D0
#define SD_MMC_D0           2
#endif

// Rotary dial pins
#ifndef DIAL_1
#define DIAL_1              19
#endif
#ifndef DIAL_2_IN_MOTION
#define DIAL_2_IN_MOTION    22
#endif

// =============================================================================
// Board: A1S with MUX (BOARD_A1S_MUX)
// =============================================================================
#if defined(BOARD_A1S_MUX)

#define AUX_BUTTONS_AND_MUX     true
#define HOOK_SWITCH_PIN         -1      // Hook switch on MUX channel 7

// MUX control pins
#define MUX_SIG_PIN             13
#define MUX_S0_PIN              32
#define MUX_S1_PIN              12
#define MUX_S2_PIN              27

// Input MUX pins
#define INPUT_MUX_SIG_PIN       35
#define INPUT_MUX_S0_PIN        5
#define INPUT_MUX_S1_PIN        4
#define INPUT_MUX_S2_PIN        0

// =============================================================================
// Board: A1S Simple - no MUX (BOARD_A1S_SIMPLE)
// =============================================================================
#elif defined(BOARD_A1S_SIMPLE)

#define AUX_BUTTONS_AND_MUX     false
#define HOOK_SWITCH_PIN         23      // Direct GPIO for hook switch

// =============================================================================
// Default fallback (matches original code behavior)
// =============================================================================
#else

#warning "No board defined, using defaults (BOARD_A1S_SIMPLE behavior)"
#define AUX_BUTTONS_AND_MUX     false
#define HOOK_SWITCH_PIN         23

#endif

/* Copyright (C) 2023 jonylee@hfd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

#if __has_include("macros.h")
#include "macros.h"
#else
#define MACRO_STRING1 ""
#define MACRO_STRING2 ""
#define MACRO_STRING3 ""
#define MACRO_STRING4 ""
#define MACRO_STRING5 ""
#endif

typedef tap_dance_state_t qk_tap_dance_state_t;
typedef tap_dance_action_t qk_tap_dance_action_t;

// Tap dance

typedef enum {
    TAP_STATE_RESET,
    SINGLE_TAP,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    TRIPLE_TAP,
    TRIPLE_HOLD,
    SUCCESSIVE_TAP,
    SUCCESSIVE_HOLD,
} tap_state_t;

// Global TapDance State
static struct t_tap {
    tap_state_t quotes;
    tap_state_t grave;
    tap_state_t magic;
} qk_tap_state = {
    .quotes = 0,
    .grave = 0,
    .magic = 0,
};

/* Sentinel value for invalid tap dance exit */
#define TAP_DANCE_NO_MATCH 64

tap_state_t get_tapdance_state(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) {
            return SINGLE_TAP;
        } else {
            return SINGLE_HOLD;
        }
    } else if (state->count == 2) {
        if (state->interrupted || !state->pressed) {
            return DOUBLE_TAP;
        } else {
            return DOUBLE_HOLD;
        }
    } else if (state->count == 3) {
        if (state->interrupted || !state->pressed) {
            return TRIPLE_TAP;
        } else {
            return TRIPLE_HOLD;
        }
    } else {
        if (state->interrupted || !state->pressed) {
            return SUCCESSIVE_TAP;
        } else {
            return SUCCESSIVE_HOLD;
        }
    }
}

void td_quotes_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.quotes = get_tapdance_state(state);
    switch (qk_tap_state.quotes) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            register_code16(KC_QUOTE);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            register_code16(KC_DOUBLE_QUOTE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            register_code16(KC_PIPE);
        default:
            break;
    }
}

void td_quotes_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.quotes) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            unregister_code16(KC_QUOTE);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            unregister_code16(KC_DOUBLE_QUOTE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            unregister_code16(KC_PIPE);
            break;
        default:
            break;
    }
    qk_tap_state.quotes = 0;
}

void td_grave_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.grave = get_tapdance_state(state);
    switch (qk_tap_state.grave) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            register_code16(KC_GRV);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            register_code16(KC_TILDE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            SEND_STRING("```");
            break;
        default:
            break;
    }
}

void td_grave_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.grave) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
        case SUCCESSIVE_TAP:
        case SUCCESSIVE_HOLD:
            unregister_code16(KC_GRV);
            break;
        case DOUBLE_TAP:
        case DOUBLE_HOLD:
            unregister_code16(KC_TILDE);
            break;
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            break;
        default:
            break;
    }
    qk_tap_state.grave = 0;
}

void td_magic_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_state.magic = get_tapdance_state(state);
    switch (qk_tap_state.magic) {
        case SINGLE_TAP: {
            SEND_STRING(SS_LSFT(SS_LCTL(SS_LALT(SS_LGUI("x"))))); // Hyper-X
            break;
        }
        case DOUBLE_TAP: {
            SEND_STRING(SS_LSFT(SS_LCTL(SS_LALT(SS_LGUI("y"))))); // Hyper-Y
            break;
        }
        case TRIPLE_TAP: {
            SEND_STRING(SS_LSFT(SS_LCTL(SS_LALT(SS_LGUI("z"))))); // Hyper-Z
            break;
        }
        case SINGLE_HOLD:
        case DOUBLE_HOLD:
        case TRIPLE_HOLD:
        case SUCCESSIVE_HOLD:
            register_mods(MOD_LGUI|MOD_LALT|MOD_LCTL);
            break;
        default:
            break;
    }
}

void td_magic_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (qk_tap_state.magic) {
        case SINGLE_TAP:
        case DOUBLE_TAP:
        case TRIPLE_TAP:
        case SUCCESSIVE_TAP:
            break;
        case SINGLE_HOLD:
        case DOUBLE_HOLD:
        case TRIPLE_HOLD:
        case SUCCESSIVE_HOLD:
            unregister_mods(MOD_LGUI|MOD_LALT|MOD_LCTL);
            break;
        default:
            break;
    }
    qk_tap_state.magic = 0;
}

// Tap Dance Declarations
enum tapdance_keycodes {
    TD_QUOTES,
    TD_GRAVE,
    TD_APP_MAGIC,
    TD_5_F5,
    TD_7_F7,
    TD_8_F8,
    TD_9_F9,
};

// Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
    // once: single quote, twice: double quote, thrice: backtick
    [TD_QUOTES] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_quotes_finished, td_quotes_reset),

    // once: `, twice: ~, thrice: `|`
    [TD_GRAVE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_grave_finished, td_grave_reset),

    // once: mute, hold: Ctrl
    [TD_APP_MAGIC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_magic_finished, td_magic_reset),

    [TD_5_F5] = ACTION_TAP_DANCE_DOUBLE(KC_5, KC_F5),
    [TD_7_F7] = ACTION_TAP_DANCE_DOUBLE(KC_7, KC_F7),
    [TD_8_F8] = ACTION_TAP_DANCE_DOUBLE(KC_8, KC_F8),
    [TD_9_F9] = ACTION_TAP_DANCE_DOUBLE(KC_9, KC_F9),
};

#define TD_QUOT TD(TD_QUOTES)
#define TD_GRV  TD(TD_GRAVE)
#define TD_5    TD(TD_5_F5)
#define TD_7    TD(TD_7_F7)
#define TD_8    TD(TD_8_F8)
#define TD_9    TD(TD_9_F9)
#define TD_MAGIC TD(TD_APP_MAGIC)

// Left-hand home row mods
#define HOME_A LCTL_T(KC_A)
#define HOME_S LALT_T(KC_S)
#define HOME_D LGUI_T(KC_D)
#define HOME_F LSFT_T(KC_F)

// Right-hand home row mods
#define HOME_J RSFT_T(KC_J)
#define HOME_K RGUI_T(KC_K)
#define HOME_L LALT_T(KC_L)
#define HOME_SCLN RCTL_T(KC_SCLN)

#define MT_LSFT MT(MOD_LSFT, KC_MINUS)
#define MT_RSFT MT(MOD_RSFT, KC_QUOTE)
#define MT_LCTL MT(MOD_LCTL, KC_LPRN)
#define MT_LALT MT(MOD_LALT, KC_LBRC)
#define MT_LGUI MT(MOD_LGUI, KC_LCBR)

#define OS_LSFT OSM(MOD_LSFT)
#define OS_RSFT OSM(MOD_RSFT)
#define OS_LCTL OSM(MOD_LCTL)
#define OS_LALT OSM(MOD_LALT)
#define OS_LGUI OSM(MOD_LGUI)

#define TR_SCLN TRAILING_SEMICOLON
#define TR_COMM TRAILING_COMMA
#define TR_RARR RIGHT_ARROW

#define CMD_Z LGUI(KC_Z)
#define CMD_X LGUI(KC_X)
#define CMD_C LGUI(KC_C)
#define CMD_V LGUI(KC_V)
#define HY_TAB HYPR(KC_TAB)

enum planck_keycodes {
  QWERTY = SAFE_RANGE,
  BACKLIT,
  TRAILING_SEMICOLON,
  TRAILING_COMMA,
  RIGHT_ARROW,
  COLEMAK,
  DVORAK,
  PLOVER,
  EXT_PLV,

  MACKEY1,
  MACKEY2,
  MACKEY3,
  MACKEY4,
  MACKEY5,
};

#define TD_QUOT TD(TD_QUOTES)
#define TD_GRV  TD(TD_GRAVE)
#define TD_5    TD(TD_5_F5)
#define TD_7    TD(TD_7_F7)
#define TD_8    TD(TD_8_F8)
#define TD_9    TD(TD_9_F9)
#define TD_MAGIC TD(TD_APP_MAGIC)

// Left-hand home row mods
#define HOME_A LCTL_T(KC_A)
#define HOME_S LALT_T(KC_S)
#define HOME_D LGUI_T(KC_D)
#define HOME_F LSFT_T(KC_F)

// Right-hand home row mods
#define HOME_J RSFT_T(KC_J)
#define HOME_K RGUI_T(KC_K)
#define HOME_L LALT_T(KC_L)
#define HOME_SCLN RCTL_T(KC_SCLN)

// Layers

enum planck_layers {
  _QWERTY,
  _RAISE,
  _LOWER,
  _ADJUST,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_QWERTY] = LAYOUT(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        KC_ESC,  HOME_A,  HOME_S,  HOME_D,  HOME_F,  KC_G,    KC_H,    HOME_J,  HOME_K,  HOME_L,  HOME_SCLN, KC_ENT,
        MT_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, MT_RSFT,
        KC_HYPR, KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

    [_LOWER] = LAYOUT(
        KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR,    KC_ASTR,    KC_LPRN, KC_RPRN, KC_BSPC,
        KC_DEL,  _______, _______, _______, _______, KC_LPRN, KC_RPRN, KC_MINS,    KC_PLUS,    KC_LCBR, KC_RCBR, TR_SCLN,
        OS_LSFT, _______, _______, _______, _______, KC_LBRC, KC_RBRC, KC_EQL,     KC_LABK,    KC_RABK, KC_BSLS, OS_RSFT,
        _______, _______, _______, _______, _______, TR_RARR, _______,    KC_HOME,    KC_PGDN, KC_PGUP, KC_END
    ),

    [_RAISE] = LAYOUT(
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_DOWN, KC_UP,   KC_LBRC, KC_RBRC, KC_PGUP,
        KC_LSFT, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_HOME, KC_END,  KC_PIPE, KC_PGDN,
        _______, _______, _______, _______, _______, KC_UNDS, _______, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPLY
    ),

    [_ADJUST] = LAYOUT(
        _______, QK_BOOT, DB_TOGG, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, RGB_MOD,
        RGB_HUI, _______, _______, _______, _______,  AG_NORM, AG_SWAP, MACKEY1, MACKEY2, MACKEY3, MACKEY4, _______,
        _______, _______, _______, _______, _______,  MI_ON,   MI_OFF,  MACKEY5, _______, _______, RGB_VAI, _______,
        _______, _______, _______, _______, _______, _______,           _______, _______, RGB_SPD, RGB_VAD, RGB_SPI
    ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case TRAILING_SEMICOLON:
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_END)";");
            return false;
        }
        break;
    case TRAILING_COMMA:
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_END)",");
            return false;
        }
        break;
    case RIGHT_ARROW:
        if (record->event.pressed) {
            SEND_STRING("->");
            return false;
        }
        break;
    case MACKEY1:
      if (record->event.pressed) {
         send_string_with_delay_P(MACRO_STRING1, 10);
         return false;
      }
      break;
    case MACKEY2:
      if (record->event.pressed) {
        send_string_with_delay_P(MACRO_STRING2, 10);
        return false;
      }
      break;
    case MACKEY3:
      if (record->event.pressed) {
        send_string_with_delay_P(MACRO_STRING3, 10);
        return false;
      }
      break;
    case MACKEY4:
      if (record->event.pressed) {
        send_string_with_delay_P(MACRO_STRING4, 10);
        return false;
      }
      break;
    case MACKEY5:
      if (record->event.pressed) {
        send_string_with_delay_P(MACRO_STRING5, 10);
        return false;
      }
      break;
  }
  return true;
}
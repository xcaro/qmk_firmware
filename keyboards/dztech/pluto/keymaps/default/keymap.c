/* Copyright 2023 DZTECH <moyi4681@Live.cn>
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

// Custom keycode: tap = toggle default layer (macOS <-> Windows), hold = MO(2)
enum custom_keycodes {
    SW_OS = SAFE_RANGE,
};

static uint16_t sw_os_timer     = 0;
static uint16_t sw_os_dbl_timer = 0;
static bool     sw_os_pending   = false; // waiting for possible 2nd tap

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_BSPC:
            if (get_highest_layer(default_layer_state) == 0) {
                uint8_t mods = get_mods();
                bool ctrl  = mods & MOD_MASK_CTRL;
                bool shift = mods & MOD_MASK_SHIFT;

                if (ctrl && shift) {
                    if (record->event.pressed) {
                        register_code(KC_EJCT);
                    } else {
                        unregister_code(KC_EJCT);
                    }
                    return false;
                }
            }
            return true;
        case SW_OS:
            if (record->event.pressed) {
                sw_os_timer = timer_read();
                layer_on(2); // activate Fn layer immediately on press (hold behavior)
            } else {
                layer_off(2);
                if (timer_elapsed(sw_os_timer) < TAPPING_TERM) {
                    // This was a tap — check for double tap
                    if (sw_os_pending && timer_elapsed(sw_os_dbl_timer) < TAPPING_TERM * 2) {
                        // Second tap within window: toggle default layer
                        sw_os_pending = false;
                        if (get_highest_layer(default_layer_state) == 0) {
                            set_single_persistent_default_layer(1);
                        } else {
                            set_single_persistent_default_layer(0);
                        }
                    } else {
                        // First tap: start waiting for second tap
                        sw_os_pending   = true;
                        sw_os_dbl_timer = timer_read();
                    }
                } else {
                    // Was a hold, cancel any pending double-tap
                    sw_os_pending = false;
                }
            }
            return false;
        default:
            return true;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Layer 0 — Base (macOS): LCtrl | LAlt | LGui | ... | RGui | RAlt | SW_OS | RCtrl
    [0] = LAYOUT_all(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,     KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_NUHS,             KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_EJCT,                      KC_UP,
        KC_LCTL, KC_LALT, KC_LGUI,                            KC_SPC,                             KC_RGUI, KC_RALT, SW_OS,   KC_RCTL,             KC_LEFT, KC_DOWN, KC_RGHT
    ),

    // Layer 1 — Base (Windows): same layout, same SW_OS key to toggle back
    [1] = LAYOUT_all(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,     KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_NUHS,             KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_EJCT,                      KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, SW_OS,   KC_RCTL,             KC_LEFT, KC_DOWN, KC_RGHT
    ),

    // Layer 2 — Function (hold SW_OS)
    [2] = LAYOUT_all(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______,    _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                      _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______,             _______, _______, _______
    )
};

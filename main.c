#include <stdint.h>
#include "stm32f429_regs.h"
#include "model_weights.h"
#include "spi.h"
#include "ili9341.h"
#include "stmpe811.h"

static void SystemClock_Config(void) {
    RCC->CR |= (1 << 16);
    while (!(RCC->CR & (1 << 17)));

    *((volatile uint32_t *)0x40023C00) = 0x705;

    RCC->PLLCFGR = (8U)          |
                   (336U << 6)   |
                   (0U  << 16)   |
                   (1U  << 22)   |
                   (7U  << 24);

    RCC->CR |= (1 << 24);
    while (!(RCC->CR & (1 << 25)));

    RCC->CFGR = (0U << 4)  |
                (5U << 10) |
                (4U << 13);

    RCC->CFGR |= (2U << 0);
    while ((RCC->CFGR & (3U << 2)) != (2U << 2));
}

static void delay_ms(volatile uint32_t ms) {
    while (ms--) {
        volatile uint32_t count = 56000;
        while (count--) { __asm__("nop"); }
    }
}

const char EMNIST_LABELS[47] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z',
    'A', 'B', 'D', 'E', 'F', 'G', 'H', 'N', 'Q', 'R', 'T'
};

static void compute_layer1(const int8_t *input, int32_t *out) {
    for (int i = 0; i < 128; i++) {
        int32_t acc = 0;
        for (int j = 0; j < 784; j++) {
            acc += (int32_t)input[j] * (int32_t)layer1_weights[i * 784 + j];
        }
        acc += layer1_bias[i];
        if (acc < 0) acc = 0;
        out[i] = acc;
    }
}

static void compute_layer2(const int32_t *input, int32_t *out) {
    for (int i = 0; i < 47; i++) {
        int32_t acc = 0;
        for (int j = 0; j < 128; j++) {
            acc += input[j] * (int32_t)layer2_weights[i * 128 + j];
        }
        acc += layer2_bias[i];
        out[i] = acc;
    }
}

static void find_top2(const int32_t *logits, int *top1, int *top2) {
    *top1 = 0;
    *top2 = 1;
    if (logits[1] > logits[0]) {
        *top1 = 1;
        *top2 = 0;
    }

    for (int i = 2; i < 47; i++) {
        if (logits[i] > logits[*top1]) {
            *top2 = *top1;
            *top1 = i;
        } else if (logits[i] > logits[*top2]) {
            *top2 = i;
        }
    }
}

#define BOX_X 22
#define BOX_Y 45
#define SCALE 7
#define BOX_SIZE (28 * SCALE)

#define BTN_PRED_X 12
#define BTN_PRED_Y 282
#define BTN_PRED_W 102
#define BTN_PRED_H 32

#define BTN_CLEAR_X 126
#define BTN_CLEAR_Y 282
#define BTN_CLEAR_W 102
#define BTN_CLEAR_H 32

static int8_t user_canvas[784];
static int8_t centered_canvas[784];
static int8_t transposed_canvas[784];

static void center_and_normalize_drawing(void) {
    int32_t total_mass = 0;
    int32_t sum_r = 0;
    int32_t sum_c = 0;

    for (int r = 0; r < 28; r++) {
        for (int c = 0; c < 28; c++) {
            int8_t val = user_canvas[r * 28 + c];
            if (val > 0) {
                total_mass += val;
                sum_r += r * val;
                sum_c += c * val;
            }
        }
    }

    for (int i = 0; i < 784; i++) {
        centered_canvas[i] = 0;
    }

    if (total_mass == 0) return;

    int center_r = sum_r / total_mass;
    int center_c = sum_c / total_mass;

    int shift_r = 14 - center_r;
    int shift_c = 14 - center_c;

    for (int r = 0; r < 28; r++) {
        for (int c = 0; c < 28; c++) {
            int8_t val = user_canvas[r * 28 + c];
            if (val > 0) {
                int nr = r + shift_r;
                int nc = c + shift_c;
                if (nr >= 0 && nr < 28 && nc >= 0 && nc < 28) {
                    centered_canvas[nr * 28 + nc] = val;
                }
            }
        }
    }
}

static void draw_button_frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    for (int i = 0; i < w; i++) {
        ili9341_draw_pixel(x + i, y, color);
        ili9341_draw_pixel(x + i, y + h, color);
    }
    for (int i = 0; i < h; i++) {
        ili9341_draw_pixel(x, y + i, color);
        ili9341_draw_pixel(x + w, y + i, color);
    }
}

static void draw_ui_layout(void) {
    ili9341_fill_screen(ILI9341_BLACK);

    ili9341_draw_string(20, 10, "EMNIST TINYML AI", ILI9341_WHITE, ILI9341_BLACK);
    ili9341_draw_string(20, 26, "STM32 BARE-METAL", ILI9341_CYAN, ILI9341_BLACK);

    for (int i = 0; i < BOX_SIZE + 4; i++) {
        ili9341_draw_pixel(BOX_X - 2 + i, BOX_Y - 2, ILI9341_CYAN);
        ili9341_draw_pixel(BOX_X - 2 + i, BOX_Y - 1, ILI9341_CYAN);
        ili9341_draw_pixel(BOX_X - 2 + i, BOX_Y + BOX_SIZE, ILI9341_CYAN);
        ili9341_draw_pixel(BOX_X - 2 + i, BOX_Y + BOX_SIZE + 1, ILI9341_CYAN);

        ili9341_draw_pixel(BOX_X - 2, BOX_Y - 2 + i, ILI9341_CYAN);
        ili9341_draw_pixel(BOX_X - 1, BOX_Y - 2 + i, ILI9341_CYAN);
        ili9341_draw_pixel(BOX_X + BOX_SIZE, BOX_Y - 2 + i, ILI9341_CYAN);
        ili9341_draw_pixel(BOX_X + BOX_SIZE + 1, BOX_Y - 2 + i, ILI9341_CYAN);
    }

    ili9341_draw_string(18, 246, "PREDICTION:  -", ILI9341_WHITE, ILI9341_BLACK);
    ili9341_draw_string(18, 264, "STATUS: DRAW CHARACTER", 0xFFE0, ILI9341_BLACK);

    draw_button_frame(BTN_PRED_X, BTN_PRED_Y, BTN_PRED_W, BTN_PRED_H, ILI9341_GREEN);
    ili9341_draw_string(BTN_PRED_X + 16, BTN_PRED_Y + 12, "[ PREDICT ]", ILI9341_GREEN, ILI9341_BLACK);

    draw_button_frame(BTN_CLEAR_X, BTN_CLEAR_Y, BTN_CLEAR_W, BTN_CLEAR_H, ILI9341_RED);
    ili9341_draw_string(BTN_CLEAR_X + 22, BTN_CLEAR_Y + 12, "[ CLEAR ]", ILI9341_WHITE, ILI9341_BLACK);
}

static void clear_canvas(void) {
    for (int i = 0; i < 784; i++) {
        user_canvas[i] = 0;
        centered_canvas[i] = 0;
        transposed_canvas[i] = 0;
    }
    ili9341_fill_rect(BOX_X, BOX_Y, BOX_SIZE, BOX_SIZE, ILI9341_BLACK);
    ili9341_draw_string(18, 246, "PREDICTION:  -         ", ILI9341_WHITE, ILI9341_BLACK);
    ili9341_draw_string(18, 264, "STATUS: DRAW CHARACTER ", 0xFFE0, ILI9341_BLACK);
}

static void plot_brush_cell(int r, int c) {
    if (r < 0 || r >= 28 || c < 0 || c >= 28) return;
    user_canvas[r * 28 + c] = 127;
    ili9341_fill_rect(BOX_X + c * SCALE, BOX_Y + r * SCALE, SCALE, SCALE, ILI9341_WHITE);
}

static void plot_brush(int r, int c) {
    plot_brush_cell(r, c);
    plot_brush_cell(r - 1, c);
    plot_brush_cell(r + 1, c);
    plot_brush_cell(r, c - 1);
    plot_brush_cell(r, c + 1);
}

static void run_prediction_pipeline(void) {
    ili9341_draw_string(18, 264, "STATUS: INFERRING...   ", ILI9341_CYAN, ILI9341_BLACK);

    center_and_normalize_drawing();

    for (int r = 0; r < 28; r++) {
        for (int c = 0; c < 28; c++) {
            transposed_canvas[r * 28 + c] = centered_canvas[c * 28 + r];
        }
    }

    static int32_t hidden[128];
    static int32_t logits[47];

    compute_layer1(transposed_canvas, hidden);
    compute_layer2(hidden, logits);

    int top1, top2;
    find_top2(logits, &top1, &top2);

    char pred_str[24];
    pred_str[0] = 'P';
    pred_str[1] = 'R';
    pred_str[2] = 'E';
    pred_str[3] = 'D';
    pred_str[4] = ':';
    pred_str[5] = ' ';
    pred_str[6] = EMNIST_LABELS[top1];
    pred_str[7] = ' ';
    pred_str[8] = '(';
    pred_str[9] = 'A';
    pred_str[10] = 'L';
    pred_str[11] = 'T';
    pred_str[12] = ':';
    pred_str[13] = ' ';
    pred_str[14] = EMNIST_LABELS[top2];
    pred_str[15] = ')';
    pred_str[16] = ' ';
    pred_str[17] = ' ';
    pred_str[18] = '\0';

    ili9341_draw_string(18, 246, pred_str, ILI9341_GREEN, ILI9341_BLACK);
    ili9341_draw_string(18, 264, "STATUS: DONE!          ", ILI9341_WHITE, ILI9341_BLACK);
}

int main(void) {
    SystemClock_Config();

    spi5_init();
    ili9341_init();
    stmpe811_init();

    draw_ui_layout();

    uint16_t last_tx = 0, last_ty = 0;
    uint8_t touch_was_down = 0;

    while (1) {
        uint16_t tx = 0, ty = 0;
        if (stmpe811_read_touch(&tx, &ty)) {
            if (tx >= BTN_CLEAR_X && tx <= (BTN_CLEAR_X + BTN_CLEAR_W) &&
                ty >= BTN_CLEAR_Y && ty <= (BTN_CLEAR_Y + BTN_CLEAR_H)) {
                clear_canvas();
                delay_ms(250);
                touch_was_down = 0;
                continue;
            }

            if (tx >= BTN_PRED_X && tx <= (BTN_PRED_X + BTN_PRED_W) &&
                ty >= BTN_PRED_Y && ty <= (BTN_PRED_Y + BTN_PRED_H)) {
                run_prediction_pipeline();
                delay_ms(350);
                touch_was_down = 0;
                continue;
            }

            if (tx >= BOX_X && tx < (BOX_X + BOX_SIZE) &&
                ty >= BOX_Y && ty < (BOX_Y + BOX_SIZE)) {
                int c = (tx - BOX_X) / SCALE;
                int r = (ty - BOX_Y) / SCALE;

                plot_brush(r, c);

                if (touch_was_down) {
                    int lc = (last_tx - BOX_X) / SCALE;
                    int lr = (last_ty - BOX_Y) / SCALE;
                    int steps = 3;
                    for (int s = 1; s < steps; s++) {
                        int ir = lr + (r - lr) * s / steps;
                        int ic = lc + (c - lc) * s / steps;
                        plot_brush(ir, ic);
                    }
                }

                last_tx = tx;
                last_ty = ty;
                touch_was_down = 1;
            }
        } else {
            touch_was_down = 0;
        }

        delay_ms(5);
    }
}
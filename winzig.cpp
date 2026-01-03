#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "lib/pico-ssd1306/shapeRenderer/ShapeRenderer.h"
#include "lib/pico-ssd1306/ssd1306.h"
#include "lib/pico-ssd1306/textRenderer/TextRenderer.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "textRenderer/5x8_font.h"
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdio.h>

using namespace pico_ssd1306;

#define SCREEN_WIDTH 127
#define SCREEN_HEIGHT 64

#define L_ARROW 14
#define R_ARROW 15
#define BALL_SIZE 1
#define RACKET_HEIGHT 1

#define X_BALL_POS (SCREEN_WIDTH - 32) >> 1
#define Y_BALL_POS ((SCREEN_WIDTH - 32) >> 1) + 32

int score = 0;
int x1 = X_BALL_POS, x2 = Y_BALL_POS;
int ball_x = SCREEN_WIDTH / 2, ball_y = 10;
int ball_speed_x = 1, ball_speed_y = 1;

int main() {
  stdio_init_all();
  sleep_ms(2000);
  i2c_init(i2c0, 1000000);
  gpio_set_function(21, GPIO_FUNC_I2C);
  gpio_set_function(20, GPIO_FUNC_I2C);
  gpio_pull_up(21);
  gpio_pull_up(20);

  gpio_init(R_ARROW);
  gpio_set_dir(R_ARROW, GPIO_IN);
  gpio_pull_up(R_ARROW);

  gpio_init(L_ARROW);
  gpio_set_dir(L_ARROW, GPIO_IN);
  gpio_pull_up(L_ARROW);

  SSD1306 display = SSD1306(i2c0, 0x3C, Size::W128xH64);

  display.setOrientation(0);

  while (1) {
    char score_str[1024];
    sprintf(score_str, "%d", score);

    ball_x += ball_speed_x;
    ball_y += ball_speed_y;

    if (ball_x == 0 || ball_x == SCREEN_WIDTH)
      ball_speed_x = -ball_speed_x;
    
    if (ball_y == 0)
      ball_speed_y = -ball_speed_y;

    if (ball_y == SCREEN_HEIGHT - RACKET_HEIGHT - BALL_SIZE) {
      if (ball_x >= x1 && ball_x <= x2) {
        ball_speed_y = -ball_speed_y;
	score++;
      } else {
 	score--;
        ball_x = SCREEN_WIDTH / 2;
        ball_y = 10;
      }
    }

    if (gpio_get(R_ARROW) == 0 && x2 <= 127)
      x1++, x2++;
    else if (gpio_get(L_ARROW) == 0 && x1 > 0)
      x1--, x2--;

    display.clear();

    drawLine(&display, x1, 63, x2, 63);
    drawLine(&display, 0, 0, 127, 0);
    drawText(&display, font_5x8, score_str, 10, 10);

    display.setPixel(ball_x, ball_y);

    display.sendBuffer();
  }
}

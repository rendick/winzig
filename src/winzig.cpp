#include <pico/stdio_usb.h>
#include <string.h>
extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <hardware/regs/dreq.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "api.h"
#include "ff.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "sd_card.h"
#include "shapeRenderer/ShapeRenderer.h"
#include "ssd1306.h"
#include "textRenderer/5x8_font.h"
#include "textRenderer/TextRenderer.h"

using namespace pico_ssd1306;

#define SCREEN_WIDTH  127
#define SCREEN_HEIGHT 64

#define MOD_BTN 13
#define L_ARROW 14
#define R_ARROW 15

#define BALL_SIZE     1
#define RACKET_HEIGHT 1

#define X_BALL_POS (SCREEN_WIDTH - 32) >> 1
#define Y_BALL_POS ((SCREEN_WIDTH - 32) >> 1) + 32

int score = 0;
int x1 = X_BALL_POS, x2 = Y_BALL_POS;
float ball_x = SCREEN_WIDTH / 2.0, ball_y = 10.0;
float ball_speed_x = 1.0, ball_speed_y = 1.0;

void pong_game(pico_ssd1306::SSD1306 &display);
void menu(pico_ssd1306::SSD1306 &display);

void pong_game(pico_ssd1306::SSD1306 &display)
{
	srand(time(NULL));
	int status = 1;

	while (status) {
		char score_str[1024];
		sprintf(score_str, "%d", score);

		ball_x += ball_speed_x;
		ball_y += ball_speed_y;
		printf("%fx%f\n", ball_x, ball_y);
		if (ball_speed_x > 127.0) {
			ball_x = (float)SCREEN_WIDTH / 2;
			ball_y = 10;
		}

		if ((int)ball_x == 0 || (int)ball_x == SCREEN_WIDTH)
			ball_speed_x = -ball_speed_x;

		if ((int)ball_y == 0)
			ball_speed_y = -ball_speed_y;

		if ((int)ball_y == SCREEN_HEIGHT - RACKET_HEIGHT - BALL_SIZE) {
			if ((int)ball_x >= x1 && (int)ball_x <= x2) {
				float hit_pos =
				    (float)(ball_x - x1) / (x2 - x1);
				float offset = hit_pos * 2.0 - 1.0;

				float max_angle = 75.0 * (M_PI / 180.0);
				float angle = max_angle * offset;

				float speed = sqrt(ball_speed_x * ball_speed_x +
				                   ball_speed_y * ball_speed_y);

				ball_speed_x = speed * sin(angle);
				ball_speed_y = -speed * cos(angle);

				score++;
			} else {
				score--;
				ball_x = (float)SCREEN_WIDTH / 2;
				ball_y = 10;
			}
		}

		if (gpio_get(R_ARROW) == 0 && x2 <= 127) {
			x1++, x2++;
		}
		if (gpio_get(L_ARROW) == 0 && x1 > 0) {
			x1--, x2--;
		}

		display.clear();

		drawLine(&display, x1, 63, x2, 63);
		drawLine(&display, 0, 0, 127, 0);
		drawText(&display, font_5x8, score_str, 10, 10);

		display.setPixel(ball_x, ball_y);

		display.sendBuffer();
		if (gpio_get(MOD_BTN) == 0) {
			int hold_time = 0;

			while (gpio_get(MOD_BTN) == 0) {
				sleep_ms(10);
				hold_time += 10;
				printf("hold time: %d\n", hold_time);

				if (hold_time >= 500)
					break;
			}
			if (hold_time >= 500) {
				status = 0;
			}
		}
	}

	menu(display);

	return;
}

void menu(pico_ssd1306::SSD1306 &display)
{
	score = 0;
	display.clear();
	drawText(&display, font_5x8, "Press right arrow to start", 0,
	         SCREEN_HEIGHT / 2);
	display.sendBuffer();

	while (gpio_get(R_ARROW) != 0) {
		sleep_ms(10);
	}

	pong_game(display);
}

void parse_dir()
{
	DIR dir;
	FILINFO fno;
	FRESULT fr;

	fr = f_opendir(&dir, "0:");
	if (fr != FR_OK) {
		printf("ERROR: Could not open directory (%d)\n", fr);
	}

	do {
		f_readdir(&dir, &fno);
		if (fno.fname[0] != 0) {
			printf("File: %s\n", fno.fname);
		}
	} while (fno.fname[0] != 0);

	f_closedir(&dir);
}

int main(void)
{
	char l_test[1024];
	l_test[0] = '\0';

	FRESULT fr;
	FATFS fs;
	FILINFO fno;
	FIL fil;
	int ret;
	char buf[100];
	char filename[] = "test.txt";

	stdio_init_all();

	sleep_ms(2000);
	while (!stdio_usb_connected()) {
		sleep_ms(100);
	}

	printf("WINZIG\n");

	if (!sd_init_driver()) {
		printf("ERROR: Could not initialize SD card\r\n");
	}

	fr = f_mount(&fs, "0:", 1);
	if (fr != FR_OK) {
		printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
	}

	parse_dir();

	fr = f_open(&fil, filename, FA_READ);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
	}

	// ret = f_printf(&fil, "TEST\r\n");
	// if (ret < 0){
	// 	printf("ERROR: Could not write to SD card\r\n");
	// 	f_close(&fil);
	// }
	//
	// ret = f_printf(&fil, "of writing to an SD card.\r\n");
	// if (ret < 0){
	// 	printf("ERROR: Could not write to SD card\r\n");
	// 	f_close(&fil);
	// }

	fr = f_close(&fil);
	if (fr != FR_OK) {
		printf("ERROR: Could not close file (%d)\r\n", fr);
	}

	fr = f_open(&fil, filename, FA_READ);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
	}

	printf("Reading from file: %s\n", filename);
	printf("---\n");
	while (f_gets(buf, sizeof(buf), &fil)) {
		strcat(l_test, buf);
	}
	printf("\r\n---\r\n");

	printf("LTEST: %s\n", l_test);

	fr = f_close(&fil);
	if (fr != FR_OK) {
		printf("ERROR: Could not close file (%d)\r\n", fr);
	}

	f_unmount("0:");

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

	gpio_init(MOD_BTN);
	gpio_set_dir(MOD_BTN, GPIO_IN);
	gpio_pull_up(MOD_BTN);

	SSD1306 display = SSD1306(i2c0, 0x3C, Size::W128xH64);
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	reg_lua_api(L, &display);
	display.clear();
	luaL_loadbuffer(L, l_test, strlen(l_test), "test");

	lua_pcall(L, 0, 0, 0);
	// luaL_dofile(L, "test.lua");
	display.sendBuffer();

	display.setOrientation(0);

	sleep_ms(2000);

	while (1) {
		menu(display);
	}
}

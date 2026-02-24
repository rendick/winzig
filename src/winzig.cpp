#include <hardware/spi.h>
extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <hardware/regs/dreq.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <stdio.h>

#include "api/api.h"
#include "config.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "sd/sd.h"
#include "shapeRenderer/ShapeRenderer.h"
#include "ssd1306.h"
#include "textRenderer/TextRenderer.h"

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

using namespace pico_ssd1306;

lua_State *L;
char l_test[4096];

char games[128][128];
int file_count = 0;

int mark_x = SCREEN_WIDTH - 12 - 2;
int mark_y = 2;

int text_x = 5;
int text_y = 2;

int range = 3;
int mark = 0;
int skip = 0;

void run_game() {}

void menu(pico_ssd1306::SSD1306 &display)
{
	if (range > ARRAY_LENGTH(games)) {
		range = ARRAY_LENGTH(games);
	}

	display.clear();

	if (gpio_get(R_ARROW) == 0) {
		if (mark < file_count) {
			mark++;
			mark_y += 22;
			sleep_ms(200);
		}

		if (mark % 3 == 0 && mark < file_count) {
			mark_y = 2;
			range += 3;
			skip += 3;
			sleep_ms(200);
		}
		sleep_ms(10);
	}

	if (gpio_get(L_ARROW) == 0) {
		if (mark % 3 == 0 && mark > 0) {
			mark_y = 46;
			range -= 3;
			skip -= 3;
			mark--;
		} else if (mark > 0) {
			mark--;
			mark_y -= 22;
			sleep_ms(200);
		}

		sleep_ms(10);
	}

	if (gpio_get(MOD_BTN) == 0) {
		printf("GPIO_MOD_BTN: %d\n", gpio_get(MOD_BTN));
		printf("Start: %s\n", games[mark]);
		read_sd_file((char *)"test.txt", l_test);
		printf("ltest: %s\n", l_test);

		while (1) {
			display.clear();

		if (luaL_loadbuffer(L, l_test, strlen(l_test), "test") == LUA_OK) {
			lua_pcall(L, 0, 0, 0);
		}

			display.sendBuffer();

			sleep_ms(16);
		}
		sleep_ms(500);
	}

	printf("from %d to %d\n", skip, range);
	printf("range = %d\n", range);

	for (int i = 0; i < range; i++) {
		if (i >= skip && i < range) {
			printf("[%d] [%d] %s\n", i, mark, games[i]);
			drawText(&display, font_12x16, games[i], 5, text_y);
			text_y += 22;
		}
	}

	drawText(&display, font_12x16, "*", mark_x, mark_y);

	drawLine(&display, 0, 20, SCREEN_WIDTH, 20);
	drawLine(&display, 0, 42, SCREEN_WIDTH, 42);
	display.sendBuffer();

	text_y = 2;
}

int main(void)
{

	stdio_init_all();
	sleep_ms(2000);
	while (!stdio_usb_connected()) {
		sleep_ms(100);
	}

	printf("WINZIG\n");

	init_sd_card();
	parse_sd_dir(games, &file_count);
	read_sd_file("test.txt", l_test);
	sleep_ms(1000);

	for (int i = 0; i < file_count; i++) {
		printf("stored: %s\n", games[i]);
	}

	i2c_init(i2c0, 1000000);
	gpio_set_function(21, GPIO_FUNC_I2C);
	gpio_set_function(20, GPIO_FUNC_I2C);
	gpio_pull_up(21);
	gpio_pull_up(20);

	SSD1306 display = SSD1306(i2c0, 0x3C, Size::W128xH64);
	display.clear();
	display.sendBuffer();
	display.setOrientation(0);

	gpio_init(R_ARROW);
	gpio_set_dir(R_ARROW, GPIO_IN);
	gpio_pull_up(R_ARROW);

	gpio_init(L_ARROW);
	gpio_set_dir(L_ARROW, GPIO_IN);
	gpio_pull_up(L_ARROW);

	gpio_init(MOD_BTN);
	gpio_set_dir(MOD_BTN, GPIO_IN);
	gpio_pull_up(MOD_BTN);

	L = luaL_newstate();
	luaL_openlibs(L);

	reg_lua_api(L, &display);
	reg_macros(L);

	while (1) {
		// display.clear();
		// if (luaL_loadbuffer(L, l_test, strlen(l_test), "test") !=
		//     LUA_OK) {
		// 	printf("LUA ERROR: %s\n", lua_tostring(L, -1));
		// }
		//
		// lua_pcall(L, 0, 0, 0);
		//
		// display.sendBuffer();
		//
		// sleep_ms(16);
		menu(display);
	}
}

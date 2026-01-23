#pragma once

extern "C" {
	#include "lua.h"
}

#include "ssd1306.h"

int l_draw_pixel(lua_State *L);
void reg_lua_api(lua_State *L, pico_ssd1306::SSD1306 *display);

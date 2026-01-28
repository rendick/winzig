#pragma once

extern "C" {
#include "lua.h"
}

#include "ssd1306.h"

int l_gpio_get(lua_State *L);
int l_sleep_ms(lua_State *L);
int l_send_buffer(lua_State *L);
int l_clear(lua_State *L);
int l_draw_line(lua_State *L);
int l_draw_pixel(lua_State *L);
void reg_lua_api(lua_State *L, pico_ssd1306::SSD1306 *display);
void reg_macros(lua_State *L);

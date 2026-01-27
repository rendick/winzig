extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

#include <hardware/gpio.h>

#include "api.h"
#include "config.h"
#include "shapeRenderer/ShapeRenderer.h"
#include "ssd1306.h"

using namespace pico_ssd1306;

static SSD1306 *g_display = nullptr;

void set_display(SSD1306 *display) { g_display = display; }

int l_gpio_get(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int value = gpio_get(pin);

	lua_pushinteger(L, value);

	return 1;
}

int l_draw_pixel(lua_State *L)
{
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);

	if (g_display) {
		g_display->setPixel(x, y);
	}

	return 0;
}

int l_draw_line(lua_State *L)
{
	int x0 = luaL_checkinteger(L, 1);
	int y0 = luaL_checkinteger(L, 2);
	int x1 = luaL_checkinteger(L, 3);
	int y1 = luaL_checkinteger(L, 4);

	if (g_display) {
		drawLine(g_display, x0, y0, x1, y1);
	}

	return 0;
}

void reg_lua_api(lua_State *L, SSD1306 *display)
{
	set_display(display);
	lua_register(L, "draw_pixel", l_draw_pixel);
	lua_register(L, "draw_line", l_draw_line);
	lua_register(L, "gpio_get", l_gpio_get);
}

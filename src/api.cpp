extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

#include "api.h"
#include "ssd1306.h"

using namespace pico_ssd1306;

static SSD1306 *g_display = nullptr;

void set_display(SSD1306 *display) { g_display = display; }

int l_draw_pixel(lua_State *L)
{
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);

	if (g_display) {
		g_display->setPixel(x, y);
	}

	return 0;
}

void reg_lua_api(lua_State *L, SSD1306 *display)
{
	set_display(display);
	lua_register(L, "draw_pixel", l_draw_pixel);
}

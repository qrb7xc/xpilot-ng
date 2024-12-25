/*
 * XPilotNG/SDL, an SDL/OpenGL XPilot client.
 *
 * Copyright (C) 2003-2004 Juha Lindström <juhal@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "xpclient_sdl.h"

#include "sdlkeys.h"

typedef struct {
	const char *name;
	SDL_Scancode	key;
} sdlkey_t;

static sdlkey_t sdlkeys[] = {
   { "BackSpace",    SDL_SCANCODE_BACKSPACE },
   { "Tab",          SDL_SCANCODE_TAB },
   { "Return",       SDL_SCANCODE_RETURN },
   { "Pause",        SDL_SCANCODE_PAUSE },
   { "Scroll_Lock",  SDL_SCANCODE_SCROLLLOCK },
   { "Print",        SDL_SCANCODE_PRINTSCREEN },
   { "Escape",       SDL_SCANCODE_ESCAPE },
   { "Delete",       SDL_SCANCODE_DELETE },
   { "Home",         SDL_SCANCODE_HOME },
   { "Left",         SDL_SCANCODE_LEFT },
   { "Up",           SDL_SCANCODE_UP },
   { "Right",        SDL_SCANCODE_RIGHT },
   { "Down",         SDL_SCANCODE_DOWN },
   { "Page_Up",      SDL_SCANCODE_PAGEUP },
   { "Page_Down",    SDL_SCANCODE_PAGEDOWN },
   { "Prior",        SDL_SCANCODE_PAGEUP },
   { "Next",         SDL_SCANCODE_PAGEDOWN },
   { "End",          SDL_SCANCODE_END },
   { "Insert",       SDL_SCANCODE_INSERT },
   { "Num_Lock",     SDL_SCANCODE_NUMLOCKCLEAR },
   { "KP_Enter",     SDL_SCANCODE_KP_ENTER },
   { "KP_Multiply",  SDL_SCANCODE_KP_MULTIPLY },
   { "KP_Add",       SDL_SCANCODE_KP_PLUS },
   { "KP_Subtract",  SDL_SCANCODE_KP_MINUS },
   { "KP_Decimal",   SDL_SCANCODE_KP_PERIOD },
   { "KP_Divide",    SDL_SCANCODE_KP_DIVIDE },
   { "KP_Insert",    SDL_SCANCODE_KP_0 },
   { "KP_Delete",    SDL_SCANCODE_KP_PERIOD },
   { "KP_0",         SDL_SCANCODE_KP_0 },
   { "KP_1",         SDL_SCANCODE_KP_1 },
   { "KP_2",         SDL_SCANCODE_KP_2 },
   { "KP_3",         SDL_SCANCODE_KP_3 },
   { "KP_4",         SDL_SCANCODE_KP_4 },
   { "KP_5",         SDL_SCANCODE_KP_5 },
   { "KP_6",         SDL_SCANCODE_KP_6 },
   { "KP_7",         SDL_SCANCODE_KP_7 },
   { "KP_8",         SDL_SCANCODE_KP_8 },
   { "KP_9",         SDL_SCANCODE_KP_9 },
   { "F1",           SDL_SCANCODE_F1 },
   { "F2",           SDL_SCANCODE_F2 },
   { "F3",           SDL_SCANCODE_F3 },
   { "F4",           SDL_SCANCODE_F4 },
   { "F5",           SDL_SCANCODE_F5 },
   { "F6",           SDL_SCANCODE_F6 },
   { "F7",           SDL_SCANCODE_F7 },
   { "F8",           SDL_SCANCODE_F8 },
   { "F9",           SDL_SCANCODE_F9 },
   { "F10",          SDL_SCANCODE_F10 },
   { "F11",          SDL_SCANCODE_F11 },
   { "F12",          SDL_SCANCODE_F12 },
   { "Shift_L",      SDL_SCANCODE_LSHIFT },
   { "Shift_R",      SDL_SCANCODE_RSHIFT },
   { "Control_L",    SDL_SCANCODE_LCTRL },
   { "Control_R",    SDL_SCANCODE_RCTRL },
   { "Caps_Lock",    SDL_SCANCODE_CAPSLOCK },
   { "space",        SDL_SCANCODE_SPACE },
   { "apostrophe",   SDL_SCANCODE_APOSTROPHE },
   { "quoteright",   SDL_SCANCODE_APOSTROPHE },
   { "comma",        SDL_SCANCODE_COMMA },
//   { "plus",         SDL_SCANCODE_PLUS },
   { "minus",        SDL_SCANCODE_MINUS },
   { "period",       SDL_SCANCODE_PERIOD },
   { "slash",        SDL_SCANCODE_SLASH },
   { "0",            SDL_SCANCODE_0 },
   { "1",            SDL_SCANCODE_1 },
   { "2",            SDL_SCANCODE_2 },
   { "3",            SDL_SCANCODE_3 },
   { "4",            SDL_SCANCODE_4 },
   { "5",            SDL_SCANCODE_5 },
   { "6",            SDL_SCANCODE_6 },
   { "7",            SDL_SCANCODE_7 },
   { "8",            SDL_SCANCODE_8 },
   { "9",            SDL_SCANCODE_9 },
   { "semicolon",    SDL_SCANCODE_SEMICOLON },
   { "equal",        SDL_SCANCODE_EQUALS },
   { "a",            SDL_SCANCODE_A },
   { "b",            SDL_SCANCODE_B },
   { "c",            SDL_SCANCODE_C },
   { "d",            SDL_SCANCODE_D },
   { "e",            SDL_SCANCODE_E },
   { "f",            SDL_SCANCODE_F },
   { "g",            SDL_SCANCODE_G },
   { "h",            SDL_SCANCODE_H },
   { "i",            SDL_SCANCODE_I },
   { "j",            SDL_SCANCODE_J },
   { "k",            SDL_SCANCODE_K },
   { "l",            SDL_SCANCODE_L },
   { "m",            SDL_SCANCODE_M },
   { "n",            SDL_SCANCODE_N },
   { "o",            SDL_SCANCODE_O },
   { "p",            SDL_SCANCODE_P },
   { "q",            SDL_SCANCODE_Q },
   { "r",            SDL_SCANCODE_R },
   { "s",            SDL_SCANCODE_S },
   { "t",            SDL_SCANCODE_T },
   { "u",            SDL_SCANCODE_U },
   { "v",            SDL_SCANCODE_V },
   { "w",            SDL_SCANCODE_W },
   { "x",            SDL_SCANCODE_X },
   { "y",            SDL_SCANCODE_Y },
   { "z",            SDL_SCANCODE_Z },
   { "a",            SDL_SCANCODE_A },
   { "b",            SDL_SCANCODE_B },
   { "c",            SDL_SCANCODE_C },
   { "d",            SDL_SCANCODE_D },
   { "e",            SDL_SCANCODE_E },
   { "f",            SDL_SCANCODE_F },
   { "g",            SDL_SCANCODE_G },
   { "h",            SDL_SCANCODE_H },
   { "i",            SDL_SCANCODE_I },
   { "j",            SDL_SCANCODE_J },
   { "k",            SDL_SCANCODE_K },
   { "l",            SDL_SCANCODE_L },
   { "m",            SDL_SCANCODE_M },
   { "n",            SDL_SCANCODE_N },
   { "o",            SDL_SCANCODE_O },
   { "p",            SDL_SCANCODE_P },
   { "q",            SDL_SCANCODE_Q },
   { "r",            SDL_SCANCODE_R },
   { "s",            SDL_SCANCODE_S },
   { "t",            SDL_SCANCODE_T },
   { "u",            SDL_SCANCODE_U },
   { "v",            SDL_SCANCODE_V },
   { "w",            SDL_SCANCODE_W },
   { "x",            SDL_SCANCODE_X },
   { "y",            SDL_SCANCODE_Y },
   { "z",            SDL_SCANCODE_Z },
   { "bracketleft",  SDL_SCANCODE_LEFTBRACKET },
   { "backslash",    SDL_SCANCODE_BACKSLASH },
   { "bracketright", SDL_SCANCODE_RIGHTBRACKET },
   { "grave",        SDL_SCANCODE_GRAVE },
   { "quoteleft",    SDL_SCANCODE_GRAVE },
//   { "quotedbl",     SDL_SCANCODE_DBLAPOSTROPHE },
   { "section",      SDL_SCANCODE_INTERNATIONAL7 },
   { NULL,           SDL_SCANCODE_UNKNOWN },
};

SDL_Scancode Get_key_by_name(const char* name)
{
    sdlkey_t *k;

    for (k = &sdlkeys[0]; k->name != NULL; k++)
        if (!strcmp(name, k->name)) {
	   return k->key;
	}

    return SDL_SCANCODE_UNKNOWN;
}

char *Get_name_by_key(SDL_Scancode key)
{
    sdlkey_t *k;

    for (k = &sdlkeys[0]; k->name != NULL; k++)
        if (key == k->key)
            return (char *)(k->name);

    return NULL;
}

xp_keysym_t String_to_xp_keysym(/*const*/ char *name)
{
    SDL_Scancode sdlk = Get_key_by_name(name);
    if (sdlk == SDL_SCANCODE_UNKNOWN) return XP_KS_UNKNOWN;
    return (xp_keysym_t)sdlk;
}

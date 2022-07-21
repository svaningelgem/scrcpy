#include "plugin.h"
#include "scrcpy.h"
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <windows.h>

void on_window_message(void *userdata, void *hWnd, unsigned int message, Uint64 wParam, int64_t lParam) {
    (void)(userdata);
    (void)(hWnd);

    if ( message != WM_PLUGIN_BASE ) {
        return;
    }

    PluginActions action = (PluginActions) wParam;

    switch (action) {
    case click: {
        // Click the button
        SDL_MouseButtonEvent sdlevent;
        ZeroMemory(&sdlevent, sizeof(sdlevent));

        sdlevent.type = SDL_MOUSEBUTTONDOWN;
        sdlevent.button = SDL_MOUSE_LEFT;
        sdlevent.state = SDL_PRESSED;
        sdlevent.clicks = 1;
        sdlevent.x = LOWORD(lParam);
        sdlevent.y = HIWORD(lParam);
        SDL_PushEvent(&sdlevent);

        // And release it.
        sdlevent.type = SDL_MOUSEBUTTONUP;
        sdlevent.state = SDL_RELEASED;
        SDL_PushEvent(&sdlevent);
    }
    break;

    case take_screenshot:
        lParam = MAKE_LONG(SDL_SCANCODE_S, KMOD_ALT|KMOD_SHIFT);
        // fall down

    case type_key: {
        // Punch that key
        SDL_KeyboardEvent sdlevent;
        sdlevent.type = SDL_KEYDOWN;
        sdlevent.state = SDL_PRESSED;
        sdlevent.keysym.scancode = LOWORD(lParam);
        sdlevent.keysym.sym = HIWORD(lParam);
        SDL_PushEvent(&sdlevent);

        // Release it
        sdlevent.type = SDL_KEYUP;
        sdlevent.state = SDL_RELEASED;
        SDL_PushEvent(&sdlevent);
    }
    break;
    }
}
#endif


void plugin_init(){
#ifdef _WIN32
    SDL_SetWindowsMessageHook(on_window_message,NULL);
#endif
}

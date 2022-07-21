#include "plugin.h"
#include "scrcpy.h"
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_events.h>

#ifdef _WIN32
#include <windows.h>

void on_window_message(void *userdata, void *hWnd, unsigned int message, Uint64 wParam, int64_t lParam) {
    (void)(userdata);
    (void)(hWnd);

    if ( message != WM_PLUGIN_MESSAGE ) {
        return;
    }

    PluginActions action = (PluginActions) wParam;

    switch (action) {
    case click: {
        // Click the button
        SDL_Event sdlevent;
        ZeroMemory(&sdlevent, sizeof(sdlevent));

        sdlevent.type = SDL_MOUSEBUTTONDOWN;
        sdlevent.button.button = SDL_BUTTON_LEFT;
        sdlevent.button.state = SDL_PRESSED;
        sdlevent.button.clicks = 1;
        sdlevent.button.x = LOWORD(lParam);
        sdlevent.button.y = HIWORD(lParam);
        SDL_PushEvent(&sdlevent);

        // And release it.
        sdlevent.button.type = SDL_MOUSEBUTTONUP;
        sdlevent.button.state = SDL_RELEASED;
        SDL_PushEvent(&sdlevent);
    }
    break;

    case take_screenshot:
        lParam = MAKELONG(SDL_SCANCODE_S, KMOD_LALT|KMOD_LSHIFT);
        /* fall through */

    case type_key: {
        // Punch that key
        SDL_Event sdlevent;
        sdlevent.type = SDL_KEYDOWN;
        sdlevent.key.state = SDL_PRESSED;
        sdlevent.key.keysym.scancode = LOWORD(lParam);
        sdlevent.key.keysym.sym = SDL_GetKeyFromScancode(LOWORD(lParam));
        sdlevent.key.keysym.mod = HIWORD(lParam);
        SDL_PushEvent(&sdlevent);

        // Release it
        sdlevent.key.type = SDL_KEYUP;
        sdlevent.key.state = SDL_RELEASED;
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

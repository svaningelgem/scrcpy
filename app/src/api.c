#include "api.h"
#include "esw/EmbeddableWebServer.h"
#include <time.h>

#ifdef WIN32
#pragma comment(lib, "ws2_32") // link against Winsock on Windows
#endif


struct api_server_info {
    struct sc_screen* screen;
    uint16_t port;
};

static struct Server server = {0};


static THREAD_RETURN_TYPE STDCALL_ON_WIN32 acceptConnectionsThread(void* params) {
    serverInit(&server);

    struct api_server_info* server_info = (api_server_info*)params;
    server.tag = server_info->screen;

    struct sockaddr_in listen_on = {0};
    listen_on.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listen_on.sin_family = AF_INET;
    listen_on.sin_port = htons(server_info->port);
    
    free(server_info);

    acceptConnectionsUntilStopped(&server, (struct sockaddr*) &listen_on, sizeof(listen_on));

    serverDeInit(&server);
    memset(&server, 0, sizeof(server));

    return (THREAD_RETURN_TYPE) 0;
}


struct Response*
api_error(const char* url, const char *method, const char *post_data) {
    struct Response* response = responseAlloc(400, "Bad Request", "text/html; charset=UTF-8", 0);
    heapStringAppendString(
        &response->body,
        "ERROR: Cannot interpret %s request to '%s'\r\nPOST data: '%s'",
        method,
        url,
        post_data
    );
    return response;
}


struct Response*
api_ok() {
    return responseAllocJSON("OK");
}


struct Response*
api_tap(struct sc_screen* screen, int32_t x, int32_t y) {

    struct sc_mouse_click_event evt = {
        .position = {
            .screen_size = screen->frame_size,
            .point = sc_screen_convert_window_to_frame_coords(screen, x, y),
        },
        .action = SC_ACTION_DOWN,
        .button = SC_MOUSE_BUTTON_LEFT,
        .buttons_state = SC_MOUSE_BUTTON_LEFT,
    };

    screen->im->mp->ops->process_mouse_click(screen->im->mp, &evt);

    // And release it
    evt.action = SC_ACTION_UP;
    screen->im->mp->ops->process_mouse_click(screen->im->mp, &evt);

    return api_ok();
}


struct Response*
api_swipe(struct sc_screen* screen, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t speed) {
    return responseAlloc400BadRequestHTML("Not implemented yet")
    return api_ok();
}


struct Response*
api_text(struct sc_screen* screen, char * text) {
    return responseAlloc400BadRequestHTML("Not implemented yet")

    if ( text == NULL ) {
        return api_ok();
    }
    else if ( text[0] == 0 ) {
        free(text);
        return api_ok();
    }

    SDL_Event sdlevent;
    ZeroMemory(&sdlevent, sizeof(sdlevent));
    sdlevent.key.keysym.mod = 0;

    for ( int i = 0, end = strlen(text); i < end; ++i ) {
        const char c = text[i]

        // Press the key
        sdlevent.type = SDL_KEYDOWN;
        sdlevent.key.state = SDL_PRESSED;
        sdlevent.key.keysym.scancode = SDL_GetScancodeFromKey(c);
        sdlevent.key.keysym.sym = c;
        SDL_PushEvent(&sdlevent);

        // Release it
        sdlevent.key.type = SDL_KEYUP;
        sdlevent.key.state = SDL_RELEASED;
        SDL_PushEvent(&sdlevent);
    }

    free(text);

    return api_ok();
}


struct Response*
api_screenshot(struct sc_screen* screen, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    return responseAlloc400BadRequestHTML("Not implemented yet")
    return api_ok();
}


int32_t
_get_number(const struct Request* request, const char* param) {
    char * tmp = strdupDecodePOSTParam(param, request, "");
    int32_t ret_val = atoi(tmp);
    free(tmp);
    return ret_val;
}


struct Response*
createResponseForRequest(const struct Request* request, struct Connection* connection) {
    if ( strcasecmp(request->method, "POST") ) {
        return api_error(request->path, request->method, request->body.contents);
    }

    struct sc_screen* screen = (sc_screen*)connection->server->tag;
    if (!strcmp(request->path, "/tap")) {
        return api_tap(
            screen,
            _get_number(request, "x="),
            _get_number(request, "y=")
        );

    }

    if (!strcmp(request->path, "/swipe")) {
        return api_swipe(
            screen,
            _get_number(request, "x1="),
            _get_number(request, "y1="),
            _get_number(request, "x2="),
            _get_number(request, "y2="),
            _get_number(request, "speed=")
        );
    }

    if (!strcmp(request->path, "/text")) {
        return api_text(screen, strdupDecodePOSTParam("text=", request, ""););
    }

    if (!strcmp(request->path, "/screenshot")) {
        return api_screenshot(
            screen,
            _get_number(request, "x1="),
            _get_number(request, "y1="),
            _get_number(request, "x2="),
            _get_number(request, "y2="),
        );
    }

    return api_error(request->path, request->method, request->body.contents);
}


bool
sc_api_start(struct sc_screen *screen, unsigned short api_port) {
    if ( api_port == 0 ) {
        return false;
    }

    struct api_server_info *server_info = malloc(sizeof(api_server_info));
    server_info->screen = screen;
    server_info->port = api_port;

    LOGD("Starting API service");

    pthread_t threadHandle;
    return pthread_create(&threadHandle, NULL, &acceptConnectionsThread, (void*)serverinfo) == 0;
}


bool
sc_api_stop() {
    if ( !server.initialized ) {
        return false;
    }

    LOGD("Stopping API service");

    serverStop(&server);

    return true;
}

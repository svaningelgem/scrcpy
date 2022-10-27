#include "api.h"
#include "esw/EmbeddableWebServer.h"
#include <time.h>

#ifdef WIN32
#pragma comment(lib, "ws2_32") // link against Winsock on Windows
#endif

static struct Server server = {0};


static THREAD_RETURN_TYPE STDCALL_ON_WIN32 acceptConnectionsThread(void* port_to_use) {
    serverInit(&server);

    struct sockaddr_in listen_on = {0};
    listen_on.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listen_on.sin_family = AF_INET;
    listen_on.sin_port = htons((unsigned short)port_to_use);

    acceptConnectionsUntilStopped(&server, (struct sockaddr*) &listen_on, sizeof(listen_on));

    serverDeInit(&server);
    memset(&server, 0, sizeof(server));

    return (THREAD_RETURN_TYPE) 0;
}


struct Response* createResponseForRequest(const struct Request* request, struct Connection* connection) {
    time_t t;
    time(&t);
    return responseAllocHTMLWithFormat("<html><h1>The time is seconds is %ld</h1></html>", t);
}



int
api_error(httplib_connection *conn, const char* url, const char *method, const char *post_data) {
    httplib_printf(
            conn,
            "HTTP/1.0 404 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "ERROR: Cannot interpret %s request to '%s'\r\n"
            "POST data: '%s'",

            url,
            method,
            post_data
    );

    return 1;
}


int
api_ok(httplib_connection *conn) {
    httplib_printf(
        conn,
        "HTTP/1.0 200 OK\r\n"
        "\r\n"
    );

    return 1;
}

int
api_tap(httplib_connection *conn, unsigned long x, unsigned long y) {
    SDL_Event sdlevent;
    ZeroMemory(&sdlevent, sizeof(sdlevent));

    sdlevent.type = SDL_MOUSEBUTTONDOWN;
    sdlevent.button.button = SDL_BUTTON_LEFT;
    sdlevent.button.state = SDL_PRESSED;
    sdlevent.button.clicks = 1;
    sdlevent.button.x = LOWORD(x);
    sdlevent.button.y = HIWORD(y);
    SDL_PushEvent(&sdlevent);

    // And release it.
    sdlevent.button.type = SDL_MOUSEBUTTONUP;
    sdlevent.button.state = SDL_RELEASED;
    SDL_PushEvent(&sdlevent);

    return api_ok(conn);
}


int
api_swipe(httplib_connection *conn, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, unsigned long speed) {
    return 1;
}


int
api_text(httplib_connection *conn, const char * text) {
    SDL_Event sdlevent;
    ZeroMemory(&sdlevent, sizeof(sdlevent));

    for (int i = 0; i < strlen(text); ++i ) {
        const char c = text[i]

        sdlevent.type = SDL_KEYDOWN;
        sdlevent.key.state = SDL_PRESSED;
        sdlevent.key.keysym.scancode = SDL_GetScancodeFromKey(c);
        sdlevent.key.keysym.sym = c;
        sdlevent.key.keysym.mod = 0;
        SDL_PushEvent(&sdlevent);

        // Release it
        sdlevent.key.type = SDL_KEYUP;
        sdlevent.key.state = SDL_RELEASED;
        SDL_PushEvent(&sdlevent);
    }

    return api_ok(conn);
}


int
api_screenshot(httplib_connection *conn, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2) {
    return 1;
}


static int
begin_request_handler(httplib_connection *conn) {
    const struct httplib_request_info *ri = httplib_get_request_info(conn);
    if ( ri->request_method != "POST" ) {
        return api_error(conn, ri->uri, ri->request_method, "");
    }

    char post_data[1024];
    int post_data_len = httplib_read(conn, post_data, sizeof(post_data));

    if (!strcmp(ri->uri, "/tap")) {
        char x[10], y[10];

        httplib_get_var(post_data, post_data_len, "x", x, sizeof(x));
        httplib_get_var(post_data, post_data_len, "y", y, sizeof(y));

        return api_tap(conn, atol(x), atol(y));
    }

    if (!strcmp(ri->uri, "/swipe")) {
        char x1[10], y1[10], x2[10], y2[10], speed[20];

        httplib_get_var(post_data, post_data_len, "x1", x1, sizeof(x1));
        httplib_get_var(post_data, post_data_len, "y1", y1, sizeof(y1));
        httplib_get_var(post_data, post_data_len, "x2", x2, sizeof(x2));
        httplib_get_var(post_data, post_data_len, "y2", y2, sizeof(y2));
        httplib_get_var(post_data, post_data_len, "speed", speed, sizeof(speed));

        return api_swipe(conn, atol(x1), atol(y1), atol(x2), atol(y2), atol(speed));
    }

    if (!strcmp(ri->uri, "/text")) {
        return api_text(conn, post_data);
    }

    if (!strcmp(ri->uri, "/screenshot")) {
        char x1[10], y1[10], x2[10], y2[10];

        httplib_get_var(post_data, post_data_len, "x1", x1, sizeof(x1));
        httplib_get_var(post_data, post_data_len, "y1", y1, sizeof(y1));
        httplib_get_var(post_data, post_data_len, "x2", x2, sizeof(x2));
        httplib_get_var(post_data, post_data_len, "y2", y2, sizeof(y2));

        return api_screenshot(conn, atol(x1), atol(y1), atol(x2), atol(y2));
    }

    return api_error(conn, ri->url, ri->request_method, post_data);
}


bool
sc_api_start(unsigned short api_port) {
    if ( api_port == 0 || server.initialized ) {
        return false;
    }

    LOGD("Starting API service");

    pthread_t threadHandle;
    pthread_create(&threadHandle, NULL, &acceptConnectionsThread, (void*)api_port);

    return true;
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

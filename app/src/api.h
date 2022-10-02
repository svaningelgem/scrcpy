#ifndef SCRCPY_API_H
#define SCRCPY_API_H


#include "common.h"
#include "screen.h"


bool sc_api_start(struct sc_screen *screen, unsigned short api_port);

void sc_api_stop();


#endif // SCRCPY_API_H

#ifndef __PLUGIN_H__
#define __PLUGIN_H__


#define WM_PLUGIN_MESSAGE WM_USER

typedef enum PluginActions {
    click = 1,
    type_key = 2,
    take_screenshot = 3,
} PluginActions;

void plugin_init();

#endif
#pragma once
//core.h
#include <array>
#include "state.h"


namespace core {
    extern state::WindowConfig& windowConfig;
    enum API {
        OPENGL,
        VULKAN,
        DIRECTX,
        AUTO,
        NONE
    };
    int init(API api);
	int createWindow();
    int get_vesion_core();
};
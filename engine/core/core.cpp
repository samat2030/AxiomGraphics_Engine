//core.cpp

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <array>
#include <cstdlib>
#include "platform/opengl.h"
#include "state.h"

//0 - ок
//1 - уже инициализировано
//2 - еще не инициализировано
//100 - не один из поддерживаемых API не удалось инициализировать
//101 - OpenGL не поддерживаетс€ на этом устройстве
//109 - ¬ыбран неподдерживаемый API

namespace core {
	state::WindowConfig& windowConfig = state::windowConfig;
	enum API {
		OPENGL,
		VULKAN,
		DIRECTX,
		AUTO,
		NONE
	};
	int init(API api) {
		if (q::selectedAPI != NONE) {
			return 1;
		}
		return q::init(api);
	}
	int createWindow() {
		return q::api.createWindow();
	}
	int get_vesion_core() {
		// ‘ормат числа: ddmmyyyyvv
		// dd   - день мес€ца (без ведущего нул€, если <10, например 8)
		// mm   - мес€ц (например, 02)
		// yyyy - год (например, 2026)
		// vv   - верси€ (по умолчанию 00)
		// ѕример: 802202600 -> 8 феврал€ 2026, верси€ 00
		// ѕример: 1106202558 -> 11 июн€ 2025, верси€ 58
		return 102202600;
	}
};

namespace q {
	/*struct API {
		int init();
		bool __is_available__();
		API(core::API selectedAPI) {
			if (selectedAPI==core::OPENGL) {
				this->windowConfig = &gl::windowConfig;
			}
		}
	};*/
	struct API_constucrt {
		int createWindow() {
			if (selectedAPI == core::OPENGL) {
				return gl::createWindow();
			}
			else {
				std::cerr << "ERROR:2:NOT_INITIALIZED)\n";
				exit(2);
			}
		}
		int init() {
			if (selectedAPI == core::OPENGL) {
				return gl::init();
			}
			else {
				std::cerr << "ERROR:2:NOT_INITIALIZED)\n";
				exit(2);
			}
			return 0;
		}
	};
	API_constucrt api;
	core::API selectedAPI = core::NONE;
	static int init(core::API _api) {
		if (selectedAPI != core::NONE) {
			return 1;
		}
		selectedAPI = _api;
		switch (selectedAPI) {
		case core::OPENGL:
			break;
		case core::AUTO:
			if (gl::__is_available__()) {
				selectedAPI = core::OPENGL;
			}
			else {
				return 100;
			}
			break;
		default:
			return 109;
			break;
		}
		int result = api.init();
		return result;
	}
};

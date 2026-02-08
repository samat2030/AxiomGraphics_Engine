// opengl.cpp
#include "opengl.h"
#include "stb_image.h"
#include "../state.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include <thread>
#include <chrono> 

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#include <GLFW/glfw3native.h>

typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

inline bool checkWindows10or11(GLFWwindow* window) {
	if (!window)
		return false;

	// проверка, что Win32 handle существует
	HWND hwnd = glfwGetWin32Window(window);
	if (!hwnd)
		return false;

	// проверка версии Windows >= 10
	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
	if (!hMod)
		return false;

	auto rtlGetVersion =
		(RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
	if (!rtlGetVersion)
		return false;

	RTL_OSVERSIONINFOW os = {};
	os.dwOSVersionInfoSize = sizeof(os);

	if (rtlGetVersion(&os) != 0)
		return false;

	return os.dwMajorVersion >= 10;
}

#else

  // не Windows Ч всегда false
inline bool checkWindows10or11(void*) {
	return false;
}

#endif

//0 - ок
//1 - уже инициализировано
//2 - еще не инициализировано
//100 - не один из поддерживаемых API не удалось инициализировать
//101 - OpenGL не поддерживаетс€ на этом устройстве
//110 - 

namespace p { // hiden namespace
	enum Status {
		NO_INIT,
		INITIALIZED
	};
	enum Status status = NO_INIT;
	inline bool isOpenGLAvailable() {
		if (status == INITIALIZED) {
			return true;
		}
		if (!glfwInit())
			return false;
		// пробуем создать контекст OpenGL 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow* window = glfwCreateWindow(
			1, 1, "", nullptr, nullptr
		);
		if (!window) {
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader(
			(GLADloadproc)glfwGetProcAddress)) {
			glfwDestroyWindow(window);
			glfwTerminate();
			return false;
		}
		glfwDestroyWindow(window);
		glfwTerminate();
		return true;
	}

	GLFWwindow* window = nullptr;
	bool windows10or11 = false;
	double TimeFrame = glfwGetTime();
	float targetFrameDuration = 1.0f / float(state::windowConfig.fps);
	double frameStart = glfwGetTime();


	struct CopyWindowConfig {
		int width;
		int height;
		int fps;
		short selectedMonitor_of_fullscreen;
		const char* title;
		bool fullscreen;
		bool resizable;
		bool clickThrough;
		bool vsync;
	};
	CopyWindowConfig copyWindowConfig;

	// рефлизаци€ под windowConfig
	void setPositionP(int x, int y) {
		if (status != INITIALIZED || !window || state::windowConfig.fullscreen == true) return;
		glfwSetWindowPos(window, x, y);
	}
	void getPositionP(int &x, int &y) {
		if (status != INITIALIZED || !window || state::windowConfig.fullscreen == true) return;
		glfwGetWindowPos(window, &x, &y);
	}

	void minimizeP() {
		if (status != INITIALIZED || !window) return;
		glfwIconifyWindow(window);
	}
	void maximizeP() {
		if (status != INITIALIZED || !window) return;
		glfwRestoreWindow(window);
	}
	void hideP() {
		if (status != INITIALIZED || !window) return;
		glfwHideWindow(window);
	}
	void showP() {
		if (status != INITIALIZED || !window) return;
		glfwShowWindow(window);
	}

	void setIconP(const char* path) {
#ifndef __APPLE__
		GLFWimage images[1];
		int channels;
		images[0].pixels = stbi_load(path, &images[0].width, &images[0].height, &channels, 4);

		if (images[0].pixels) {
			glfwSetWindowIcon(window, 1, images);
			stbi_image_free(images[0].pixels);
		}
#endif
	}

	int getMonitorCountP() {
		int count;
		glfwGetMonitors(&count);
		return count;
	}
	const char* getMonitorNameP(int index) {
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		if (index >= 0 && index < count) {
			return glfwGetMonitorName(monitors[index]);
		}
		return "Unknown";
	}

	//функции пользовател€
	void(*closeFunc)() = nullptr;



	//системные функции
	struct WindowData {
		void (*userCallback)(int, int) = nullptr;
	};
};


namespace gl {
	int init() {
		if (p::status == p::INITIALIZED) {
			return 1;
		}
		if (!p::isOpenGLAvailable()) {
			return 101;
		}
		state::windowConfig.changeFuncs(
			&p::setPositionP,
			&p::getPositionP,
			&p::minimizeP,
			&p::maximizeP,
			&p::hideP,
			&p::showP,
			&p::setIconP,
			&p::getMonitorCountP,
			&p::getMonitorNameP
		);
		p::copyWindowConfig.width = state::windowConfig.width;
		p::copyWindowConfig.height = state::windowConfig.height;
		p::copyWindowConfig.fps = state::windowConfig.fps;
		p::copyWindowConfig.selectedMonitor_of_fullscreen = state::windowConfig.selectedMonitor_of_fullscreen;
		p::copyWindowConfig.title = state::windowConfig.title;
		p::copyWindowConfig.fullscreen = state::windowConfig.fullscreen;
		p::copyWindowConfig.resizable = state::windowConfig.resizable;
		p::copyWindowConfig.clickThrough = state::windowConfig.clickThrough;
		p::copyWindowConfig.vsync = state::windowConfig.vsync;
		return 0;
	}
	
	
	// методы
	// управление окном
	int createWindow() {
		//прочие проврерки
		if (p::status == p::INITIALIZED) {
			return 1;
		}
		
		//инициализаци€ OpenGL
		if (!glfwInit())
			return 101;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, state::windowConfig.resizable);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, state::windowConfig.transparent);

		GLFWmonitor* monitor = nullptr;
		if (state::windowConfig.fullscreen) {
			int count;
			GLFWmonitor** monitors = glfwGetMonitors(&count);
			if (state::windowConfig.selectedMonitor_of_fullscreen < count)
				monitor = monitors[state::windowConfig.selectedMonitor_of_fullscreen];
			else
				monitor = glfwGetPrimaryMonitor();
		}

		p::window = glfwCreateWindow(
			state::windowConfig.width,
			state::windowConfig.height,
			state::windowConfig.title,
			monitor,
			nullptr
		);
		if (!p::window) {
			glfwTerminate();
			return 101;
		}
		
		glfwMakeContextCurrent(p::window);
		if (!gladLoadGLLoader(
			(GLADloadproc)glfwGetProcAddress)) {
			glfwDestroyWindow(p::window);
			glfwTerminate();
			return 101;
		}
		p::windows10or11 = checkWindows10or11(p::window);
		// ---------- vsync ----------
		glfwSwapInterval(state::windowConfig.vsync ? 1 : 0);

		if (state::windowConfig.clickThrough && p::windows10or11) {
#ifdef _WIN32
			HWND hwnd = glfwGetWin32Window(p::window);
			if (hwnd) {
				LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
				SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
			}
#endif
		}
		//успешно проинициализировано
		p::status = p::INITIALIZED;
		//все - ок
		return 0;
	}
	int quit() {
		if (p::status != p::INITIALIZED || !p::window) return 2;
		glfwDestroyWindow(p::window);
		glfwTerminate();
		p::status = p::NO_INIT;
		return 0;
	}

	// rule/set методы
	void allowResize(bool allow) {
		if (p::status != p::INITIALIZED || !p::window) return;
		glfwSetWindowAttrib(p::window, GLFW_RESIZABLE, allow ? GLFW_TRUE : GLFW_FALSE);
	}
	void setSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight) {
		if (p::status != p::INITIALIZED || !p::window) return;
		glfwSetWindowSizeLimits(p::window, minWidth, minHeight, maxWidth, maxHeight);
	}
	void setAspectRatio(int numerator, int denominator) {
		if (p::status != p::INITIALIZED || !p::window) return;
		glfwSetWindowAspectRatio(p::window, numerator, denominator);
	}


	// event методы
	void setCloseFunc(void (*func)()) {
		if (p::status != p::INITIALIZED || !p::window) return;
		p::closeFunc = func;
	}
	


	void setResizeFunc(void (*func)(int, int)) {
		if (p::status != p::INITIALIZED || !p::window) return;

		// —охран€ем указатель на функцию в "карман" окна
		static p::WindowData data;
		data.userCallback = func;
		glfwSetWindowUserPointer(p::window, &data);

		glfwSetFramebufferSizeCallback(p::window, [](GLFWwindow* window, int width, int height) {
			auto* d = (p::WindowData*)glfwGetWindowUserPointer(window);

			glViewport(0, 0, width, height);

			if (d && d->userCallback) {
				d->userCallback(width, height);
			}
		});
	}



	// системные методы
	void update() {
		if (p::status != p::INITIALIZED || !p::window) return;
		glfwPollEvents();

		if (glfwWindowShouldClose(p::window)) {
			if (p::closeFunc) {
				p::closeFunc();
			}
			else {
				glfwDestroyWindow(p::window);
				glfwTerminate();
				p::status = p::NO_INIT;
			}
		}


		//проверка windowConfig
		if (state::windowConfig.width != p::copyWindowConfig.width ||
			state::windowConfig.height != p::copyWindowConfig.height) {
			glfwSetWindowSize(p::window, state::windowConfig.width, state::windowConfig.height);
			p::copyWindowConfig.width = state::windowConfig.width;
			p::copyWindowConfig.height = state::windowConfig.height;
		}
		
		if (state::windowConfig.title != p::copyWindowConfig.title) {
			glfwSetWindowTitle(p::window, state::windowConfig.title);
			p::copyWindowConfig.title = state::windowConfig.title;
		}

		if (state::windowConfig.fullscreen != p::copyWindowConfig.fullscreen ||
			state::windowConfig.selectedMonitor_of_fullscreen != p::copyWindowConfig.selectedMonitor_of_fullscreen) {
			GLFWmonitor* monitor = nullptr;
			if (state::windowConfig.fullscreen) {
				int count;
				GLFWmonitor** monitors = glfwGetMonitors(&count);
				if (state::windowConfig.selectedMonitor_of_fullscreen < count)
					monitor = monitors[state::windowConfig.selectedMonitor_of_fullscreen];
				else
					monitor = glfwGetPrimaryMonitor();
			}
			glfwSetWindowMonitor(p::window, monitor, 0, 0, state::windowConfig.width, state::windowConfig.height, GLFW_DONT_CARE);
			p::copyWindowConfig.fullscreen = state::windowConfig.fullscreen;
			p::copyWindowConfig.selectedMonitor_of_fullscreen = state::windowConfig.selectedMonitor_of_fullscreen;
		}

		if (state::windowConfig.vsync != p::copyWindowConfig.vsync) {
			glfwSwapInterval(state::windowConfig.vsync ? 1 : 0);
			p::copyWindowConfig.vsync = state::windowConfig.vsync;
		}

		if (state::windowConfig.clickThrough != p::copyWindowConfig.clickThrough && p::windows10or11) {
			glfwSetWindowAttrib(p::window, GLFW_MOUSE_PASSTHROUGH, state::windowConfig.clickThrough ? GLFW_TRUE : GLFW_FALSE);
		}
		if (state::windowConfig.fps != p::copyWindowConfig.fps) {
			p::targetFrameDuration = 1.0f / float(state::windowConfig.fps);
			p::copyWindowConfig.fps = state::windowConfig.fps;
		}


		double frameElapsed = glfwGetTime() - p::frameStart;

		if (frameElapsed < p::targetFrameDuration) {
			std::this_thread::sleep_for(std::chrono::duration<double>(p::targetFrameDuration - frameElapsed));
		}


		p::frameStart = glfwGetTime();

		glfwSwapBuffers(p::window);
	}







	
	bool __is_available__() {
		return p::isOpenGLAvailable();
	}
	int version(bool GL) {
		if (GL) {
			const char* versionStr = (const char*)glGetString(GL_VERSION);
			if (versionStr) {
				int major = 0, minor = 0;
				sscanf(versionStr, "%d.%d", &major, &minor);
				return major * 100 + minor * 10; // например, 3.3 -> 330
			}
			return 0; // не удалось получить версию
		}
		else {
			// ‘ормат числа: ddmmyyyyvv
			// dd   - день мес€ца (без ведущего нул€, если <10, например 8)
			// mm   - мес€ц (например, 02)
			// yyyy - год (например, 2026)
			// vv   - верси€ (по умолчанию 00)
			// ѕример: 802202600 -> 8 феврал€ 2026, верси€ 00
			// ѕример: 1106202558 -> 11 июн€ 2025, верси€ 58
			return 802202600;
		}
	}
};
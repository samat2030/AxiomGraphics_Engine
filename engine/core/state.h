#pragma once
//state.h
#include <iostream>
#include <cstdlib>

namespace state {
	struct WindowConfig {
	private:
		int posX = 100;
		int posY = 100;
		bool changed_pos = false;

		[[noreturn]] inline void non_init() {
			std::cerr << "ERROR:2:NOT_INITIALIZED\n";
			exit(2);
		}

		void (*setPositionP)(int, int) = nullptr;
		void (*getPositionP)(int&, int&) = nullptr;
		void (*minimizeP)() = nullptr;
		void (*maximizeP)() = nullptr;
		void (*hideP)() = nullptr;
		void (*showP)() = nullptr;
		void (*setIconP)(const char*) = nullptr;
		int (*getMonitorCountP)() = nullptr;
		const char* (*getMonitorNameP)(int) = nullptr;
	public:
		int width = 800;
		int height = 600;
		short selectedMonitor_of_fullscreen = 0;
		int fps = 60;

		const char* title = "My Engine";

		bool fullscreen = false;
		bool resizable = true;
		bool transparent = false; //not changeable after init
		bool clickThrough = false;//only for Windows
		bool vsync = true;

		void setPosition(int x, int y) {
			if (!setPositionP) non_init();
			setPositionP(x, y);
		}
		void getPosition(int& x, int& y) {
			if (!getPositionP) non_init();
			getPositionP(x, y);
		}

		void minimize() {
			if (!minimizeP) non_init();
			minimizeP();
		}
		void maximize() {
			if (!maximizeP) non_init();
			maximizeP();
		}
		void hide() {
			if (!hideP) non_init();
			hideP();
		}
		void show() {
			if (!showP) non_init();
			showP();
		}

		void setIcon(const char* path) {
			if (!setIconP) non_init();
			setIconP(path);
		}

		int getMonitorCount() {
			if (!getMonitorCountP) non_init();
			return getMonitorCountP();
		}
		const char* getMonitorName(int index) {
			if (!getMonitorNameP) non_init();
			return getMonitorNameP(index);
		}

		void changeFuncs(
			void (*setPositionPP)(int, int),
			void (*getPositionPP)(int&, int&),
			void (*minimizePP)(),
			void (*maximizePP)(),
			void (*hidePP)(),
			void (*showPP)(),
			void (*setIconPP)(const char*),
			int (*getMonitorCountPP)(),
			const char* (*getMonitorNamePP)(int)
		) noexcept {
			this->setPositionP = setPositionPP;
			this->getPositionP = getPositionPP;
			this->minimizeP = minimizePP;
			this->maximizeP = maximizePP;
			this->hideP = hidePP;
			this->showP = showPP;
			this->setIconP = setIconPP;
			this->getMonitorCountP = getMonitorCountPP;
			this->getMonitorNameP = getMonitorNamePP;
		}
	};
	extern WindowConfig windowConfig;
};
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
//101 - OpenGL не поддерживается на этом устройстве
//109 - Выбран неподдерживаемый API

namespace q {
	/*struct API {
		struct WindowConfig {
	   private:
		   int posX;
		   int posY;
		   bool changed_pos;
	   public:
		   int width;
		   int height;
		   short selectedMonitor_of_fullscreen;
		   int fps;

		   const char* title;

		   bool fullscreen;
		   bool resizable;    //not changeable after init
		   bool transparent;  //not changeable after init
		   bool clickThrough; //only for Windows
		   bool vsync;

		   void setPosition(int x, int y);
		   void getPosition(int& x, int& y);
		   void getPosition(int& x, int& y, bool NON_TOUCH);

		   void minimize();
		   void maximize();
		   void hide();
		   void show();

		   void setIcon(const char* path);

		   int getMonitorCount();
		   const char* getMonitorName(int index);
	   };
		WindowConfig* windowConfig = nullptr;
		int init();
		bool __is_available__();
		API(core::API selectedAPI) {
			if (selectedAPI==core::OPENGL) {
				this->windowConfig = &gl::windowConfig;
			}
		}
	};//добавь суда функции который в зависомости от selectedAPI будут вести к нужному api::*/
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
		API_constucrt() {
			if (selectedAPI == core::OPENGL) {
				gl:init();
			}
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
				selectedAPI == core::OPENGL;
			}
			else {
				return 100;
			}
			break;
		default:
			return 109;
			break;
		}
		int result = 0;//api::init();
		return result;
	}
};
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
	int createWindow();
	int get_vesion_core() {
		// xx - day(if started == 0 {deleted 0})
		// xx - month
		// xxxx - year
		// xx - day version(default 00)
		return 102202600;
	}
};



/*
float pxToGLX(int x, int screenWidth) {
	return (2.0f * x) / screenWidth - 1.0f; // -1..1
}

float pxToGLY(int y, int screenHeight) {
	return 1.0f - (2.0f * y) / screenHeight; // -1..1 и переворачиваем Y
}

class Triangle {
private:
	unsigned int VAO, VBO;
	vector<float> vertices;

public:
	Triangle(const vector<float>& verts) {
		vertices = verts;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		// позиции
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// цвета
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void draw() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
	}

	~Triangle() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
};


int main() {
	const int width = 800;
	const int height = 600;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "3 Triangles", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

	// Шейдеры
	const char* vertexShaderSource = R"(
	#version 330 core
	layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec3 aColor;
	out vec3 vertexColor;
	void main() {
		gl_Position = vec4(aPos, 1.0);
		vertexColor = aColor;
	})";

	const char* fragmentShaderSource = R"(
	#version 330 core
	in vec3 vertexColor;
	out vec4 FragColor;
	void main() {
		FragColor = vec4(vertexColor, 1.0);
	})";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 3 треугольника с координатами в пикселях
	vector<float> tri1 = {
		pxToGLX(400, width), pxToGLY(100, height), 0.0f, 1.0f, 0.0f, 0.0f,
		pxToGLX(300, width), pxToGLY(300, height), 0.0f, 0.0f, 1.0f, 0.0f,
		pxToGLX(500, width), pxToGLY(300, height), 0.0f, 0.0f, 0.0f, 1.0f
	};

	vector<float> tri2 = {
		pxToGLX(200, width), pxToGLY(400, height), 0.0f, 1.0f, 1.0f, 0.0f,
		pxToGLX(100, width), pxToGLY(500, height), 0.0f, 0.0f, 1.0f, 1.0f,
		pxToGLX(300, width), pxToGLY(500, height), 0.0f, 1.0f, 0.0f, 1.0f
	};

	vector<float> tri3 = {
		pxToGLX(600, width), pxToGLY(400, height), 0.0f, 1.0f, 0.5f, 0.2f,
		pxToGLX(550, width), pxToGLY(500, height), 0.0f, 0.2f, 0.5f, 1.0f,
		pxToGLX(650, width), pxToGLY(500, height), 0.0f, 0.7f, 1.0f, 0.3f
	};

	Triangle t1(tri1);
	Triangle t2(tri2);
	Triangle t3(tri3);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		t1.draw();
		t2.draw();
		t3.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
*/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GLFW/glfw3.h>
#include <vector>

#include "Entity.hpp"
#include "Camera.hpp"

enum shader_mode{
	LIGHT_TEXTURE,
	WIREFRAME_DEBUG,
	NORM_DEBUG,
	DIFFUSE_DEBUG
};

enum lighting_mode{
	BLUE_LIGHT,
	HEAD_LIGHT,
	YELLOW_LIGHT,
	DARKNESS
};

class Graphics{
public:
	Graphics(std::vector<Entity> *entities = NULL, Camera *camera = NULL, int xWindowSize = 1000, int yWindowSize = 700);
	void setData(std::vector<Entity> *entities, Camera *camera);
	void initWindow();
	GLFWwindow *getWindow();

	// Rendering
	void renderFrame(float t = 0.0f);

	// Mode changes
	void setShaderMode(int mode);
	void setLightingMode(int mode);
private:
	// External access
	std::vector<Entity> *entities;
	Camera *camera;

	// Shader programs
	std::vector<unsigned int> shaderPIDs;

	// Window properties
	GLFWwindow *window;
	int windowSizeX, windowSizeY;

	// Modes
	shader_mode shaderMode;
	lighting_mode lightingMode;

	// Setup methods
	void initialiseShaders();
	unsigned int compileShader(std::string name);
	void setLighting(unsigned int PID, float t = 0.0f);
};

// 	std::vector<unsigned int> shaderProgramIDs;
// 	Camera *camera;

// 	GLFWwindow *window;
// 	unsigned int shaderProgram;
// 	volatile bool *shouldStop;
// 	int windowSizeX, windowSizeY;

// 	void resizeWindow();
// 	bool initWindow();
// 	void display();

// 	// Threaded operation (NYI)
// 	void startGraphicsThread();

// 	// Callbacks
// 	static void error_callback(int error, const char *description);
// 	void window_resize_callback(int x, int y);
// public:
// 	Graphics(){}
// 	Graphics(std::vector<Object> *objects, Camera *camera);

// 	// Synchronous operation
// 	GLFWwindow * initialiseGraphics(int windowSizeX = 500, int windowSizeY = 500);
// 	void renderFrame(glm::vec2 goal);
// 	void exitWindow();

// 	// Threaded operation (NYI)
// 	GLFWwindow * initialiseGraphicsThread(int windowSizeX = 500, int windowSizeY = 500);
// };

#endif
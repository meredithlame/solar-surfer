#include "ModelLoader.hpp"

#include <algorithm>
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	if( argc < 2){
		std::cout << "Usage: assign2 [-c] pathToObj" << std::endl;
	}
	ModelLoader ml;
	if( argc==3 ){
		if( !strcmp(argv[1],"-c") ){
			ml.setCharacter(true);
		}else if( !strcmp(argv[2], "-c") ){
			ml.setCharacter(true);
			argc--;
		}else{
			std::cout << "Usage: assign2 [-c] pathToObj" << std::endl;
		}
	}
	std::vector<std::string> paths;
	// for( int i=1; i<argc; i++ ){
	// 	std::string path = argv[i];
	// 	paths.push_back(path);
	// }
	paths.push_back(argv[argc-1]);

	ml.initialise(paths);
	// Print usage guide
	std::cout << "Controls:" << std::endl;
	std::cout << "C: Toggle character mode" << std::endl;
	std::cout << "Normal mode controls:" << std::endl;
	std::cout << "L: Cycle lighting" << std::endl;
	std::cout << "D: Cycle debug" << std::endl;
	std::cout << "S: Switch between lighting and debug" << std::endl;
	std::cout << "Character mode controls:" << std::endl;
	std::cout << "W, A, S, D: Move forward, left, backward, and right" << std::endl;
	std::cout << "SPACE, X: Move up and down" << std::endl;
	std::cout << "L: Cycle lighting" << std::endl;
	std::cout << "M: Cycle debug" << std::endl;
	std::cout << "N: Switch between lighting and debug" << std::endl;
	ml.start();
}

Graphics ModelLoader::graphics;
Camera ModelLoader::camera;
bool ModelLoader::leftMouseDown = false;
bool ModelLoader::rightMouseDown = false;
bool ModelLoader::debug = false;
bool ModelLoader::character = false;
double ModelLoader::xprev, ModelLoader::yprev;
int ModelLoader::windowX, ModelLoader::windowY;
int ModelLoader::nextShaderMode = 1;
int ModelLoader::nextLightingMode = 1;

ModelLoader::ModelLoader(){
	camera = Camera();
	graphics.setData(&entities, &camera);
	windowX = 1000;
	windowY = 700;
}

void ModelLoader::setCharacter(bool c){
	character = c;
}

void ModelLoader::loadModel(std::string path){
	float max = 1.2f;//camera.maxX();
	models.push_back(Model(path));
	entities.push_back(Entity(&models.back()));
	float extremum = models.back().getExtremum();
	std::cout <<"Max view " << max << " Extremum " << extremum << std::endl;
	// Scale s.t. extremum == max
	if( extremum!=0 ){
		entities.back().resize(max/extremum);
	}
}

void ModelLoader::initCamera(){
	camera.reposition(glm::vec3(0.0f, 2.0f, 6.0f));
	camera.fixedLookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void ModelLoader::initialise(std::vector<std::string> paths){
	graphics.initWindow();
	window = graphics.getWindow();
	initCamera();
	for( int i=0; i<paths.size(); i++ ){
		loadModel(paths[i]);
	}
	// Character
	// models.push_back(Model("craft/cube-simple.obj"));
	// entities.push_back(Entity(&models.back()));
	// entities.back().resize(0.1f);
}

void ModelLoader::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
	if( action == GLFW_PRESS || action == GLFW_REPEAT ){
		glm::vec2 keyDirection;
		if( character ){
			glm::vec3 direction(0.0f);
			switch( key ){
				case GLFW_KEY_A:
					direction = glm::cross(camera.getUpDirection(), camera.getMoveDirection());
				break;
				case GLFW_KEY_D:
					direction = glm::cross(camera.getMoveDirection(), camera.getUpDirection());
				break;
				case GLFW_KEY_W:
					direction = camera.getMoveDirection();
				break;
				case GLFW_KEY_S:
					direction = -camera.getMoveDirection();
				break;
				case GLFW_KEY_SPACE:
					direction = camera.getUpDirection();
				break;
				case GLFW_KEY_X:
					direction = -camera.getUpDirection();
				break;
				case GLFW_KEY_L:
					if( !debug ){
						graphics.setLightingMode(nextLightingMode++);
						nextLightingMode %= 4;
					}else{
						debug = true;
						nextShaderMode = 0;
						graphics.setShaderMode(nextShaderMode++);
					}
				break;
				case GLFW_KEY_M:
					if( debug ){
						graphics.setShaderMode(0);
						graphics.setShaderMode(nextShaderMode++);
						nextShaderMode %= 4;
					}
				break;
				case GLFW_KEY_N:
					debug = !debug;
					nextShaderMode = 1;
					nextLightingMode = 1;
					if( debug ){
						graphics.setShaderMode(0);
					}else{
						graphics.setLightingMode(0);
					}
				break;
				case GLFW_KEY_C:
					character = false;
				break;
			}
			camera.move(0.1f * direction);
		}
		else{
			switch( key ){
				case GLFW_KEY_D:
					if( debug ){
						graphics.setShaderMode(nextShaderMode++);
						nextShaderMode %= 4;
					}else{
						debug = true;
						nextShaderMode = 0;
						graphics.setShaderMode(nextShaderMode++);
					}
				break;
				case GLFW_KEY_L:
					graphics.setShaderMode(0);
					graphics.setLightingMode(nextLightingMode++);
					nextLightingMode %= 4;
				break;
				case GLFW_KEY_S:
					debug = !debug;
					nextShaderMode = 1;
					nextLightingMode = 1;
					if( debug ){
						graphics.setShaderMode(0);
					}
				break;
				case GLFW_KEY_C:
					character = true;
				break;
			}
		}
	}
}

void ModelLoader::click_callback(GLFWwindow *window, int button, int action, int mods){
	if( button == GLFW_MOUSE_BUTTON_LEFT && !rightMouseDown){
		if( action == GLFW_PRESS ){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwGetCursorPos(window, &xprev, &yprev);
			leftMouseDown = true;
		}else if( action == GLFW_RELEASE ){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			leftMouseDown = false;
		}
	}else if( button == GLFW_MOUSE_BUTTON_RIGHT && !leftMouseDown ){
		if( action == GLFW_PRESS ){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwGetCursorPos(window, &xprev, &yprev);
			rightMouseDown = true;
		}else if( action == GLFW_RELEASE ){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			rightMouseDown = false;
		}
	}
}

void ModelLoader::cursor_position_callback(GLFWwindow *window, double xpos, double ypos){
	if( leftMouseDown ){
		double deltaX = xprev - xpos;
		double deltaY = ypos - yprev;
		camera.orbit(2 * M_PI * deltaY/windowY, 2 * M_PI * deltaX/windowX);
		xprev = xpos;
		yprev = ypos;
	}else if( rightMouseDown ){
		double deltaY = yprev - ypos;
		camera.zoom(2 * M_PI * deltaY/windowY);
		xprev = xpos;
		yprev = ypos;
	}
}

void ModelLoader::window_resize_callback(GLFWwindow *window, int x, int y){
	camera.setWindowSize(x, y);
}

void ModelLoader::registerCallbacks(){
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, click_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(window, window_resize_callback);
}

void ModelLoader::start(){
	registerCallbacks();
	std::chrono::time_point<std::chrono::system_clock> t0 = std::chrono::system_clock::now();
	std::chrono::duration<float> t;
	while( !glfwWindowShouldClose(window) ){
		t = std::chrono::system_clock::now() - t0;
		graphics.renderFrame(t.count());
	}
	glfwDestroyWindow(window);
    glfwTerminate();
}















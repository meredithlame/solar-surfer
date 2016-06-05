#include <string>
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics.hpp"
#include "shader.hpp"

#include "Model.hpp"

#define N_SHADERS 4

Graphics::Graphics(std::vector<Entity> *entities, Camera *camera, int xWindowSize, int yWindowSize){
	this->entities = entities;
	this->camera = camera;
	windowSizeX = xWindowSize;
	windowSizeY = yWindowSize;
	shaderMode = LIGHT_TEXTURE;
	lightingMode = BLUE_LIGHT;
}

void Graphics::setData(std::vector<Entity> *entities, Camera *camera){
	this->entities = entities;
	this->camera = camera;
}

void Graphics::renderFrame(float t){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderPIDs[shaderMode]);

	if( lightingMode == YELLOW_LIGHT ){
		// Update light position
		float x = 8 * cos(2 * M_PI * t/6);
		float y = 8 * sin(2 * M_PI * t/6);
		glm::vec4 lightPosition = glm::vec4(x, y, 0.0f, 1.0f);
		GLint lightPosHandle = glGetUniformLocation(shaderPIDs[shaderMode], "light_position");
		glUniform4fv(lightPosHandle, 1, glm::value_ptr(lightPosition));
	}
	for( int i=0; i<entities->size(); i++ ){
		Entity current = entities->at(i);
		current.render(camera->getProjection(), camera->getView(), shaderPIDs[shaderMode]);
	}
	
	glFlush();
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Graphics::initialiseShaders(){
	shaderPIDs.resize(N_SHADERS);
	shaderPIDs[LIGHT_TEXTURE] = (compileShader("light_texture"));
	shaderPIDs[NORM_DEBUG] = (compileShader("debug_normal"));
	shaderPIDs[DIFFUSE_DEBUG] = (compileShader("debug_diffuse"));
	shaderPIDs[WIREFRAME_DEBUG] = (compileShader("debug_wireframe"));
}

unsigned int Graphics::compileShader(std::string name){
	unsigned int PID = LoadShaders((name + ".vert").c_str(), (name + ".frag").c_str());
	if( PID == 0 ){
		exit(1);
	}
	return PID;
}

// TODO: time and position changing, call at render
// Seperate into lighting module
void Graphics::setLighting(unsigned int PID, float t){
	glUseProgram(PID);

	glm::vec4 lightPosition;

	glm::vec3 lightAmbient;
	glm::vec3 lightDiffuse;
	glm::vec3 lightSpecular;

	if( lightingMode == BLUE_LIGHT ){
		// Load aerial point light
		lightPosition = glm::vec4(0.0f, 8.0f, 0.0f, 1.0f);
		lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		lightDiffuse = glm::vec3(0.5f, 0.5f, 1.0f);
		lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	}else if( lightingMode == HEAD_LIGHT ){
		lightPosition = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	}else if( lightingMode == YELLOW_LIGHT ){
		lightPosition = glm::vec4(8.0f, 0.0f, 0.0f, 1.0f);
		lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	}else if( lightingMode == DARKNESS ){
		lightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
		lightDiffuse = glm::vec3(0.0f, 0.0f, 0.0f);
		lightSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	GLint lightPosHandle = glGetUniformLocation(PID, "light_position");
	glUniform4fv(lightPosHandle, 1, glm::value_ptr(lightPosition));

	// Load point light radiant properties
	GLint ambientHandle = glGetUniformLocation(PID, "light_ambient");
	GLint diffHandle = glGetUniformLocation(PID, "light_diffuse");
	GLint specHandle = glGetUniformLocation(PID, "light_specular");

	glUniform3fv(ambientHandle, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(diffHandle, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(specHandle, 1, glm::value_ptr(lightSpecular));
}

void Graphics::setShaderMode(int mode){
	shaderMode = (shader_mode)mode;
	if( shaderMode == WIREFRAME_DEBUG ){
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}else{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
}

void Graphics::setLightingMode(int mode){
	lightingMode = (lighting_mode)mode;
	setLighting(shaderPIDs[shaderMode]);
}


void error_callback(int error, const char* description){
	fputs(description, stderr);
	fputs("\n", stderr);
}

void Graphics::initWindow(){
	glfwSetErrorCallback(error_callback);

	if( !glfwInit() ){
		exit(1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(600, 600, "Assignment 3", NULL, NULL);

	if( !window ){
		glfwTerminate();
		exit(0);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glewExperimental = true;
	if( glewInit() != GLEW_OK ){
		fprintf(stderr, "GLEW initialisation failed\n");
		exit(1);
	}

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	initialiseShaders();
	setLighting(shaderPIDs[shaderMode]);
}

GLFWwindow *Graphics::getWindow(){
	return window;
}


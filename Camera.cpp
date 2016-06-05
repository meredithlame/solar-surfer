#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <iostream>

#include "Camera.hpp"

Camera::Camera(int xsize, int ysize, float fovy, float near, float far){
	// Flags
	orientationFixed = false;
	eyeChanged = true;
	projectionChanged = true;

	// Default values
	position = glm::vec3(0.0f);
	orientation = glm::vec3(0.0f, 0.0f, 0.0f);
	target = glm::vec3(0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	view = glm::mat4();
	this->fovy = fovy;
	this->aspect = (float)xsize/ysize;
	this->near = near;
	this->far = far;

	xrot = 0;
	yrot = 0;
}

glm::vec3 Camera::getTarget(){
	return target;
}


glm::vec3 Camera::getPosition(){
	return position;
}

glm::vec3 Camera::getMoveDirection(){
	return glm::normalize(target - position);
}

glm::vec3 Camera::getUpDirection(){
	return glm::normalize(up);
}

glm::mat4 Camera::getProjection(){
	if( projectionChanged ){
		updateProjectionMatrix();
	}
	return projection;
}

glm::mat4 Camera::getView(){
	if( eyeChanged ){
		updateCameraMatrix();
	}
	return view;
}

float Camera::maxX(){
	float distance = std::abs(glm::length(target - position));
	return std::abs(2.0f * distance / atan((M_PI - aspect)/2.0f));
}

void Camera::setWindowSize(int x, int y){
	aspect = (float)x/y;
	projectionChanged = true;
}

glm::mat4 Camera::getCameraMatrix(){
	if( projectionChanged ){
		updateProjectionMatrix();
	}
	if( eyeChanged ){
		updateCameraMatrix();
	}
	return camera;
}

inline void Camera::updateCameraMatrix(){
	// Calculate view:
	if( orientationFixed ){
		view = glm::lookAt(position, target, up);
	}
	else{
		view = glm::translate(glm::mat4(), -position);
		view = glm::rotate(view, -orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, -orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, -orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	camera = projection * view;
	eyeChanged = false;
}

inline void Camera::updateProjectionMatrix(){
	projection = glm::perspective(fovy, aspect, near, far);
	camera = projection * view;
	projectionChanged = false;
}

void Camera::lookAt(glm::vec3 target, glm::vec3 up){
	view = glm::lookAt(position, target, up);
	eyeChanged = false;
	camera = projection * view;
}

void Camera::lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up){
	view = glm::lookAt(position, target, up);
	eyeChanged = false;
	camera = projection * view;
}

void Camera::fixedLookAt(){
	if( !orientationFixed ){
		view = glm::lookAt(position, target, up);
		orientationFixed = true;
		eyeChanged = true;
	}
}

void Camera::fixedLookAt(glm::vec3 target, glm::vec3 up){
	lookAt(target, up);
	this->target = target;
	this->up = up;
	orientationFixed = true;
}

void Camera::releaseLookAt(){
	eyeChanged = orientationFixed;
	orientationFixed = false;
}

void Camera::move(glm::vec3 movement){
	position += movement;
	target += movement;
	lookAt(target, up);
}

void Camera::reposition(glm::vec3 position){
	this->position = position;
	eyeChanged = true;
}

void Camera::retarget(glm::vec3 target){
	this->target = target;
	eyeChanged = true;
}

void Camera::orbit(float radAboutX, float radAboutY){
	// Make target effective origin
	position -= target;
	xrot += radAboutX;
	yrot += radAboutX;
	// rotate position by xrad, yrad in camera coords
	glm::vec3 viewX = glm::vec3(glm::row(view, 0));
	position = glm::rotate(position, -radAboutX, viewX);
	up = glm::rotate(up, -radAboutX, viewX);
	position = glm::rotate(position, radAboutY, up);

	// Reset position of target
	position += target;
	lookAt(target, up);
}

void Camera::zoom(float amount){
	glm::vec3 direction = target - position;
	if( glm::length(direction) > amount ){
		position += amount * glm::normalize(direction);
		eyeChanged = true;
	}
}


#include "Entity.hpp"

#include <glm/gtc/matrix_transform.hpp>

/**
 * Entity constructor simply takes pointer to Model object
 * and sets default values for position, orientation, and scale.
 * Position, orientation, and size, must then be updated separately
 */
Entity::Entity(Model *model){
	this->model = model;

	// Transformation traits
	position = glm::vec3(0.0f);
	orientation = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);

	// Tranformation matrix
	transformation = glm::mat4(1.0f);
}

void Entity::updateTransformation(){
	transformation = glm::translate(glm::mat4(), position);
	transformation = glm::scale(transformation, scale);
	transformation = glm::rotate(transformation, orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	transformation = glm::rotate(transformation, orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transformation = glm::rotate(transformation, orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

// Rendering
void Entity::render(glm::mat4 projection, glm::mat4 camera, unsigned int PID){
	updateTransformation();
	model->render(projection, camera * transformation, camera, PID);
}

// Getting tranformation properties
glm::vec3 Entity::getPosition(){
	return position;
}

glm::vec3 Entity::getOrientation(){
	return orientation;
}

glm::vec3 Entity::getScale(){
	return scale;
}

/**
 * --- Setting transformation properties ---
 * The following methods update the position, orientation, 
 * or scale to an entirely new value.
 */
// Position
void Entity::reposition(glm::vec3 pos){
	position = pos;
}

void Entity::reposition(float xpos, float ypos, float zpos){
	position = glm::vec3(xpos, ypos, zpos);
}

// Orientation
void Entity::reorient(float radians, glm::vec3 axis){
	axis /= length(axis);
	orientation = radians * axis;
}

void Entity::reorient(glm::vec3 orientation){
	this->orientation = orientation;
}

void Entity::reorient(float xrad, float yrad, float zrad){
	orientation = glm::vec3(xrad, yrad, zrad);
}

// Scale
void Entity::rescale(glm::vec3 scale){
	this->scale = scale;
}

void Entity::rescale(float xscale, float yscale, float zscale){
	scale = glm::vec3(xscale, yscale, zscale);
}

void Entity::resize(float scaleFactor){
	scale = glm::vec3(scaleFactor);
}

/**
 * --- Altering transformation properties ---
 * The following methods modify the current position,
 * orientation, or scale according to the specified value.
 */
// Position
void Entity::move(float distance, glm::vec3 direction){
	direction /= length(direction);
	position += distance * direction;
}

void Entity::move(glm::vec3 movement){
	position += movement;
}

void Entity::move(float xdist, float ydist, float zdist){
	position += glm::vec3(xdist, ydist, zdist);
}

// Orientation
void Entity::rotate(float radians, glm::vec3 axis){
	axis /= length(axis);
	orientation += radians * axis;
}

void Entity::rotate(glm::vec3 rotation){
	orientation += rotation;
}

void Entity::rotate(float xrad, float yrad, float zrad){
	orientation += glm::vec3(xrad, yrad, zrad);
}

// Scale
void Entity::stretch(glm::vec3 stretchFactors){
	scale *= stretchFactors;
}

void Entity::stretch(float xstretch, float ystretch, float zstretch){
	scale *= glm::vec3(xstretch, ystretch, zstretch);
}

void Entity::expand(float scaleFactor){
	scale *= scaleFactor;
}



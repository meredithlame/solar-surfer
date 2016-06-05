#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>

#include "Model.hpp"

/**
 * The Entity class represents a single object in the world.
 * An Entity object contains a VAO handle, as well as the information required to position
 * the Model corresponding to the VAO correctly in the world.
 * The full interface will contain methods for:
 * 		Getting and setting transformation matrix properties:
 * 			Position, orientation, size/scaling
 * 		Altering current transformation matrix properties:
 * 			Moving, rotating, expanding, stretching
 */

class Entity{
	// Model
	Model *model;

	// Transformation matrix traits
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 scale;

	// Indicates whether transform needs to be updated on next render
	bool update;
	glm::mat4 transformation;

	void updateTransformation();
public:
	Entity(Model *model);
	void render(glm::mat4 projection, glm::mat4 camera, unsigned int PID);

	// Getting tranformation properties
	glm::mat4 getTransform();
	glm::vec3 getPosition();
	glm::vec3 getOrientation();
	glm::vec3 getScale();

	// Setting tranformation properties
	void reposition(glm::vec3 pos);
	void reposition(float xpos, float ypos, float zpos);

	void reorient(float radians, glm::vec3 axis);
	void reorient(glm::vec3 orientation);
	void reorient(float xrad, float yrad, float zrad);

	void rescale(glm::vec3 scale);
	void rescale(float xscale, float yscale, float zscale);
	void resize(float scaleFactor);

	// Altering transformation properties
	void move(float distance, glm::vec3 direction);
	void move(glm::vec3 movement);
	void move(float xdist, float ydist, float zdist);

	void rotate(float radians, glm::vec3 axis);
	void rotate(glm::vec3 rotation);
	void rotate(float xrad, float yrad, float zrad);

	void stretch(glm::vec3 stretchFactors);
	void stretch(float xstretch, float ystretch, float zstretch);
	void expand(float scaleFactor);
};

#endif
#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <glm/glm.hpp>

#include "tiny_obj_loader.h"


/**
 * The Model class is contains the model data (as parsed by Tiny Object)
 * and is responsible for loading and then rendering this data using a
 * given transformation matrix.
 * The full interface will contain methods for:
 * 		Generating the Tiny Object data (from provided OBJ filepath)
 * 		Retrieving and modifying object data (VAO handle, vertices, normals, indices, etc.)
 * Children of the model class can implement their own rules for generating a VAO
 * as well as rendering. The render method is virtual, so children can take advantage
 * of polymorphism.
 */

class Model{
protected:
	std::vector<unsigned int> VAOs;
	unsigned int *texIDs;

	static unsigned int nextTexUnit;

	// Model data
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string objDir;

	// Bound
	float extremum;

	void generateVAOs();
	void genTextures();
	void loadTexture(std::string texpath);
	void loadDefaultTexture();
public:
	Model(std::string objPath);
	virtual void render(glm::mat4 projection, glm::mat4 modelview, glm::mat4 view, unsigned int PID);

	// Bounds
	void calculateExtremum();
	float getExtremum();
};

#endif
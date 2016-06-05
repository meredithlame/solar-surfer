#include "Model.hpp"

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/rotate_vector.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.hpp"

#define VALS_PER_VERT 3
#define VALS_PER_NORM 3
#define VALS_PER_TEXEL 2

unsigned int Model::nextTexUnit = 0;


// TESTING
GLFWwindow *window;

Model::Model(std::string objPath){
	int pos = objPath.rfind("/");
	if( pos != std::string::npos ){
		objDir = objPath.substr(0, pos + 1);
	}else{
		objDir = "";
	}

	// Populate shapes and materials (using Tiny obj loader)
	std::string error;
	bool nonfatal = tinyobj::LoadObj(shapes, materials, error, objPath.c_str(), objDir.c_str());
	if( !error.empty() ){
		std::cerr << error;
	}
	if( !nonfatal ){
		exit(1);
	}
	generateVAOs();
	genTextures();
	calculateExtremum();
}

/**
 * generateVao creates a VAO based on the data provided by Tiny
 * Object that was parsed from the OBJ file.
 */
void Model::generateVAOs(){
	// Create VAO for each shape
	// This is simpler and allows for per-shape uniform variables (other functionality for this NYI)
	VAOs.resize(shapes.size());
	glGenVertexArrays(shapes.size(), &VAOs[0]);

	for( int i=0; i<shapes.size(); i++ ){
		// Current mesh
		tinyobj::mesh_t mesh = shapes[i].mesh;

		glBindVertexArray(VAOs[i]);

		// Set up buffers for vertices, normals, texcoords, and indices
		unsigned int buffer[4];
		glGenBuffers(4, buffer);

		// Load vertices
		glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
		glBufferData(GL_ARRAY_BUFFER, mesh.positions.size() * sizeof(float), &mesh.positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

		// Load normals
		if( mesh.normals.size() == 0 ){
			mesh.normals.resize(mesh.positions.size(), 0.0f);
		}
		glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), &mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);			

		// Load texcoords
		if( mesh.texcoords.size() == 0 ){
			mesh.texcoords.resize(mesh.positions.size(), 0.0f);
		}
		glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
		glBufferData(GL_ARRAY_BUFFER, mesh.texcoords.size() * sizeof(float), &mesh.texcoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, VALS_PER_TEXEL, GL_FLOAT, GL_FALSE, 0, 0);

		// Load indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);
	}
}

// 	unsigned int PID = LoadShaders()
// }
// Data driven
// Modular
// Parallelisable


void Model::genTextures(){
	texIDs = new unsigned int[materials.size()];
	glGenTextures(materials.size(), texIDs);
	for( int i=0; i<materials.size(); i++ ){
		glActiveTexture(GL_TEXTURE0 + nextTexUnit++);
		glBindTexture(GL_TEXTURE_2D, texIDs[i]);

		std::string texname = materials[i].diffuse_texname;
		if( !texname.empty() ){
			loadTexture(objDir + texname);
		}else{
			loadDefaultTexture();
		}
	}
}

/**
 * loadTexture is responsible for loading a single texture
 * from an image file specified by texPath into the currently
 * active texture.
 * @param texPath Path to the texture image file
 */
void Model::loadTexture(std::string texPath){
	int x, y, n;
	unsigned char *data = stbi_load(texPath.c_str(), &x, &y, &n, 3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Model::loadDefaultTexture(){
	unsigned char colour[] = {255, 255, 255};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, colour);
}

void Model::render(glm::mat4 projection, glm::mat4 modelview, glm::mat4 view, unsigned int PID){
	glUseProgram(PID);

	// Load transformation matrices
	GLint viewHandle = glGetUniformLocation(PID, "view_matrix");
	glUniformMatrix4fv(viewHandle, 1, GL_FALSE, glm::value_ptr(view));

	GLint modelViewHandle = glGetUniformLocation(PID, "modelview_matrix");
	glUniformMatrix4fv(modelViewHandle, 1, GL_FALSE, glm::value_ptr(modelview));

	GLint normalHandle = glGetUniformLocation(PID, "normal_matrix");
	glm::mat3 normal(modelview);
	glUniformMatrix3fv(normalHandle, 1, GL_FALSE, glm::value_ptr(normal));

	GLint projectionHandle = glGetUniformLocation(PID, "projection_matrix");
	glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, glm::value_ptr(projection));

	for( int i=0; i<shapes.size(); i++ ){
		// Load shape-specific uniform variables
		unsigned int matID = shapes[i].mesh.material_ids[0];

		// Load lighting material properties
		GLint ambientHandle = glGetUniformLocation(PID, "ambient");
		GLint diffHandle = glGetUniformLocation(PID, "diffuse");
		GLint specHandle = glGetUniformLocation(PID, "specular");
		GLint shininessHandle = glGetUniformLocation(PID, "shininess");
		glUniform3fv(ambientHandle, 1, &(materials[matID].ambient[0]));
		glUniform3fv(diffHandle, 1, &materials[matID].diffuse[0]);
		glUniform3fv(specHandle, 1, &materials[matID].specular[0]);
		glUniform1fv(shininessHandle, 1, &materials[matID].shininess);

		// Load textures
		GLint diffmapHandle = glGetUniformLocation(PID, "diffmap");
		glUniform1i(diffmapHandle, matID);

		glBindVertexArray(VAOs[i]);
		glDrawElements(GL_TRIANGLES, shapes[i].mesh.indices.size() * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

		glBindVertexArray(0);
	}
}

void Model::calculateExtremum(){
	extremum = 0.0f;
	for( int i=0; i<shapes.size(); i++ ){
		std::vector<float> positions = shapes[i].mesh.positions;
		for( int j=0; j<positions.size(); j++ ){
			extremum = std::max(extremum, std::abs(positions[j]));
			// std::cout << "E " << std::abs(positions[j]) << std::endl;
		}
	}
}

float Model::getExtremum(){
	return extremum;
}

// float Model::xMax(){
// 	return xmax;
// }

// float Model::yMax(){
// 	return ymax;
// }

// float Model::zMax(){
// 	return zmax;
// }

// float Model::xMin(){
// 	return xmax;
// }

// float Model::yMin(){
// 	return ymax;
// }

// float Model::zMin(){
// 	return zmax;
// }


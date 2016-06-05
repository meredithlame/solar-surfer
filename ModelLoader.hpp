#include <vector>

#include "Entity.hpp"
#include "Model.hpp"
#include "Graphics.hpp"

class ModelLoader{
	// Modules
	static Graphics graphics;
	static Camera camera;

	// Window
	GLFWwindow *window;
	static int windowX, windowY;

	// Input
	static bool leftMouseDown;
	static bool rightMouseDown;
	static bool debug;
	static bool character;
	static double xprev, yprev;
	static int nextShaderMode;
	static int nextLightingMode;
	
	// Data
	std::vector<Entity> entities;
	std::vector<Model> models;
	float xmax, ymax, zmax;
	float xmin, ymin, zmin;

	void loadModel(std::string path);
	void registerCallbacks();
	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void click_callback(GLFWwindow *window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
	static void window_resize_callback(GLFWwindow *window, int x, int y);
public:
	// Loading models
	ModelLoader();
	void initialise(std::vector<std::string> paths);
	void start();

	static void setCharacter(bool c);
	// Camera controls
	void initCamera();
};
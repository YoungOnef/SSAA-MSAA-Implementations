#include "Includes.h"
#include "MsaaScene.h"
#include "SsaaScene.h"
#include "NormalScene.h"
#include <sstream>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Camera_settings camera_settings{ 800, 800, 0.1, 100.0 };
Camera* camera = nullptr;
int width = 800;
int height = 800;
float lastX = width / 2.0f;
float lastY = height / 2.0f;
Timer timer;
bool flipScene = false;

MsaaScene* MSAAScene = nullptr;
SsaaScene* SSAAScene = nullptr;
NormalScene* normalScene = nullptr;

bool			saveToFile = false;
ofstream		logCSV;
const char* windowTitle = "Coursework";

int main()
{
	// Ask the user what to render
	std::cout << "(default normal) Choose rendering mode:" << std::endl;
	std::cout << "1 - Normal" << std::endl;
	std::cout << "2 - MSAA" << std::endl;
	std::cout << "3 - SSAA" << std::endl;
	std::string input1;
	std::getline(std::cin, input1);
	int renderMode = 1; // Default to normal rendering
	if (!input1.empty()) {
		std::stringstream ss(input1);
		ss >> renderMode;
	}
	if (renderMode < 1 || renderMode > 3) {
		std::cout << "Invalid rendering mode, defaulting to normal." << std::endl;
		renderMode = 1;
	}



	// Prompt the user for the window width or use the default value
	std::cout << "Enter the width of the window (default value 800): ";
	std::string input;
	std::getline(std::cin, input);
	if (!input.empty()) {
		std::stringstream ss(input);
		ss >> width;
	}

	// Prompt the user for the window height or use the default value
	std::cout << "Enter the height of the window (default value 800): ";
	std::getline(std::cin, input);
	if (!input.empty()) {
		std::stringstream ss(input);
		ss >> height;
	}
	lastX = width / 2.0f;
	lastY = height / 2.0f;

	int aaResolution = 2;
	// ask user which resolution for SSAA
	if (renderMode == 3) {
		std::cout << "Enter the SSAA resolution (default value 2): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			std::stringstream ss(input);
			ss >> aaResolution;
		}
	}

	
	int aaSamples = 2;
	if (renderMode == 2) {
		std::cout << "Enter the number of samples for MSAA (default sample 2): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			std::stringstream ss(input);
			ss >> aaSamples;
		}
	}
	else if (renderMode == 3) {
		std::cout << "Enter the number of samples for SSAA (default sample 2): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			std::stringstream ss(input);
			ss >> aaSamples;
		}
	}

	//creating the camera
	camera = new Camera(Camera_settings{ static_cast<unsigned int>(width) , static_cast<unsigned int>(height) , 0.1, 100.0 }, glm::vec3(0.0f, 2.5f, 10.0f));


	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// specifies the number of samples to use for multisampling
	if (renderMode == 2) {
		glfwWindowHint(GLFW_SAMPLES, aaSamples);
	}

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(width, height, "Real-Time Rendering", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSwapInterval(0);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling
	glFrontFace(GL_CCW);//Specifies which winding order if front facing
	glEnable(GL_BLEND);
	
	if (renderMode == 2) {
		glEnable(GL_MULTISAMPLE);
	}
	TexturedQuad* SceneQuad = nullptr;
	TexturedQuad* texturedQuad = nullptr;

	// constructor for the scene / shader / texture / 
	if (renderMode == 1) {
		normalScene = new NormalScene(camera, width, height);
		SceneQuad = new TexturedQuad(normalScene->getEarthSceneTexture(), true);
	}
	else if (renderMode == 2) {
		MSAAScene = new MsaaScene(camera, width, height, 8);
		SceneQuad = new TexturedQuad(MSAAScene->getEarthSceneTexture(), true);
	}
	else if (renderMode == 3) {
		SSAAScene = new SsaaScene(camera, width, height, aaResolution);
		SceneQuad = new TexturedQuad(SSAAScene->getEarthSceneTexture(), width, height, aaSamples, true);
	}

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		timer.tick();

		// rendering
		if (renderMode == 1) {
			
			normalScene->render();
		}
		else if (renderMode == 2) {
			
			MSAAScene->render();
		}
		else if (renderMode == 3) {
			
			SSAAScene->render();
		}
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(0.28, 0.28, 0.28));
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 projection = camera->getProjectionMatrix();
		glm::mat4 viewProjection = projection * view;

		//displaying on the screen the quad in which the shader is rendering
		if (SceneQuad)
			SceneQuad->render(glm::mat4(1.0));


		string title = "FPS: " + std::to_string(timer.averageFPS()) + " SPF: " + std::to_string(timer.currentSPF());
		glfwSetWindowTitle(window, title.c_str());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera->updateScreenSize(width, height);
}

// glfw: whenever a key is pressed, this callback is called
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		flipScene = !flipScene;
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		camera->processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->processMouseScroll(yoffset);
}
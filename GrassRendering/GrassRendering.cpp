#pragma once
#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>
#include <../glm/gtc/type_ptr.hpp>
#include "HighMapLoad.h"
#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include"GrassGenerate.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
 
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0,0,0));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//wind处理优化
int windactive = 0;
float grasstime = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	/*glEnable(GL_CULL_FACE);*/
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// build and compile shaders
	// -------------------------

	GrassRelatedModel grassrelatemodel;
	grassrelatemodel.LoadGrassPlane();
	grassrelatemodel.LoadGrass();
	/*Terrain terrain;
	terrain.loadTerrain("D:\\GraduationProject\\GrassRendering\\GrassRendering\\Heightmap.bmp","D:\\GraduationProject\\Graphbase\\Rockyground.jpg", "D:\\GraduationProject\\Graphbase\\LavaLand.jpg");
	Shader terrainShader("terrain.vert", "terrain.frag");*/
	Shader grassShader("grass.vert", "grass.frag", nullptr, "grass.tesc", "grass.tese");
	// draw in wireframe
	Shader grassGroundShader("grassground.vert", "grassground.frag");

	grassShader.use();
	glm::vec3 FirstgrassPlane,SecondgrassPlane;
	grassrelatemodel.getGrassPlane(FirstgrassPlane,SecondgrassPlane);
	glm::vec3 fieldRect[2] = { FirstgrassPlane - glm::vec3(1.0f, 0.0f, 1.0f) ,
								SecondgrassPlane + glm::vec3(1.0f,0.0f,1.0f) };
	/*test(*fieldRect);*/
	grassShader.setVec3("fieldRect", *fieldRect, 2);
	//ofstream outputfile;
	//outputfile.open("C:\\Users\\18838\\Desktop\\test.txt");


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = (currentFrame - lastFrame);
		lastFrame = currentFrame;
		/*outputfile << deltaTime << endl;*/
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		/*terrainShader.use();*/
		grassGroundShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
	/*	terrainShader.setMat4("projection", projection);
		terrainShader.setMat4("view", view);*/
		grassGroundShader.setMat4("projection", projection);
		grassGroundShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		/*terrainShader.setMat4("model", model);
		terrain.Draw(terrainShader);*/
		grassGroundShader.setMat4("object", model);
		grassrelatemodel.DrawGrassPlane(grassGroundShader,model);

		grassShader.use();
		grassShader.setMat4("objectTransform", model);
		grassShader.setMat4("viewTransform", view);
		grassShader.setMat4("projectionTransform", projection);
		grassShader.setVec3("cameraPos", camera.Position);

		grassShader.setFloat("time", grasstime);
		grassShader.setInt("windActive", windactive);
		grassrelatemodel.DrawGrass(grassShader,model);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		grasstime += 0.005f;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//outputfile.close();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		windactive = (windactive + 1) % 2;
		grasstime = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
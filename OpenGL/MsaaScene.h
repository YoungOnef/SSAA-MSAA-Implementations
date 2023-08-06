#ifndef MSAA_SCENE_H
#define MSAA_SCENE_H

#include "Sphere.h"
#include "Camera.h"

class Sphere;


class MsaaScene {


	GLuint earthTexture;
	GLuint LightShader;

	// Camera uniforms
	GLint							model;
	GLint							view;
	GLint							projection;

	GLint							cameraPosLocation;


	unsigned int textureColorBufferMultiSampled;
	unsigned int textureDepthBufferMultiSampled;
	unsigned int framebuffer;
	unsigned int textureColorBuffer;
	unsigned int intermediateFBO;


	int screenWidth;
	int screenHeight;
	

	// Number of samples per pixel for MSAA
	int samples;

	//models
	Model* plane;
	Model* StandingPoint;
	Model* StandingPoint2;
	Model* Stand1_01;
	Model* Stand1_02;
	Model* Stand1_03;
	Model* Stand1_04;
	//textures
	GLuint marbleTex;
	GLuint metalTex;
	GLuint blockyTex;
	GLuint StandTextur1;
	GLuint metalTexSpec;
	GLuint Stand1_01Tex;
	GLuint Stand1_02Tex;
	GLuint Stand1_03Tex;
	GLuint Stand1_04Tex;
	GLuint Stand1_05Tex;
	GLuint Stand1_06Tex;
	GLuint Stand1_07Tex;

public:
	Camera* sceneCam = nullptr;
	MsaaScene(Camera* mainCam, int newWidth, int newHeight, int sampleRate) {
		sceneCam = mainCam;
		screenWidth = newWidth;
		screenHeight = newHeight;
		samples = sampleRate;



		plane = new Model("Resources\\Models\\Plane.obj");
		StandingPoint = new Model("Resources\\Models\\StandingPoint.obj");
		StandingPoint2 = new Model("Resources\\Models\\StandingPoint2.obj");
		Stand1_01 = new Model("Resources\\Models\\Stand1_01.obj");
		Stand1_02 = new Model("Resources\\Models\\Stand1_02.obj");
		Stand1_03 = new Model("Resources\\Models\\Stand1_03.obj");
		Stand1_04 = new Model("Resources\\Models\\Stand1_04.obj");



		marbleTex = TextureLoader::loadTexture("Resources\\Models\\marble_texture.jpg");
		metalTex = TextureLoader::loadTexture("Resources\\Models\\metal_texture.png");
		blockyTex = TextureLoader::loadTexture("Resources\\Models\\block_texture.jpg");
		Stand1_01Tex = TextureLoader::loadTexture("Resources\\Models\\Stand1_01.jpg");
		Stand1_02Tex = TextureLoader::loadTexture("Resources\\Models\\Stand1_02.jpg");
		Stand1_03Tex = TextureLoader::loadTexture("Resources\\Models\\Stand1_03.jpg");
		Stand1_04Tex = TextureLoader::loadTexture("Resources\\Models\\Stand1_04.jpg");
		Stand1_05Tex = TextureLoader::loadTexture("Resources\\Models\\Stand1_05.jpg");
		Stand1_06Tex = TextureLoader::loadTexture("Resources\\Models\\Stand1_06.jpg");
		Stand1_07Tex = TextureLoader::loadTexture("Resources\\Models\\Stand1_07.jpg");

		GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
			string("Resources\\Shaders\\Light-Shader.vs"),
			string("Resources\\Shaders\\Light-Shader.fs"),
			&LightShader);


		model = glGetUniformLocation(LightShader, "model");
		view = glGetUniformLocation(LightShader, "view");
		projection = glGetUniformLocation(LightShader, "projection");


		// generate multisampled color attachment texture
		glGenTextures(1, &textureColorBufferMultiSampled);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, screenWidth, screenHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		// generate multisampled depth attachment texture
		glGenTextures(1, &textureDepthBufferMultiSampled);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureDepthBufferMultiSampled);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		// create a (also multisampled) framebuffer object
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// attach textures to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, textureDepthBufferMultiSampled, 0);

		// check if FBO is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;


		// create a normal color attachment texture
		glGenTextures(1, &textureColorBuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// create another FBO that has a normal color attachment texture
		glGenFramebuffers(1, &intermediateFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

		// attach texture to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

		// check if FBO is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
	}

	~MsaaScene() {

	}

	Camera* MsaaScene::getEarthSceneCamera() {

		return sceneCam;
	}

	GLuint MsaaScene::getEarthSceneTexture() {

		return textureColorBuffer;
	}
	void render() {

		// Bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// Clear the screen (i.e. the texture)
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		// Set viewport to specified texture size (see above)
		glViewport(0, 0, screenHeight, screenHeight);

		glm::mat4 T = sceneCam->getProjectionMatrix() * sceneCam->getViewMatrix();
		glm::mat4 modelTransform;
		modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));


		//// Calculate inverse transpose of the modelling transform for correct transformation of normal vectors
		glm::mat4 inverseTranspose = glm::transpose(glm::inverse(modelTransform));;

		// Set shader program
		glUseProgram(LightShader);

		// Set camera uniforms
		glm::mat4 viewProjectionMatrix = sceneCam->getProjectionMatrix() * sceneCam->getViewMatrix();
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(modelTransform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(inverseTranspose));
		glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));


		// Activate and Bind the texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, marbleTex);

		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render plane
		plane->draw(LightShader);

		// Render the StandingPoint
		glm::mat4  standingPointTransform = glm::translate(glm::mat4(1.0), glm::vec3(-1.0, 0.0, -1.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.01, 0.01, 0.01));
		glm::mat4 standingPointInvTranspose = glm::transpose(glm::inverse(standingPointTransform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPointTransform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPointInvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, metalTex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		StandingPoint->draw(LightShader);


		glm::mat4 standingPoint1Transform = glm::translate(glm::mat4(1.0), glm::vec3(-1, 2.5, -1.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.005, 0.005, 0.005));
		glm::mat4 standingPoin1tInvTranspose = glm::transpose(glm::inverse(standingPoint1Transform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPoint1Transform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPoin1tInvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Stand1_01Tex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		Stand1_01->draw(LightShader);

		glm::mat4  standingPoint2Transform = glm::translate(glm::mat4(1.0), glm::vec3(-2.0, 0.0, 2.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.01, 0.01, 0.01));
		glm::mat4 standingPoint2InvTranspose = glm::transpose(glm::inverse(standingPoint2Transform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPoint2Transform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPoint2InvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Stand1_02Tex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		StandingPoint2->draw(LightShader);

		glm::mat4 standingPoint3Transform = glm::translate(glm::mat4(1.0), glm::vec3(-2, 2.5, 2.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.005, 0.005, 0.005));
		glm::mat4 standingPoin3tInvTranspose = glm::transpose(glm::inverse(standingPoint3Transform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPoint3Transform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPoin3tInvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Stand1_03Tex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		Stand1_02->draw(LightShader);

		// Render the StandingPoint
		glm::mat4  standingPoint4Transform = glm::translate(glm::mat4(1.0), glm::vec3(1.0, 0.0, -1.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.01, 0.01, 0.01));
		glm::mat4 standingPoint4InvTranspose = glm::transpose(glm::inverse(standingPoint4Transform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPoint4Transform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPoint4InvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Stand1_04Tex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		StandingPoint->draw(LightShader);

		glm::mat4 standingPoint5Transform = glm::translate(glm::mat4(1.0), glm::vec3(1, 2.5, -1.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.005, 0.005, 0.005));
		glm::mat4 standingPoin5tInvTranspose = glm::transpose(glm::inverse(standingPoint5Transform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPoint5Transform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPoin5tInvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Stand1_05Tex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		Stand1_03->draw(LightShader);

		glm::mat4  standingPoint6Transform = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.0, 2.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.01, 0.01, 0.01));
		glm::mat4 standingPoint6InvTranspose = glm::transpose(glm::inverse(standingPoint6Transform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPoint6Transform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPoint6InvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Stand1_06Tex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		StandingPoint2->draw(LightShader);

		glm::mat4 standingPoint7Transform = glm::translate(glm::mat4(1.0), glm::vec3(2, 2.5, 2.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.005, 0.005, 0.005));
		glm::mat4 standingPoin7tInvTranspose = glm::transpose(glm::inverse(standingPoint7Transform));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(standingPoint7Transform));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(standingPoin7tInvTranspose));

		// Activate and bind the metal texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Stand1_07Tex);
		// Set the "textureSampler" uniform in the shader program to use texture unit 1
		glUniform1i(glGetUniformLocation(LightShader, "textureSampler"), 1);
		// Render the StandingPoint
		Stand1_04->draw(LightShader);
		// Restore default OpenGL shaders (Fixed function operations)
		glUseProgram(0);

		// bind both FBOs and resolve multisampling
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0,
			screenWidth, screenHeight,
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST);

		// Set OpenGL to render to the MAIN framebuffer (i.e. the screen itself!!)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

};

#endif
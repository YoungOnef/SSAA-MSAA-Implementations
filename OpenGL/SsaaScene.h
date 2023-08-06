#ifndef SSAA_SCENE_H
#define SSAA_SCENE_H

#include "Sphere.h"
#include "Camera.h"

class Sphere;


class SsaaScene {

private:
	GLuint SSAAShader;
	GLuint LightShader;

	// Camera uniforms
	GLint							model;
	GLint							view;
	GLint							projection;


	GLuint demoFBO;
	GLuint fboColourTexture;
	GLuint fboDepthTexture;
	bool fboOkay;

	int screenWidth; // Screen width
	int screenHeight; // Screen height

	int xResolution; // 2, 4, 8, 16, 32,


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
	SsaaScene(Camera* mainCam, int newWidth, int newHeight, int aaResolution) {
		sceneCam = mainCam;
		screenWidth = newWidth;
		screenHeight = newHeight;
		xResolution = aaResolution;

		cout << "Rendered Reselution within the FBO - screenWidth: " << screenWidth * xResolution << endl;
		cout << "Rendered Reselution within the FBO - screenHeight: " << screenHeight * xResolution << endl;

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

		// Get the location of uniform variables in the LightShader program
		model = glGetUniformLocation(LightShader, "model");
		view = glGetUniformLocation(LightShader, "view");
		projection = glGetUniformLocation(LightShader, "projection");
		// Create a framebuffer object and bind it
		glGenFramebuffers(1, &demoFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);
		// Create a texture to store the color information for the framebuffer
		glGenTextures(1, &fboColourTexture);
		glBindTexture(GL_TEXTURE_2D, fboColourTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth * xResolution, screenHeight * xResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// Set the texture filtering and wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		// Setup depth texture
		// Create a texture to store the depth information for the framebuffer
		glGenTextures(1, &fboDepthTexture);
		glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth * xResolution, screenHeight * xResolution, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
		// Set the texture filtering and wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		//
		// Attach textures to the FBO
		//

		// Attach the colour texture object to the framebuffer object's colour attachment point #0
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			fboColourTexture,
			0);

		// Attach the depth texture object to the framebuffer object's depth attachment point
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			fboDepthTexture,
			0);

		GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE) {
			fboOkay = false;
			cout << "Could not successfully create framebuffer object to render texture!" << endl;
		}
		else {
			fboOkay = true;
			cout << "FBO successfully created" << endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~SsaaScene() {

	}

	Camera* SsaaScene::getEarthSceneCamera() {

		return sceneCam;
	}

	GLuint SsaaScene::getEarthSceneTexture() {

		return fboColourTexture;
	}
	void render() {
		if (!fboOkay)
			return; // Don't render anything if the FBO was not created successfully

		// Bind framebuffer object so all rendering redirected to attached images (i.e. our texture)
		glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);

		// All rendering from this point goes to the bound textures (setup at initialisation time) and NOT the actual screen!!!!!

		// Clear the screen (i.e. the texture)
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set viewport to specified texture size (see above)
		glViewport(0, 0, screenWidth * xResolution, screenHeight * xResolution);

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


		// Set OpenGL to render to the MAIN framebuffer (i.e. the screen itself!!)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

};

#endif
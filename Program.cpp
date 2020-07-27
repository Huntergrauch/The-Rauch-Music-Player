#pragma once
#include "RauchAudio.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include "RauchInput.h"
#include "RauchGUI.h"
#include <glm/glm.hpp> //OpenGl Math Library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <portable-file-dialogs.h>


//forward declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void UpdateTime();

unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 1024;

bool submitted = false, browse = false;

int main()
{
	
	//GLFW initializtion
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);


	//GLFW Window Creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Rauch Audio Player", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	GLFWimage images[2];
	int width, height;
	images[0].pixels = stbi_load(".\\Textures\\TetraLogo.png", &images[0].width, &images[0].height, 0, 4);
	images[1].pixels = stbi_load(".\\Textures\\TetraLogoSmall.png", &images[1].width, &images[1].height, 0, 4);
	glfwSetWindowIcon(window, 2, images);


	//GLAD initilization
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //resize OpenGL Viewport if glfw Window is Resized
	glfwSetCursorPosCallback(window, mouse_position_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetCharCallback(window, character_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMaximizeWindow(window);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); //Set OpenGL Viewport Size

	//find limits of current system and write them to output
	int nrAttributes, maxTextureUnits, maxVertexTextureUnits, maxTextureLayers, maxTextureSize;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureUnits);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxTextureLayers);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	std::cout << "OpenGL GPU limits on this system: " << endl
		<< "Maximum number of vertex attributes supported: " << nrAttributes << std::endl;
	std::cout << "Maximum number of fragment texture units supported: " << maxTextureUnits << std::endl;
	std::cout << "Maximum number of vertex texture units supported: " << maxVertexTextureUnits << std::endl;
	std::cout << "Maximum number of Texture Layers: " << maxTextureLayers << endl;
	std::cout << "Maximum Texture Size: " << maxTextureSize << endl;

	//rendering settings

	//Enable Blending
	glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_SCISSOR_TEST);
	
	Shader TextShader(".\\Shaders\\VertexShaderText.vert", ".\\Shaders\\FragmentShaderText.frag");
	Shader RectShader(".\\Shaders\\VertexShaderRect.vert", ".\\Shaders\\FragmentShaderRect.frag");


	Font font(".\\Resources\\NotoSans-Regular.ttf");
	InputTextBox inputtext(&font, vec3(0.0f, 0.0f, 0.0f), 2.0f);
	Text titletext("The Rauch Audio Player", &font, vec3(0.2f, 0.0f, 0.2f), 1.5f);
	Text introtext("Type the path to a mp3 or wav file,", &font, vec3(0.1f, 0.0f, 0.1f), 0.8);
	Text TABtext("or Press TAB to open browse your files for one.", &font, vec3(0.1f, 0.0f, 0.1f), 0.8);
	Text entertext("Press ENTER to submit", &font, vec3(0.1f, 0.0f, 0.1f), 1.0);

	SolidRectangle rect; 
	SolidRectangle titlerect;
	rect.SetUpRect();
	titlerect.SetUpRect();

	InitializeAudio();
	
	ActiveInputTextBox = &inputtext;
	while (!glfwWindowShouldClose(window))
	{
		UpdateTime();
		process_input(window);
		//UPDATESOUND;

		if (submitted)
		{
			cout << "submitted path: " << inputtext.InputText.String.c_str() << endl;
			Audio testaudio;
			if (testaudio.Load(inputtext.InputText.String.c_str()) == 0)
			{
				StartAudio(testaudio);
				submitted = false;
				
				inputtext.InputText.String = "Audio Submitted";
			}
			else
			{
				inputtext.InputText.String = "Submitted File Invalid";
				submitted = false;
			}
			
		}

		if (browse)
		{
			vector<string> selection = pfd::open_file("Select a audio file").result();
			if (selection.size() > 0)
			{
				inputtext.InputText.String = selection[0];
			}
			browse = false;
		}

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); //Set OpenGL Viewport Size
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //draw blue background

		rect.Draw(RectShader, -0.5f, -0.5f, 1.0f, 1.0f, vec3(9.0f, 9.0f, 0.0f));
		titlerect.Draw(RectShader, -0.5f, 0.3f, 1.0f, 0.2f, vec3(0.5f, 0.5f, 0.0f));
		inputtext.Draw(TextShader, -0.25f, -0.25, RectShader, 0.5f);
		titletext.Draw(&TextShader, -(titletext.GetStringScreenWidth() / 2), 0.35);
		introtext.Draw(&TextShader, -(introtext.GetStringScreenWidth() / 2), 0.2);
		TABtext.Draw(&TextShader, -(TABtext.GetStringScreenWidth() / 2), 0.1);
		entertext.Draw(&TextShader, -(entertext.GetStringScreenWidth() / 2), 0.0);


		//swap buffers & check events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	font.DeleteFont();
	DeinitializeAudio();
}

float currentTime = 0.0f;
float lastTime = 0.0f;
float deltaTime = 0.0f;
//update the timer
void UpdateTime()
{
	lastTime = currentTime;
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
}

//when the screen is resized this resizes the window to fit it
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}
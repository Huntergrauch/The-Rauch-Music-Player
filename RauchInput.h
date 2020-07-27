#pragma once
#include "GLFW/glfw3.h" //OpenGL Initilization and Input Library
#include "RauchGUI.h"

extern bool submitted;
extern bool browse;

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
float lastMouseX = (float)SCR_WIDTH / 2.0;
float lastMouseY = (float)SCR_HEIGHT / 2.0;
bool firstusedmouse = true;

InputTextBox* ActiveInputTextBox;

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{

}
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

unsigned int BackspaceStatus = 0;
void process_input(GLFWwindow* window)
{
	//closing window with escape
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	
	//backspace ability on input boxes
	if (BackspaceStatus == 0 && (glfwGetKey(window, GLFW_KEY_BACKSPACE)
		== GLFW_PRESS) && ActiveInputTextBox->InputText.String.size() > 0)
	{
		ActiveInputTextBox->InputText.String.pop_back();
		BackspaceStatus++;
	}
		if (BackspaceStatus > 0)
		{
			BackspaceStatus++;
		}
		if (BackspaceStatus > 12)
		{
			BackspaceStatus = 0;
		}
	
	
}
void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	ActiveInputTextBox->InputText.String += codepoint;
	//cout << codepoint << endl;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//submitting text in textbox
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		submitted = true;
		//ActiveInputTextBox = NULL;
	}

	//browse for file
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		browse = true;
	}
}

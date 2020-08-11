#pragma once
#include "GLFW/glfw3.h" //OpenGL Initilization and Input Library
#include "RauchGUI.h"

extern bool browse;
extern Mouse_Status Mouse_Input_Status;

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
float lastMouseX = (float)SCR_WIDTH / 2.0;
float lastMouseY = (float)SCR_HEIGHT / 2.0;
bool firstusedmouse = true;
float MousePosX = 0;
float MousePosY = 0;

extern InputTextBox* ActiveInputTextBox;
extern bool PressedPause;
extern bool PressedLoop;
extern bool PressedShuffle;

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	MousePosX = ((float)xpos / (float)(SCR_WIDTH / 2)) - 1;
	MousePosY = ((float)(SCR_HEIGHT - ypos) / (float)(SCR_HEIGHT / 2)) - 1;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		Mouse_Input_Status.Left = action;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		Mouse_Input_Status.Middle = action;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		Mouse_Input_Status.Right = action;
	}
}

double YScrollOffset = 0;
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	
	YScrollOffset += yoffset;
	//cout << yoffset << YScrollOffset <<  endl;
}


unsigned int ITB_BackspaceStatus = 0, ITB_LeftStatus = 0, ITB_RightStatus = 0;
void process_input(GLFWwindow* window)
{
	
	//closing window with escape
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	
	//backspace ability on input boxes
	if (ITB_BackspaceStatus == 0 && (glfwGetKey(window, GLFW_KEY_BACKSPACE)
		== GLFW_PRESS) && ActiveInputTextBox->InputText.String.size() > 0
		&& ActiveInputTextBox != nullptr)
	{
		ActiveInputTextBox->InputText.String.pop_back();
		ITB_BackspaceStatus++;
	}
		if (ITB_BackspaceStatus > 0)
		{
			ITB_BackspaceStatus++;
		}
		if (ITB_BackspaceStatus > 12)
		{
			ITB_BackspaceStatus = 0;
		}
}
void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	if (ActiveInputTextBox != nullptr)
	{
		string codestring; 
		codestring = codepoint;
		ActiveInputTextBox->InputText.String += codestring;
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	//browse for file
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		browse = true;
	}

	if (glfwGetKey(window, GLFW_KEY_PAUSE) == GLFW_PRESS)
	{
		PressedPause = true;
	}
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
	{
		PressedShuffle = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
	{
		PressedLoop = true;
	}
}

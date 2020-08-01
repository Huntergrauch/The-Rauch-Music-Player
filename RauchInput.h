#pragma once
#include "GLFW/glfw3.h" //OpenGL Initilization and Input Library
#include "RauchGUI.h"

extern bool browse;

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
float lastMouseX = (float)SCR_WIDTH / 2.0;
float lastMouseY = (float)SCR_HEIGHT / 2.0;
bool firstusedmouse = true;

extern InputTextBox* ActiveInputTextBox;
extern ButtonManager buttonmanager;

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	int leftstate = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);


	float screenx = ((float)xpos / (float)(SCR_WIDTH / 2)) - 1;
	float screeny = ((float)(SCR_HEIGHT - ypos) / (float)(SCR_HEIGHT / 2)) - 1;

	if (leftstate == GLFW_PRESS)
	{
		//cout << "pressed at position: " << screenx  << ", " << screeny << endl;
		for (int i = 0; i < buttonmanager.Buttons.size();i++)
		{
			buttonmanager.Buttons[i]->CheckButton(screenx, screeny);
		}
	}
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

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
		== GLFW_PRESS) && ActiveInputTextBox->InputText.String.size() > 0)
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
}

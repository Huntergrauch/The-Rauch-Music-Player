//This file was written by Hunter Rauch for OpenGL Shaders
//Uses glad, and glm libraries
//Is used by RauchGUI.h and RauchSprite.h

//preprocessor directive to make sure header is only compiled once
#ifndef SHADER_H
#define SHADER_H

#pragma once
#include <string>
#include <iostream>
#include "glad/glad.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp> //OpenGl Math Library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

//class for storing OpenGL Shader Data
class Shader

{
public:
	
	//Shader Program ID
	unsigned int ID;
	
	//Create Shader Program from the Vertex Shader at vertexShaderPath and the fragment Shader at fragmentShaderPath.
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
	{
		string vertexCode;
		string fragmentCode;
		ifstream vertexStream;
		ifstream fragmentStream;
		

		//make sure streams can throw exceptions
		//only on windows
#ifdef _WIN32
		vertexStream.exceptions(ifstream::failbit || ifstream::badbit);
		vertexStream.exceptions(ifstream::failbit || ifstream::badbit);
#endif
		try
		{
			cout << "reading shaders..." << endl;
			//open vertex and fragment shader code files from paths
			vertexStream.open(vertexShaderPath);
			fragmentStream.open(fragmentShaderPath);
			stringstream vShaderStringStream, fShaderStringStream;
			vShaderStringStream << vertexStream.rdbuf(); //read v and f shaders to stream buffer
			fShaderStringStream << fragmentStream.rdbuf();
			vertexStream.close();
			fragmentStream.close();
			vertexCode = vShaderStringStream.str(); //read stream buffer to string stream
			fragmentCode = fShaderStringStream.str();
		}
		catch(ifstream::failure e)
		{
			cout << "shader failed to read" << endl;
		}

	//	cout << vertexCode << endl;
		//cout << fragmentCode << endl;

		const char* vertexShaderCode = vertexCode.c_str(); //convert string stream  of shader to c style string 
		const char* fragmentShaderCode = fragmentCode.c_str();
		unsigned int vShader, fShader;

		//using the shader's strings we create and compile our shader code
		vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vertexShaderCode, NULL);
		glCompileShader(vShader);

		fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fragmentShaderCode, NULL);
		glCompileShader(fShader);

		//finally create Shader Program and Attach our two vertex and fragment Shaders
		ID = glCreateProgram();
		cout << "constructing shader program with ID " << ID << "..." << endl;
		glAttachShader(ID, vShader);
		glAttachShader(ID, fShader);
		glLinkProgram(ID);
		GLint success;
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			std::cout << "failed to compile Shader Program. Shader Program ID: " <<  ID << endl;
		}
		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}
	//Create Shader Progream with Geometry Shader
	Shader(const char* vertexShaderPath,const char* geometryShaderPath, const char* fragmentShaderPath)
	{
		string vertexCode;
		string geometryCode;
		string fragmentCode;
		ifstream vertexStream;
		ifstream fragmentStream;
		ifstream geometryStream;

		//make sure streams can throw exceptions
		//only on windows
#ifdef _WIN32
		vertexStream.exceptions(ifstream::failbit || ifstream::badbit);
		fragmentStream.exceptions(ifstream::failbit || ifstream::badbit);
		geometryStream.exceptions(ifstream::failbit || ifstream::badbit);
#endif
		try
		{
			cout << "reading shaders..." << endl;
			//open vertex and fragment shader code files from paths
			vertexStream.open(vertexShaderPath);
			fragmentStream.open(fragmentShaderPath);
			geometryStream.open(geometryShaderPath);
			stringstream vShaderStringStream, fShaderStringStream, gShaderStringStream;
			vShaderStringStream << vertexStream.rdbuf(); //read v and f shaders to stream buffer
			fShaderStringStream << fragmentStream.rdbuf();
			gShaderStringStream << geometryStream.rdbuf();
			vertexStream.close();
			fragmentStream.close();
			geometryStream.close();
			vertexCode = vShaderStringStream.str(); //read stream buffer to string stream
			fragmentCode = fShaderStringStream.str();
			geometryCode = gShaderStringStream.str();
		}
		catch (ifstream::failure e)
		{
			cout << "shader failed to read" << endl;
		}

		//cout << vertexCode << endl;
		//cout << fragmentCode << endl;
		//cout << geometryCode << endl;

		const char* vertexShaderCode = vertexCode.c_str(); //convert string stream  of shader to c style string 
		const char* fragmentShaderCode = fragmentCode.c_str();
		const char* geometryShaderCode = geometryCode.c_str();
		unsigned int vShader, fShader, gShader;

		//using the shader's strings we create and compile our shader code
		vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vertexShaderCode, NULL);
		glCompileShader(vShader);

		fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fragmentShaderCode, NULL);
		glCompileShader(fShader);

		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometryShaderCode, NULL);
		glCompileShader(gShader);

		//finally create Shader Program and Attach our two vertex and fragment Shaders
		ID = glCreateProgram();
		cout << "constructing shader program with ID " << ID << "..." << endl;
		glAttachShader(ID, vShader);
		glAttachShader(ID, fShader);
		glAttachShader(ID, gShader);
		glLinkProgram(ID);
		GLint success;
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			std::cout << "failed to compile Shader Program. Shader Program ID: " << ID << endl;
		}
		glDeleteShader(vShader);
		glDeleteShader(fShader);
		glDeleteShader(gShader);
	}
	//default Shader Constructer. Creates Empty Shader.
	Shader() = default;
	//Use this shader
	void use()
	{
		glUseProgram(ID);
	}
	//Set Bool Uniform of Shader
	void setBool(const string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	//Set Int Uniform of Shader
	void setInt(const string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	//Set Float Uniform of Shader
	void setFloat(const string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
};
#endif //end of preprocessor directive


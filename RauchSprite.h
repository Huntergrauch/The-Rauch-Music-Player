#pragma once
#include "glad/glad.h" //OpenGL Function Pointer Library
#include "stb_image.h"
#include <glm/glm.hpp> //OpenGl Math Library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shader.h"

struct SpriteVertex {
	vec3 Position;
	vec2 TextureCoords;
};

class Sprite;
class SpriteRenderer;

extern unsigned int SCR_WIDTH, SCR_HEIGHT;

class Sprite
{
public:
	int width, height, nrChannels;
	unsigned int textureID;
	int Format;
	unsigned int VAO, VBO;

	Sprite(const char* spritePath)
	{
		Load(spritePath);
	}
	Sprite() = default;
	void Load(const char* spritePath)
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(spritePath, &width, &height, &nrChannels, 0); //use stbi to load image
		glGenTextures(1, &textureID); //generate texture object
		glBindTexture(GL_TEXTURE_2D, textureID);
		if (nrChannels == 3)
		{
			Format = GL_RGB;
		}
		else if (nrChannels == 4)
		{
			Format = GL_RGBA;
		}
		else if (nrChannels != 3 && nrChannels != 4)
		{
			std::cout << "image format not recognized" << endl;
		}
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, Format, width, height, 0, Format, GL_UNSIGNED_BYTE, data);   //generate texture from image on bound texture object
			glGenerateMipmap(GL_TEXTURE_2D); //generate mipmap for texture
		}
		else
		{
			std::cout << "STBI failed to load image at:" << *spritePath << endl;
		}
		stbi_image_free(data); //free data for image created with stbi
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Delete()
	{
		glDeleteTextures(1, &textureID);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	void SetUpSprite()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertex) * 6, NULL, GL_DYNAMIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);
		// texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)offsetof(SpriteVertex, TextureCoords));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

class SpriteRenderer
{
public:
	Sprite RenderSprite;
	float xPos, yPos;
	float Scale;

	SpriteRenderer(Sprite sprite)
	{
		SwitchSprite(sprite);
	}
	SpriteRenderer() = default;

	void SwitchSprite(Sprite sprite)
	{
		//RenderSprite.Delete();
		RenderSprite = sprite;
		RenderSprite.SetUpSprite();
	}

	void Draw(Shader shader)
	{
		shader.use();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RenderSprite.textureID);
		glBindVertexArray(RenderSprite.VAO);

		float xpos = xPos;
		float ypos = yPos;

		float w = (RenderSprite.width * Scale) / (float)SCR_WIDTH;
		float h = (RenderSprite.height * Scale) / (float)SCR_HEIGHT;

		SpriteVertex vertices[6] = {
				{ vec3(xpos,     ypos + h, 0.0f),   vec2(0.0f, 0.0f) },
				{ vec3(xpos,     ypos, 0.0f),      vec2(0.0f, 1.0f) },
				{ vec3(xpos + w, ypos,0.0f),  vec2(1.0f, 1.0f) },

				{ vec3(xpos,     ypos + h, 0.0f),  vec2(0.0f, 0.0f) },
				{ vec3(xpos + w, ypos,0.0f),     vec2(1.0f, 1.0f) },
				{ vec3(xpos + w, ypos + h, 0.0f),  vec2(1.0f, 0.0f) }
		};
		glBindTexture(GL_TEXTURE_2D, RenderSprite.textureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, RenderSprite.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);
	}
};
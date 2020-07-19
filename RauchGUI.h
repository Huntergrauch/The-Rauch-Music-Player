#pragma once
#include "ft2build.h" //font library
#include FT_FREETYPE_H
#include <glm/glm.hpp> //OpenGl Math Library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "glad/glad.h" //OpenGL Function Pointer Library
#include <iostream>
#include "Shader.h"
#include <vector>

using namespace glm;
using namespace std;

struct Character {

	unsigned int TextureID;
	ivec2 Size;
	ivec2 Bearing;
	unsigned int Advance;
};

class Font; //Text Forward Declaration

struct FontManager
{

    vector<Font*> Fonts;
    
    void AddFont(Font* font)
    {
        Fonts.push_back(font);
    }
};

extern FontManager fontmanager;

class Font {

public:

	std::map<char, Character> Characters;
	
    Font(const char* fontPath)
    {
        Load(fontPath);
    }

	void Load(const char* fontPath)
	{
        cout << "FreeType loading Font..." << endl;
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        FT_Face face;
        if (FT_New_Face(ft, fontPath, 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        for (unsigned char c = 0; c < 128; c++)
        {
            // load character glyph 
            if (int err = FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYPE: Failed to load Glyph"  << err << std::endl;
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        cout << "Font Loaded" << endl;
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        fontmanager.AddFont(this);
	}

    void DeleteFont()
    {
        for (int i = 0; i < Characters.size();i++)
        {
            glDeleteTextures(1,&Characters.at(i).TextureID);
        }
    }
};

// Structure for storing Vertex Data for Text 
struct TextVertex {
    vec3 Position;
    vec2 TextureCoords;
};

class Text; //Text Forward Declaration

struct TextManager
{
    vector<Text*> Texts;

    void AddText(Text* text)
    {
        Texts.push_back(text);
    }
};

extern TextManager textmanager;
extern unsigned int SCR_WIDTH, SCR_HEIGHT;
class Text
{
public:
    Font* TextFont;
    string String;
    vec3 Color;

    unsigned int VAO, VBO;
    
    //Text constructor creates a line of text at the target gameobject's position.
    Text(string textstring, Font* font, vec3 color)
    {
        String = textstring; TextFont = font; Color = color;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * 6, NULL, GL_DYNAMIC_DRAW);
        
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0);
        // texture coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, TextureCoords));
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        textmanager.AddText(this);
    }

    

    void Draw(Shader* shader, float TextX, float TextY, float Scale)
    {
        // activate corresponding render state
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader->use();
        //ActiveCamera.SetCameraUniforms(*shader, SCR_WIDTH, SCR_HEIGHT);
        //shader->setMat4("model", TargetObject->Model);
        shader->setBool("ThreeDtext", false);
        glUniform3f(glGetUniformLocation(shader->ID, "textColor"), Color.x, Color.y, Color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        //float TextX = TargetObject->Position.x;
        //float TextY = TargetObject->Position.y;

        // iterate through all characters
        std::string::const_iterator c;
        for (c = String.begin(); c != String.end(); c++)
        {
            Character ch = TextFont->Characters[*c];

            float xpos = TextX + ch.Bearing.x * (Scale / SCR_WIDTH);
            float ypos = TextY - (ch.Size.y - ch.Bearing.y) * (Scale / SCR_HEIGHT);

            //cout << xpos << endl;

            float w = ch.Size.x * (Scale / SCR_WIDTH);
            float h = ch.Size.y * (Scale / SCR_HEIGHT);
            // update VBO for each character
            TextVertex vertices[6] = {
                { vec3(xpos,     ypos + h, 0.0f),   vec2(0.0f, 0.0f) },
                { vec3(xpos,     ypos, 0.0f),      vec2(0.0f, 1.0f) },
                { vec3(xpos + w, ypos,0.0f ),  vec2(1.0f, 1.0f) },

                { vec3(xpos,     ypos + h, 0.0f),  vec2( 0.0f, 0.0f) },
                { vec3(xpos + w, ypos,0.0f),     vec2(1.0f, 1.0f) },
                { vec3(xpos + w, ypos + h, 0.0f),  vec2(1.0f, 0.0f) }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            TextX += (ch.Advance >> 6) * (Scale / SCR_WIDTH); // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
    }
};


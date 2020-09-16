//This file was written by Hunter Rauch for rudimentary GUI with OpenGL
//Uses glfw, glm, glad, and freetype libraries
//Also Uses Shader.h file

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

//struct for storing information about a font's letters
struct Character {

	unsigned int TextureID;
	ivec2 Size;
	ivec2 Bearing;
	unsigned int Advance;
};

//struct for storing information about a font
class Font {

public:

	std::map<char, Character> Characters;
    unsigned int PxHeight;
	
    //Creates Font object from font file at path fontPath
    Font(const char* fontPath)
    {
        Load(fontPath);
    }
    //load a font from file
	void Load(const char* fontPath)
	{
        FT_Library ft;
        if (int err = FT_Init_FreeType(&ft))
            std::cout << "Could not init FreeType Library " << FT_Error_String(err) << std::endl;

        FT_Face face;
        if (int err = FT_New_Face(ft, fontPath, 0, &face))
            std::cout << "Freetype Failed to load font at path: " << fontPath << ". FT error: " << FT_Error_String(err) << std::endl;

        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        vector<unsigned int> heights;
        for (unsigned char c = 0; c < 128; c++)
        {
            // load character glyph 
            if (int err = FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "Freetype Failed to load Glyph "  << FT_Error_String(err) << std::endl;
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
            heights.push_back(face->glyph->bitmap.rows);

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
        PxHeight = *max_element(heights.begin(),heights.end());
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
	}
    //unallocate gpu memory associated with font's textures
    void Delete()
    {
        for( auto const& [key, val] : Characters )
        {
            glDeleteTextures(1, &val.TextureID);
        }
    }
};

// Structure for storing Vertex Data for Text 
struct TextVertex {
    vec3 Position;
    vec2 TextureCoords;
};

extern unsigned int SCR_WIDTH, SCR_HEIGHT;

//class for drawing text on the screen using the Font class
//(Text is scaled according to screen to not stretch font)
class Text
{
public:
    Font* TextFont;
    string String;
    vec3 Color;
    float Scale;

    unsigned int VAO, VBO;
    
    //Text constructor creates a line of text at the target gameobject's position.
    Text(string textstring, Font* font, vec3 color, float scale)
    {
        String = textstring; TextFont = font; Color = color; Scale = scale;
        
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
    }
    Text() = default;
    
    void Draw(Shader shader, float TextX, float TextY)
    {
        // activate corresponding render state
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader.use();
        //ActiveCamera.SetCameraUniforms(*shader, TEXTSCALE, TEXTSCALE);
        //shader->setMat4("model", TargetObject->Model);
        shader.setBool("ThreeDtext", false);
        glUniform3f(glGetUniformLocation(shader.ID, "textColor"), Color.x, Color.y, Color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        //float TextX = TargetObject->Position.x;
        //float TextY = TargetObject->Position.y;

        // iterate through all characters
        std::string::const_iterator c;
        for (c = String.begin(); c != String.end(); c++)
        {
            Character ch = TextFont->Characters[*c];

            float xpos = TextX + ch.Bearing.x * (Scale / std::max(SCR_WIDTH, (unsigned)1));
            float ypos = TextY - (ch.Size.y - ch.Bearing.y) * (Scale / std::max(SCR_HEIGHT, (unsigned)1));

            //cout << xpos << endl;

            float w = ch.Size.x * (Scale / std::max(SCR_WIDTH, (unsigned)1));
            float h = ch.Size.y * (Scale / std::max(SCR_HEIGHT, (unsigned)1));
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
            TextX += (ch.Advance >> 6) * (Scale / std::max(SCR_WIDTH, (unsigned)1)); // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
    }
    //get the width of the text onscreen in NDC units
    float GetWidth()
    {
        float StringWidth = 0.0f;
        std::string::const_iterator c;
        for (c = String.begin(); c != String.end(); c++)
        {
            Character ch = TextFont->Characters[*c];
            float w = ch.Size.x * (Scale / std::max(SCR_WIDTH,(unsigned)1));
            float space = (ch.Advance >> 6) * (Scale / std::max(SCR_WIDTH, (unsigned)1));
            StringWidth += space;
        }
        return StringWidth;
    }
    //get the height of the text onscreen in NDC units
    float GetHeight()
    {
        float StringHeight = 0.0f;
        std::string::const_iterator c;
        vector<float> charHeights;
        for (c = String.begin(); c != String.end(); c++)
        {
            Character ch = TextFont->Characters[*c];
            float h = ch.Size.y * (Scale / SCR_HEIGHT);
            charHeights.push_back(h);
        }
        if(charHeights.size() > 0)
        StringHeight = *max_element(begin(charHeights),end(charHeights));
        
        return StringHeight;
    }
    //get max height of text in NDC units
    float GetConstHeight()
    {
        return TextFont->PxHeight * (Scale / SCR_HEIGHT);
    }
    //unallocates GPU memory of Text
    void Delete()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

//class for drawing a solid color rectangle on the screen
//(SolidRectangle is NOT scaled according to the size of the screen like Text)
class SolidRectangle {

    unsigned int VAO, VBO;
public:
    void SetUpRect()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 6, NULL, GL_DYNAMIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void Draw(Shader shader, float x, float y, float width, float height, vec3 color)
    {
        glBindVertexArray(VAO);
        shader.use();
        shader.setVec3("rectColor", color);
        //SpriteShader.setMat4("projection", glm::perspective(45.0f, (float)TEXTSCALE / (float)TEXTSCALE, 0.1f, 150.0f));
        float xpos = x;
        float ypos = y;

        float w = width;
        float h = height;

        vec3 vertices[6] = {
                vec3(xpos,     ypos + h, 0.0f),
                vec3(xpos,     ypos, 0.0f),
                vec3(xpos + w, ypos,0.0f),

                vec3(xpos,     ypos + h, 0.0f),
                vec3(xpos + w, ypos,0.0f),
                vec3(xpos + w, ypos + h, 0.0f)
        };
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
    }
    //Deletes GPU memory of SolidRectangle
    void Delete()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

extern float MousePosX, MousePosY;

//struct for storing information about the mouses current location
struct Mouse_Status
{
    int Left;
    int Right;
    int Middle;
};

Mouse_Status Mouse_Input_Status;

//struct for detecting mouse input on a certain rectangle on the screen
struct RectButton
{
    float Width = 0, Height = 0;
    float xPos = 0, yPos = 0;
    bool PRESSED = false;
    bool Toggle = false;

    //creates RectButton object to check for clicks in a rectangle at parameters x and y,
    //and with a width of parameters width and height
    RectButton(float x, float y, float width, float height) {
        xPos = x; yPos = y;
        Width = width; Height = height;
    }
    //default RectButton constructor. Creates empty object.
    RectButton() = default;
    bool CheckButton(float x, float y)
    {
        float xmin = xPos;
        float ymin = yPos;
        float xmax = xPos + Width;
        float ymax = yPos + Height;

        bool inxrange = ((x >= xmin) && (x <= xmax));
        bool inyrange = ((y >= ymin) && (y <= ymax));

        if (inxrange && inyrange)
        {
            PRESSED = true;
            return true;
        }
        return false;
    }
    //Returns true if the left mouse is currently pressed and the cursor is in the rectangle
    //defined by xPos, yPos, width, and height. Otherwise returns false.
    bool CheckLeftMouse()
    {
        if(!Toggle)
        Reset();
        
        if (Mouse_Input_Status.Left == GLFW_PRESS)
        {
            return CheckButton(MousePosX, MousePosY);
        }
        return false;
    }
    //Returns true if the right mouse is currently pressed and the cursor is in the rectangle
    //defined by xPos, yPos, width, and height. Otherwise returns false.
    bool CheckRightMouse()
    {
        if (!Toggle)
            Reset();

        if (Mouse_Input_Status.Right == GLFW_PRESS)
        {
            return CheckButton(MousePosX, MousePosY);
        }
        return false;
    }
    //resets the buttons PRESSED property. Call every frame for one press buttons,
    //call only once at click for toggleable button.
    void Reset()
    {
        PRESSED = false;
    }
};

class InputTextBox;
InputTextBox* ActiveInputTextBox;

//Class for drawing a box on the screen to display text, that when active, will be changed by keyboard input.
class InputTextBox
{
public:
    Text InputText;
    SolidRectangle InputBox;
    RectButton Button;
    vec3 BoxColor = vec3(1.0f);
    bool Active = false;
    float xPos = 0, yPos = 0;
    float Width = 0, Height = 0;
    
    //Creates Box InputTextBox with specified values.
    InputTextBox(Font* font, vec3 textcolor, vec3 boxcolor, 
        float scale, float x, float y, float width)
    {
        InputText = Text("", font, textcolor, scale);
        InputBox.SetUpRect();
        BoxColor = boxcolor;
        xPos = x;
        yPos = y;
        Width = width;
        Height = 0.06f * InputText.Scale;
        Button = RectButton(x, y, width, Height);
    }
    //default InputTextBox constructor. Creates empty InputTextBox object.
    InputTextBox() = default;
    //Draws InputTextBox using specified Shaders
    void Draw(Shader textshader, Shader boxshader)
    {
        string fullstring = InputText.String;

        while (Width < InputText.GetWidth())
        {
            InputText.String.erase(0,1);
        }
        

        if (Active)InputBox.Draw(boxshader, xPos - 0.008f, yPos - 0.008f, Width + 0.005f, InputText.GetConstHeight() + 0.01f, vec3(1.0f) - BoxColor);
        InputBox.Draw(boxshader, xPos - 0.005f, yPos - 0.005f, Width, InputText.GetConstHeight(), BoxColor);
        InputText.Draw(textshader, xPos, yPos);

        InputText.String = fullstring;
    }
    //Sets this InputTextBox as the active InputTextBox, there can only be one at a time.
    //ActiveInputTextBox starts as nullptr.
    void MakeActive()
    {
        if(ActiveInputTextBox != nullptr)
        ActiveInputTextBox->Active = false;
        ActiveInputTextBox = this;
        Active = true;
    }
    //this checks if the InputTextBox is clicked on using the RectButton struct
    //if it is clicked on this function calls MakeActive().
    void Update()
    {
        Button.CheckLeftMouse();
        if (Button.PRESSED)
        {
            MakeActive();
        }
    }
    //unallocates GPU memory associated with InputTextBox
    void Delete()
    {
        InputText.Delete();
        InputBox.Delete();
    }
};

//class for Drawing a button on screen that can be clicked on and has text on it
class TextButton
{
    SolidRectangle Rect;
    vec3 Color;
    float xPos, yPos;
    float Width, Height;
public:
    RectButton Button;
    Text ButtonText;

    //Creates a TextButton object with specified parameters.
    TextButton(string buttontext, vec3 textcolor, Font* textfont, float textscale,
        vec3 buttoncolor,float x, float y, float width, float height)
    {
        Button = RectButton(x, y, width, height);
        ButtonText = Text(buttontext, textfont, textcolor, textscale);
        Color = buttoncolor;
        Rect.SetUpRect();
        xPos = x;
        yPos = y;
        Width = width;
        Height = height;
    }
    //Draws TextButton with specified shader.
    void Draw(Shader rectshader, Shader textshader)
    {
        string fullstring = ButtonText.String;

        while (Width < ButtonText.GetWidth())
        {
           ButtonText.String.erase(0, 1);
        }
        
        if (Button.PRESSED)Rect.Draw(rectshader, xPos - 0.005f, yPos - 0.005f, Width + 0.01f, Height + 0.01f, vec3(1.0f) - Color);
        Rect.Draw(rectshader,xPos,yPos,Width,Height, Color);
        ButtonText.Draw(textshader, xPos + (Width / 2) - (ButtonText.GetWidth() / 2), yPos + (Height / 2) - (ButtonText.GetConstHeight() / 2));

        ButtonText.String = fullstring;
    }
    //unallocates GPU memory associated with TextButton
    void Delete()
    {
        Rect.Delete();
        ButtonText.Delete();
    }
};

//Struct for drawing rows of a table that are clickable and can be split into different parts each with its own Text.
struct TextTableRow 
{
    SolidRectangle Rect;
    RectButton Button;
    vec3 Color;
    unsigned int Segments = 0;
    vector<Text> Texts;
    float xPos, yPos;
    float Width, Height;
    Font* TextFont;

    //Creates TextTable row with specified parameters.
    TextTableRow(Font* font, float x, float y, float width, float height, vec3 rectcolor = vec3(1.0f,1.0f,1.0f))
    {
        xPos = x;
        yPos = y;
        Width = width;
        Height = height;
        Color = rectcolor;
        Rect.SetUpRect();
        TextFont = font;
        Button = RectButton(xPos, yPos, Width, Height);
    }
    //Adds a Segment to the TextTableRow.
    void AddTextSegment(string textstring, vec3 textcolor, float scale)
    {
        Segments++;
        Texts.push_back(Text(textstring, TextFont, textcolor, scale));
    }
    //Draws the TextTableRow using the specified parameters.
    int Draw(Shader rectshader, Shader textshader)
    {
        if (Button.PRESSED)Rect.Draw(rectshader, xPos - 0.005f, yPos - 0.005f, Width + 0.01f, Height + 0.01f, vec3(1.0f,1.0f,1.0f) - Color);
        Rect.Draw(rectshader, xPos, yPos, Width, Height, Color);
        vector<float> SegmentPoses;
        if (Segments == 0)return -1;//stop divide by zero
        for (int i = 0; i < Segments; i++)
        {
            SegmentPoses.push_back(0.005f + xPos + (Width / (float)Segments) * i);
        }
        
        for (int i = 0; i < Texts.size();i++)
        {
            string fullstring = Texts[i].String;

            while (Texts[i].GetWidth() > (Width / Segments))
            {
                Texts[i].String.erase(0, 1);
            }
            
            Texts[i].Draw(textshader, SegmentPoses[i],0.005f + yPos);
            Texts[i].String = fullstring;
        }
        return 0;
    }
    //unallocate GPU memory associated with TextTableRow
    void Delete()
    {
        Rect.Delete();
        for (int i = 0; i < Texts.size();i++)
        {
            Texts[i].Delete();
        }
    }

};

unsigned int BackspaceWaitTime = 10;
extern InputTextBox* ActiveInputTextBox;
unsigned int ITB_BackspaceWait = 0;
bool backspacestatus = false;
//Process input for the InputTextBox
void process_GUI_input(GLFWwindow* window)
{
    bool lastbackspacestatus= backspacestatus;
    backspacestatus = glfwGetKey(window, GLFW_KEY_BACKSPACE);
    
    //backspace ability on input boxes
    if (backspacestatus && !lastbackspacestatus && !ActiveInputTextBox->InputText.String.empty())
    {
        ActiveInputTextBox->InputText.String.pop_back();
        ITB_BackspaceWait = 1;
    }
    else if (backspacestatus && lastbackspacestatus)
    {
        if (ITB_BackspaceWait == 0 && !ActiveInputTextBox->InputText.String.empty())
        {
            ActiveInputTextBox->InputText.String.pop_back();
            ITB_BackspaceWait = 1;
        }
        if (ITB_BackspaceWait > 0)
        {
            ITB_BackspaceWait++;
        }
        if (ITB_BackspaceWait > BackspaceWaitTime)
        {
            ITB_BackspaceWait = 0;
        }
    }

    
}

 //character callback for input text boxes
 //use with GLFW function glfwSetCharCallback()
void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    if (ActiveInputTextBox != nullptr)
    {
        string codestring;
        codestring = codepoint;
        ActiveInputTextBox->InputText.String += codestring;
    }
}

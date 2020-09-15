#pragma once
#include "RauchAudio.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include "RauchGUI.h"
#include <glm/glm.hpp> //OpenGl Math Library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <portable-file-dialogs.h>
#include <fstream>
#include "RauchSprite.h"
#include <filesystem>

namespace fs = std::filesystem;

//forward declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void UpdateTime();
void process_input(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);

double YScrollOffset = 0;

unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 1024;

float currentTime = 0.0f;
float lastTime = 0.0f;
float deltaTime = 0.0f;

bool browse = false;
bool PressedPause = false;
bool PressedLoop = false;
bool PressedShuffle = false;


string RESOURCES_PATH = "./rmpResources";
//if building to install on linux, you must use an absolute path.
#ifdef LINUX_INSTALL

RESOURCES_PATH = "/usr/local/share/rmpResources";

#endif

//struct for storing info about individual songs
struct Song
{
	string Path;
	string Title;
	string Artist;
	string Album;

	Song(string path, string title = "/UNKNOWN_TITLE/", string artist = "/UNKNOWN_ARTIST/", string album = "/UNKNOWN_ALBUM/")
	{
		Path = path;
		Title = title;
		Artist = artist;
		Album = album;
	}
	Song() = default;
};

//struct for storing info about multiple songs and their file paths as a file.
struct Playlist
{
	vector<Song> Songs;
	string path;
	//bracket operator
	Song operator [](int i) const { return Songs[i]; }
	Song& operator [](int i) { return Songs[i]; }
	int scrollnum = 0;

	//load a playlist file by path
	int Load(const char* path)
	{
		string stringpath(path);
		std::ifstream file(stringpath);

		if (file.is_open())
		{
			std::string line;
			while (std::getline(file, line)) {

				int pathbegnum = (line.find("<path>") + 6);
				int pathendnum = (line.find("</path>"));

				int titlebegnum = (line.find("<title>") + 7);
				int titleendnum = (line.find("</title>"));

				int artistbegnum = (line.find("<artist>") + 8);
				int artistendnum = (line.find("</artist>"));

				int albumbegnum = (line.find("<album>") + 7);
				int albumendnum = (line.find("</album>"));

				string foundpath;
				string foundtitle;
				string foundartist;
				string foundalbum;

				foundpath = line.substr(pathbegnum, pathendnum - pathbegnum);
				foundtitle = line.substr(titlebegnum, titleendnum - titlebegnum);
				foundartist = line.substr(artistbegnum, artistendnum - artistbegnum);
				foundalbum = line.substr(albumbegnum, albumendnum - albumbegnum);

				AddSong(Song(foundpath, foundtitle, foundartist, foundalbum));
			}
			file.close();
			return 0;
		}
		else
		{
			cout << "cout not load library at path: " << stringpath << endl;
			return -1;
		}
	}
	//save playlist at path
	int Save(const char* path)
	{
		string pathstring(path);
		ofstream myfile(pathstring);
		if (myfile.is_open())
		{
			
			for (int i = 0; i < Songs.size();i++)
			{
				myfile << "<path>" << Songs[i].Path << "</path>";
				myfile << "<title>" << Songs[i].Title << "</title>";
				myfile << "<artist>" << Songs[i].Artist << "</artist>";
				myfile << "<album>" << Songs[i].Album << "</album>" << endl;
			}

			myfile.close();
			return 0;
		}
		else
		{
			cout << "Unable to save file at path: " << path << endl;
			return -1;
		}
	}
	//add song to playlist
	int AddSong(Song song)
	{
		for (int i = 0; i < Songs.size();i++)
		{
			if (song.Path == Songs[i].Path)
			{
				return -1; //Song already in playlist
			}
		}
		Songs.push_back(song);
		return 0;
	}
};

//sorting function
bool CompareTitles(Song a, Song b)
{
	return (a.Title < b.Title);
}
//sorting function
bool CompareArtists(Song a, Song b)
{
	return (a.Artist < b.Artist);
}
//sorting function
bool CompareAlbums(Song a, Song b)
{
	return (a.Album < b.Album);
}

vec3 ThemeColor = vec3(0.25f, 0.875f, 0.8125f);
//function to read settings file
void ReadSettings()
{
	std::ifstream file(RESOURCES_PATH + "./Settings.txt");

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			int themecolorR = (line.find("R=") + 2);
			int themecolorG = (line.find("G=") + 2);
			int themecolorB = (line.find("B=") + 2);

			string themecolorRstring, themecolorGstring, themecolorBstring, backspacespeedstring;

			if (themecolorR != -1 + 2)
			{
				themecolorRstring = line.substr(themecolorR, line.size() - 1);
				cout << "R: " << themecolorRstring << endl;
				ThemeColor.r = std::stof(themecolorRstring);
			}
			else if (themecolorG != -1 + 2)
			{
				themecolorGstring = line.substr(themecolorG, line.size() - 1);
				cout << "G: " << themecolorGstring << endl;
				ThemeColor.g = std::stof(themecolorGstring);
			}
			else if (themecolorB != -1 + 2)
			{
				themecolorBstring = line.substr(themecolorB, line.size() - 1);
				cout << "B: " << themecolorBstring << endl;
				ThemeColor.b = std::stof(themecolorBstring);
			}
		}
	}
}

int main(int argc, char** argv)
{
	
	//GLFW initializtion
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);


	//GLFW Window Creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rauch Music Player", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	GLFWimage images[1];
	int width, height;
	string windowiconpath = RESOURCES_PATH + "/Textures/Logo.png";
	images[0].pixels = stbi_load(windowiconpath.c_str(), &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(window, 1, images);


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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMaximizeWindow(window);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); //Set OpenGL Viewport Size

	//rendering settings

	//Enable Blending
	glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_SCISSOR_TEST);
	
	string TextShaderVertPath = RESOURCES_PATH + "/Shaders/Text.vert", TextShaderFragPath = RESOURCES_PATH + "/Shaders/Text.frag";
	string RectShaderVertPath = RESOURCES_PATH + "/Shaders/Rect.vert", RectShaderFragPath = RESOURCES_PATH + "/Shaders/Rect.frag";
	string SpriteShaderVertPath = RESOURCES_PATH + "/Shaders/Sprite.vert", SpriteShaderFragPath = RESOURCES_PATH + "/Shaders/Sprite.frag";
	string ControlShaderVertPath = RESOURCES_PATH + "/Shaders/controls.vert", ControlShaderFragPath = RESOURCES_PATH + "/Shaders/controls.frag";

	Shader TextShader(TextShaderVertPath.c_str(), TextShaderFragPath.c_str());
	Shader RectShader(RectShaderVertPath.c_str(), RectShaderFragPath.c_str());
	Shader SpriteShader(SpriteShaderVertPath.c_str(), SpriteShaderFragPath.c_str());
	Shader ControlShader(ControlShaderVertPath.c_str(), ControlShaderFragPath.c_str());

	ReadSettings();

	string fontpath = RESOURCES_PATH + "/Fonts/NotoSans-Regular.ttf";
	Font font(fontpath.c_str());
	InputTextBox inputtext(&font, vec3(0.0f, 0.0f, 0.0f),vec3(1.0f,1.0f,1.0f), 1.5f, -0.99f, 0.6f, 0.75f);
	TextButton addbutton("Add File", vec3(0.0f), &font, 1.0f, ThemeColor * vec3(0.8f),
		-0.24f, 0.6f, 0.24f, 0.09f);
	TextButton browsebutton("Browse for file(s)", vec3(0.0f), &font, 0.75f, ThemeColor * vec3(0.8f),
		0.01f, 0.6f, 0.24f, 0.09f);;

	SolidRectangle rect; 
	SolidRectangle titlerect;
	rect.SetUpRect();
	titlerect.SetUpRect();

	Playlist library;
	string librarypath = RESOURCES_PATH + "/Playlists/rootlib.rauchplaylist";
	int libfound = library.Load(librarypath.c_str());

	if (libfound != 0)
	{
		string playlistpath = RESOURCES_PATH + "/Playlists";
		fs::create_directories(playlistpath);
	}

	TextTableRow tablelegend(&font, -0.95f, 0.5f, 1.9f, 0.09f, ThemeColor * vec3(0.8f));
	tablelegend.AddTextSegment("Title/Path", vec3(0.0f, 0.0f, 0.0f), 0.6f);
	tablelegend.AddTextSegment("Album", vec3(0.0f, 0.0f, 0.0f), 0.6f);
	tablelegend.AddTextSegment("Artist", vec3(0.0f, 0.0f, 0.0f), 0.6f);
	
	vector<TextTableRow> entrytable;
	bool needupdatelib = true;
	InitializeAudio();
	int sortmode = 0;
	bool sorted = false;

	Song ActiveSong;
	//parse arguments
	if (argc > 1)
	{
		if (argc > 3)
		{
			cout << "can't play more than one song, only playing first song" << endl;
		}
		Audio playaudio;
		if (playaudio.Load(argv[2]) != 0)
		{
			Song playSong;
			playSong.Path = argv[2];

			StartAudio(playaudio);
			ActiveSong = playSong;
		}
		else
		{
			cout << "can't play song at path: " << argv[2] << endl;
		}
	}
	
	unsigned int ActiveSongIndex = NULL;
	ActiveSong.Path = "NO_ACTIVE_SONG";
	Text ActiveSongPath("", &font, vec3(0.0f, 0.0f, 0.0f), 0.6f);
	Text ActiveSongTitle("", &font, vec3(0.0f, 0.0f, 0.0f), 0.75f);
	SolidRectangle SongStatusRect;
	SongStatusRect.SetUpRect();
	float SongStatus = 0.0f;

	string PausedSpritePath = RESOURCES_PATH + "/Textures/Play.png";
	string PauseSpritePath = RESOURCES_PATH + "/Textures/Pause.png";
	string LoopingSpritePath = RESOURCES_PATH + "/Textures/Loop.png";
	string ShuffleSpritePath = RESOURCES_PATH + "/Textures/Shuffle.png";
	Sprite PausedSprite(PausedSpritePath.c_str());
	Sprite PauseSprite(PauseSpritePath.c_str());
	Sprite LoopingSprite(LoopingSpritePath.c_str());
	Sprite ShuffleSprite(ShuffleSpritePath.c_str());
	SpriteRenderer PauseRenderer(PauseSprite);
	SpriteRenderer LoopingRenderer(LoopingSprite);
	SpriteRenderer ShuffleRenderer(ShuffleSprite);

	bool Shuffle = false;
	bool Looping = false;

	int menu_grace = 0;

	bool pauseclick = false;
	bool shuffleclick = false;
	bool loopclick = false;

	while (!glfwWindowShouldClose(window))
	{
		UpdateTime();
		process_input(window);
		process_GUI_input(window);
		
		bool lastpauseclick = pauseclick;
		pauseclick = PauseRenderer.RenderSprite.IsSpriteClicked(Mouse_Input_Status.Left == GLFW_PRESS, MousePosX, MousePosY, -0.5f, 0.85f, 0.25f);

		bool lastshuffleclick = shuffleclick;
		shuffleclick = ShuffleRenderer.RenderSprite.IsSpriteClicked(Mouse_Input_Status.Left == GLFW_PRESS, MousePosX, MousePosY, -0.4f, 0.85f, 0.25f);
		
		bool lastloopclick = loopclick;
		loopclick = ShuffleRenderer.RenderSprite.IsSpriteClicked(Mouse_Input_Status.Left == GLFW_PRESS, MousePosX, MousePosY, -0.45f, 0.85f, 0.25f);
		
		if (PressedPause == true || (pauseclick && !lastpauseclick))
		{
			bool paused = false;
			if (ActiveAudio.Samples.size() > 0)
			{
				if (ActiveAudio.Paused == false)
				{
					ActiveAudio.Paused = true;
					PauseRenderer.SwitchSprite(PausedSprite);
				}
				else if (ActiveAudio.Paused == true)
				{
					ActiveAudio.Paused = false;
					paused = true;
					PauseRenderer.SwitchSprite(PauseSprite);
				}
			}
			string prefix;
			if (paused)prefix = "un";
			cout << prefix << "paused" << endl;
			PressedPause = false;
		}
		if (((shuffleclick && !lastshuffleclick) || PressedShuffle) && !Shuffle)
		{
			Shuffle = true;
			PressedShuffle = false;
		}
		else if (((shuffleclick && !lastshuffleclick) || PressedShuffle) && Shuffle)
		{
			Shuffle = false;
			PressedShuffle = false;
		}
		if (((loopclick && !lastloopclick) || PressedLoop) && !Looping)
		{
			Looping = true;
			PressedLoop = false;
		}
		else if (((loopclick && !lastloopclick) || PressedLoop) && Looping)
		{
			Looping = false;
			PressedLoop = false;
		}
		
		if(ActiveAudio.Samples.size() > 0)
		{
			if (ActiveAudio.Completed)
			{
				unsigned int nextsongindex = ActiveSongIndex + 1;
				if (nextsongindex == library.Songs.size())
				{
					nextsongindex = 0;
				}
				if (Shuffle)
				{
					nextsongindex = rand() % (library.Songs.size() - 1);
				}
				
				Audio newaudio;
				if (newaudio.Load(library[nextsongindex].Path.c_str()) == 0)
				{
				StartAudio(newaudio);
				ActiveSong = library[nextsongindex];
				ActiveSongIndex = nextsongindex;
				}
				else
				{
					cout << "failed to load song at path: " << library[nextsongindex].Path.c_str();
				}
			}
		}

		vec3 ShuffleColor = vec3(0.5f) * ThemeColor;
		if (Shuffle)
		{
			ShuffleColor = vec3(0.25f) * ThemeColor;
		}
		vec3 LoopingColor = vec3(0.5f) * ThemeColor;
		if (Looping)
		{
			if (ActiveAudio.Looping == false)
			{
				ActiveAudio.Looping = true;
			}
			LoopingColor = vec3(0.25f) * ThemeColor;
		}
		else
		{
			if (ActiveAudio.Looping == true)
			{
				ActiveAudio.Looping = false;
			}
		}

		if (ActiveSong.Path != "NO_ACTIVE_SONG")
		{
			ActiveSongPath.String = ActiveSong.Path;
			if (ActiveSong.Title != "/UNKNOWN_TITLE/")
			{
				ActiveSongTitle.String = ActiveSong.Title;

				if (ActiveSong.Album != "/UNKNOWN_ALBUM/")
				{
					ActiveSongTitle.String = ActiveSong.Album +  " - " + ActiveSongTitle.String;
				}
				if (ActiveSong.Artist != "/UNKNOWN_ARTIST/")
				{
					ActiveSongTitle.String = ActiveSong.Artist + " - " + ActiveSongTitle.String;
				}
			}
			else
			{
				ActiveSongTitle.String = "";
			}
		}
		else
		{
			ActiveSongPath.String = "No Active Song";
			ActiveSongTitle.String = "";
		}
		if (ActiveAudio.Samples.size() > 0)
		{
			SongStatus = ((float)ActiveAudio.currentSample / (float)ActiveAudio.SamplesPerChannel);
		}
		float entrysize = 0.6f;
		float albumsize = 0.6f;
		float artistsize = 0.6f;
		if (sortmode == 1)entrysize = 0.75f;
		if (sortmode == 2)albumsize = 0.75f;
		if (sortmode == 0)artistsize = 0.75f;
		tablelegend.Texts[0].Scale = entrysize;
		tablelegend.Texts[1].Scale = albumsize;
		tablelegend.Texts[2].Scale = artistsize;

		if (tablelegend.Button.CheckLeftMouse() & !sorted)
		{
			if (sortmode == 0)
			{
				sort(library.Songs.begin(), library.Songs.end(), CompareTitles);
				sortmode = 1;
			}
			else if (sortmode == 1)
			{
				sort(library.Songs.begin(), library.Songs.end(), CompareAlbums);
				sortmode = 2;
			}
			else if (sortmode == 2)
			{
				sort(library.Songs.begin(), library.Songs.end(), CompareArtists);
				sortmode = 0;
			}
			needupdatelib = true;
			sorted = true;
		}
		else if(!tablelegend.Button.CheckLeftMouse() && sorted)
		{
			sorted = false;
		}

		//cout << library.scrollnum << endl;
		if (needupdatelib)
		{
			vector<TextTableRow> emptyttable;
			entrytable.swap(emptyttable);
			for (int i = 0; i < library.Songs.size();i++)
			{
				string entrystring = library[i].Path;
				if (library[i].Title != "/UNKNOWN_TITLE/")
				{
					entrystring = library[i].Title;
				}
				string artiststring = "Unknown";
				if (library[i].Artist != "/UNKNOWN_ARTIST/")
				{
					artiststring = library[i].Artist;
				}
				string albumstring = "Unknown";
				if (library[i].Album != "/UNKNOWN_ALBUM/")
				{
					albumstring = library[i].Album;
				}
				entrytable.push_back(TextTableRow(&font, -0.95f, 0.4f - (i * 0.1f) - (library.scrollnum * 0.1f), 1.9f, 0.09f, ThemeColor * vec3(0.9f)));

				entrytable[i].AddTextSegment(entrystring, vec3(0.0f, 0.0f, 0.0f), 0.6f);
				entrytable[i].AddTextSegment(albumstring, vec3(0.0f, 0.0f, 0.0f), 0.6f);
				entrytable[i].AddTextSegment(artiststring, vec3(0.0f, 0.0f, 0.0f), 0.6f);
				entrytable[i].Button.Toggle = true;
			}
			needupdatelib = false;
		}
		for (int i = 0; i < entrytable.size();i++)
		{
			entrytable[i].yPos = 0.4f - (i * 0.1f) - (library.scrollnum * 0.1f);
			entrytable[i].Button.yPos = 0.4f - (i * 0.1f) - (library.scrollnum * 0.1f);
		}
		if (entrytable.size() > 0)
		{
			if (0.4 - (YScrollOffset * 0.1f) > 0.35)
			{
				library.scrollnum = YScrollOffset;
			}
			else 
			{ 
				YScrollOffset = library.scrollnum;
			}
		}


		inputtext.Update();

		for (int i = 0; i < entrytable.size();i++)
		{
			Audio newaudio;
			if (menu_grace == 0 && entrytable[i].Button.CheckLeftMouse() && (entrytable[i].yPos < 0.5 && entrytable[i].yPos > -0.9f))
			{
				if (newaudio.Load(library[i].Path.c_str()) == 0)
				{
					StartAudio(newaudio);
					ActiveSong = library[i];
					ActiveSongIndex = i;
					for (int j = 0; j < entrytable.size();j++)
					{
						if(j != i)entrytable[j].Button.Reset();
					}
				}
			}
			if (entrytable[i].Button.CheckRightMouse() && (entrytable[i].yPos < 0.5 && entrytable[i].yPos > -0.9f))
			{
				bool done = false;
				bool cancelled = false;
				bool removed = false;
				Text path(library[i].Path, &font, vec3(0.0f, 0.0f, 0.0f), 1.2f);
				Text artist("Artist: ", &font, vec3(0.0f, 0.0f, 0.0f), 1.0f);
				Text title("Song Title: ", &font, vec3(0.0f, 0.0f, 0.0f), 1.0f);
				Text album("Album: ", &font, vec3(0.0f, 0.0f, 0.0f), 1.0f);
				TextButton DoneButton("OK", vec3(0.0f), &font, 1.0f, ThemeColor * vec3(0.8f),
					0.0f, -0.7f, 0.24f, 0.09f);
				TextButton CancelButton("Cancel", vec3(0.0f), &font, 1.0f, ThemeColor * vec3(0.8f),
					0.25f, -0.7f, 0.24f, 0.09f);
				TextButton RemoveButton("Remove Song", vec3(0.0f), &font, 1.0f, ThemeColor * vec3(0.8f),
					-0.25f, -0.7f, 0.24f, 0.09f);
				InputTextBox SongName(&font, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, -0.5f, -0.5f, 1.0f);
				InputTextBox SongArtist(&font, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, -0.5f, -0.3f, 1.0f);
				InputTextBox SongAlbum(&font, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, -0.5f, -0.1f, 1.0f);
				SongName.InputText.String = library[i].Title;
				SongArtist.InputText.String = library[i].Artist;
				SongAlbum.InputText.String = library[i].Album;
				while (!cancelled && !removed && !done && !glfwWindowShouldClose(window))
				{
					process_input(window);
					process_GUI_input(window);
					if (DoneButton.Button.CheckLeftMouse())
					{
						done = true;
					}
					if (CancelButton.Button.CheckLeftMouse())
					{
						cancelled = true;
					}
					if (RemoveButton.Button.CheckLeftMouse())
					{
						removed = true;
					}
					
					SongName.Update();
					SongArtist.Update();
					SongAlbum.Update();
					
					glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); //Set OpenGL Viewport Size
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glClearColor(ThemeColor.x, ThemeColor.y, ThemeColor.z, 1.0f); //draw blue background
					
					path.Draw(TextShader, 0.0f - (path.GetWidth() / 2.0f), 0.8f);
					artist.Draw(TextShader, 0.0f - (artist.GetWidth() / 2.0f), -0.2f);
					title.Draw(TextShader, 0.0f - (title.GetWidth() / 2.0f), -0.4f);
					album.Draw(TextShader, 0.0f - (album.GetWidth() / 2.0f), -0.0f);
					SongName.Draw(TextShader, RectShader);
					SongArtist.Draw(TextShader, RectShader);
					SongAlbum.Draw(TextShader, RectShader);
					DoneButton.Draw(RectShader, TextShader);
					RemoveButton.Draw(RectShader, TextShader);
					CancelButton.Draw(RectShader, TextShader);

					//swap buffers & check events
					glfwSwapBuffers(window);
					glfwPollEvents();
				}
				if (done)
				{
					library[i].Title = SongName.InputText.String;
					library[i].Artist = SongArtist.InputText.String;
					library[i].Album = SongAlbum.InputText.String;
				}
				else if (removed)
				{
					library.Songs.erase(library.Songs.begin() + i);
				}

				path.Delete();
				artist.Delete();
				album.Delete();
				DoneButton.Delete();
				CancelButton.Delete();
				RemoveButton.Delete();
				SongName.Delete();
				SongArtist.Delete();
				SongAlbum.Delete();

				menu_grace = 30;
				needupdatelib = true;
			}
		}

		if(menu_grace > 0)
		{ 
			menu_grace--;
		}
		
		if (addbutton.Button.CheckLeftMouse())
		{
			Audio newaudio;
			if (newaudio.Load(inputtext.InputText.String.c_str()) == 0)
			{
				library.AddSong(Song(inputtext.InputText.String));
				inputtext.InputText.String = "Audio Added";
				needupdatelib = true;
			}
			else
			{
				inputtext.InputText.String = "Submitted File Invalid";
			}
			addbutton.Button.Reset();
			library.Save(librarypath.c_str());
		}

		if (browsebutton.Button.CheckLeftMouse())
		{
			browse = true;
			browsebutton.Button.Reset();
		}

		if (browse)
		{
			vector<string> selection = pfd::open_file("Select one or more Audio files", ".",
				{ "Audio Files", "*.wav *.mp3 *.flac"}, pfd::opt::multiselect).result();
			if (selection.size() > 0)
			{
				for (int i = 0; i < selection.size();i++)
				{
					Audio newaudio;
					if (newaudio.Load(selection[i].c_str()) == 0)
					{
						library.AddSong(Song(selection[i].c_str()));
					}
					else
					{
						cout << "path invalid: " << selection[i] << endl;
					}
				}
			}
			needupdatelib = true;
			browse = false;
			library.Save(librarypath.c_str());
		}

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); //Set OpenGL Viewport Size
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //draw blue background

		rect.Draw(RectShader, -1.0f, -1.0f, 2.0f, 1.8f, ThemeColor);
		titlerect.Draw(RectShader, -1.0f, 0.8f, 2.0f, 0.2f, ThemeColor * vec3(0.8f));
		inputtext.Draw(TextShader, RectShader);
		addbutton.Draw(RectShader, TextShader);
		browsebutton.Draw(RectShader, TextShader);
		tablelegend.Draw(RectShader, TextShader);
		for (int i = 0; i < entrytable.size();i++)
		{
			if (entrytable[i].yPos < 0.48 && entrytable[i].yPos > -0.9f)
			{
				entrytable[i].Draw(RectShader, TextShader);
			}
		}
		ActiveSongPath.Draw(TextShader, -0.5f, 0.95f);
		ActiveSongTitle.Draw(TextShader, -0.5f, 0.9f);
		SongStatusRect.Draw(RectShader, -0.5f, 0.8f, 1.0f, 0.04f, ThemeColor * vec3(0.5f));
		SongStatusRect.Draw(RectShader, -0.5f, 0.8f, SongStatus, 0.04f, ThemeColor * vec3(0.9f));
		ControlShader.use();
		ControlShader.setVec3("Color", vec3(0.25f) * ThemeColor);
		PauseRenderer.Draw(ControlShader, -0.5f, 0.85f, 0.25f);
		ControlShader.use();
		ControlShader.setVec3("Color", LoopingColor);
		LoopingRenderer.Draw(ControlShader, -0.45f, 0.85f, 0.25f);
		ControlShader.use();
		ControlShader.setVec3("Color", ShuffleColor);
		ShuffleRenderer.Draw(ControlShader, -0.4f, 0.85f, 0.25f);

		//swap buffers & check events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	library.Save(librarypath.c_str());
	
	//delete all graphical objects
	PausedSprite.Delete();
	PauseSprite.Delete();
	LoopingSprite.Delete();
	ShuffleSprite.Delete();
	tablelegend.Delete();
	SongStatusRect.Delete();
	inputtext.Delete();
	ActiveSongPath.Delete();
	ActiveSongTitle.Delete();
	addbutton.Delete();
	rect.Delete();
	titlerect.Delete();
	font.Delete();
	browsebutton.Delete();
	for (int i = 0; i < entrytable.size();i++)
	{
		entrytable[i].Delete();
	}

	glfwTerminate();
	DeinitializeAudio();
}

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
float lastMouseX = (float)SCR_WIDTH / 2.0;
float lastMouseY = (float)SCR_HEIGHT / 2.0;
bool firstusedmouse = true;
float MousePosX = 0;
float MousePosY = 0;

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

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	YScrollOffset += yoffset;
	//cout << yoffset << YScrollOffset <<  endl;
}

void process_input(GLFWwindow* window)
{
	//closing window with escape
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
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

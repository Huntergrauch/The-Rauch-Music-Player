#pragma once
#include "RauchAudio.h"
//#include "GLFW/glfw3.h"

int main()
{
	string ChosenPath;
	cout << "Type the path of the MP3 or WAV file you would like to play:" << endl;
	cin >> ChosenPath;
	InitializeAudio();
	Audio testaudio(ChosenPath.c_str());
	StartAudio(testaudio);
	for (;;)
	{
		UPDATESOUND;
	}
	End();
}
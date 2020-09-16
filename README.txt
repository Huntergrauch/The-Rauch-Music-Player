The Rauch Audio Player
----------------------

A simple audio playlist displayer and player written in C++ using OpenGL for graphics and multiple other libraries

Supported File Types
----------------------

wav, mp3, and flac.

Controls
----------------------

Press TAB or click the "Browse for File(s)" button to open a OS-specific dialog to browse your systems files and select one or more files to add the the playlist.
Or click on the white input box, type the path to your chosen audio file, then click the "Add File" Button.
Press CTRL or click the looping icon to toggle looping.
Press ALT or click the shuffle icon to toggle shuffle.
Click on the header bar of the playlist to sort the playlist alphabetically by title, artist or album.
Right click on a song in the playlist to edit it's data, such as its title, album or artist, or to delete the song from the playlist.
Press the Pause key or click the Pause/Play icon to pause/unpause the currently playing song.

Libraries Used
----------------------

glfw
https://www.glfw.org/

glm
https://glm.g-truc.net/0.9.9/index.html

libsoundio
http://libsound.io/

minimp3
https://github.com/lieff/minimp3

dr flac
https://github.com/mackron/dr_libs/blob/master/dr_flac.h

portable file dialogs
https://github.com/samhocevar/portable-file-dialogs

Audiofile.h
https://github.com/adamstark/AudioFile/blob/master/AudioFile.h

Freetype
https://www.freetype.org/

glad
https://github.com/Dav1dde/glad

stb_image
https://github.com/nothings/stb/blob/master/stb_image.h
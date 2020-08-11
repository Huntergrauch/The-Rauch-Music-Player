cmake_minimum_required(VERSION 3.10)

set(WINLIBS glfw3 opengl32 freetype soundio) 
set(SRCFILES Program.cpp stb_image.cpp glad.c)

# set the project name
project(The_Rauch_Audio_Player)
message("The_Rauch_Audio_Player")
link_directories(${PROJECT_SOURCE_DIR}/Dependencies/Libs)

# add the executable
add_executable(The_Rauch_Audio_Player ${SRCFILES})

if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
    message("Building for Linux")
    find_package(Freetype REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(soundio REQUIRED)
    find_package(id3v2 REQUIRED)
    target_include_directories(The_Rauch_Audio_Player PRIVATE ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(The_Rauch_Audio_Player PRIVATE ${FREETYPE_LIBRARIES})
    target_include_directories(The_Rauch_Audio_Player PRIVATE ${GLFW3_INCLUDE_DIRS})
    target_link_libraries(The_Rauch_Audio_Player PRIVATE ${GLFW3_LIBRARIES})
    target_include_directories(The_Rauch_Audio_Player PRIVATE ${SOUNDIO_INCLUDE_DIRS})
    target_link_libraries(The_Rauch_Audio_Player PRIVATE ${SOUNDIO_LIBRARIES})
    target_link_libraries(The_Rauch_Audio_Player PRIVATE X11 dl GL)
elseif(WIN32)
    message("Building for Windows")
    target_link_libraries(The_Rauch_Audio_Player PRIVATE ${WINLIBS})
else()
    message( FATAL_ERROR "Unsupported platform." )
endif()

target_compile_definitions(The_Rauch_Audio_Player PRIVATE SOUNDIO_STATIC_LIBRARY=1)

# add the binary tree to the search path for include files
target_include_directories(The_Rauch_Audio_Player PUBLIC
                          ${PROJECT_BINARY_DIR}
                          ${PROJECT_SOURCE_DIR}/Dependencies/Include
                          )
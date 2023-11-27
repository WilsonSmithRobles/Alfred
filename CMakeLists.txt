# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Name of your project
project(Alfred)

# Output files destination
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

# Set C++ standard (e.g., C++11, C++14, C++17)
set(CMAKE_CXX_STANDARD 17)

# Set source files
file(GLOB SOURCES "./src/*.cpp")

# Specify the executable to be built and the source files to be used
add_executable(Alfred ${SOURCES})

include("./PortAudio.cmake")
target_link_libraries(Alfred PortAudio)

include("./Whisper.cmake")
target_link_libraries(Alfred whisper)

target_include_directories(Alfred 
	PRIVATE 
		$(PROJECT_SOURCE_DIR)/include
		$(PROJECT_SOURCE_DIR)/libs/portaudio
		$(PROJECT_SOURCE_DIR)/libs/whisper.cpp
)
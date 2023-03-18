cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_EXTENSIONS OFF)
set(PROJECT_NAME AUDIO_PLOT)
project(${PROJECT_NAME} VERSION 1.0 DESCRIPTION "Simple audio wave plot" HOMEPAGE_URL "https://github.com/Th3UwU/" LANGUAGES CXX)

file(GLOB_RECURSE
	SOURCES CONFIGURE_DEPENDS
	"./src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(
	${PROJECT_NAME} PRIVATE
	"./include/")

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	fmt
	sndfile
	glew32
	glfw3
	opengl32)

target_compile_definitions(
	${PROJECT_NAME} PRIVATE
	GLFW_INCLUDE_NONE)

target_compile_options(
	${PROJECT_NAME} PRIVATE
	-Wall -Wextra -pedantic)

target_compile_features(
	${PROJECT_NAME} PRIVATE
	cxx_std_17)

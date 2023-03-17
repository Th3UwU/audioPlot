#pragma once

typedef unsigned int GLuint;

class Shader
{
public:
	GLuint id;
	Shader(const char* vertexShader, const char* fragmentShader, const char* geometryShader = nullptr);
	void use();

	template <typename T> void setValue(const char* name, T v);
	void setValue(const char* name, float x, float y, float z, float w);
	void setValue(const char* name, float x, float y, float z);
	void setValue(const char* name, float x, float y);
};

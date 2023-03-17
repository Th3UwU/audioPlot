#include "shader.hpp"

#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C"
{
	#include <GL/glew.h>
}

using namespace glm;

static GLchar* readFile(const char* fileName)
{
	FILE* file = fopen(fileName, "rb");
	fseek(file, 0L, SEEK_END);
	size_t size = ftell(file);
	GLchar* string = new char[size + 1];
	fseek(file, 0L, SEEK_SET);
	fread(string, sizeof(char), size, file);
	string[size] = 0;
	fclose(file);
	return string;
}

Shader::Shader(const char* vertexShader, const char* fragmentShader, const char* geometryShader)
{
	char infoLog[1024];
	GLchar* code;
	GLint success;

	GLuint VShader;
	code = readFile(vertexShader);
	VShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VShader, 1, &code, NULL);
	glCompileShader(VShader);
	delete[] code;
	glGetShaderiv(VShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(VShader, 1024, NULL, infoLog);
		printf("Vertex Shader error:\n%s\n", infoLog);
	}

	GLuint FShader;
	code = readFile(fragmentShader);
	FShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FShader, 1, &code, NULL);
	glCompileShader(FShader);
	delete[] code;
	glGetShaderiv(FShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(FShader, 1024, NULL, infoLog);
		printf("Fragment Shader error:\n%s\n", infoLog);
	}

	GLuint GShader;
	if (geometryShader)
	{
		code = readFile(geometryShader);
		GShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(GShader, 1, &code, NULL);

		glCompileShader(GShader);
		delete[] code;
		glGetShaderiv(GShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(GShader, 1024, NULL, infoLog);
			printf("Geometry Shader error:\n%s\n", infoLog);
		}
	}

	id = glCreateProgram();
	glAttachShader(id, VShader);
	glAttachShader(id, FShader);
	if(geometryShader != nullptr)
		glAttachShader(id, GShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 1024, NULL, infoLog);
		printf("Linking Shader error:\n%s\n", infoLog);
	}

	glDeleteShader(VShader);
	glDeleteShader(FShader);
	if(geometryShader != nullptr)
		glDeleteShader(GShader);
}

void Shader::use()
{
	glUseProgram(id);
}

template <> void Shader::setValue<mat4>(const char* name, mat4 v) { glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, value_ptr(v)); }
template <> void Shader::setValue<mat3>(const char* name, mat3 v) { glUniformMatrix3fv(glGetUniformLocation(id, name), 1, false, value_ptr(v)); }
template <> void Shader::setValue<mat2>(const char* name, mat2 v) { glUniformMatrix2fv(glGetUniformLocation(id, name), 1, false, value_ptr(v)); }

template <> void Shader::setValue<vec4>(const char* name, vec4 v) { glUniform4fv(glGetUniformLocation(id, name), 1, value_ptr(v)); }
template <> void Shader::setValue<vec3>(const char* name, vec3 v) { glUniform3fv(glGetUniformLocation(id, name), 1, value_ptr(v)); }
template <> void Shader::setValue<vec2>(const char* name, vec2 v) { glUniform2fv(glGetUniformLocation(id, name), 1, value_ptr(v)); }

template <> void Shader::setValue<int>(const char* name, int v) { glUniform1i(glGetUniformLocation(id, name), v); }
template <> void Shader::setValue<float>(const char* name, float v) { glUniform1f(glGetUniformLocation(id, name), v); }
template <> void Shader::setValue<unsigned int>(const char* name, unsigned int v) { glUniform1ui(glGetUniformLocation(id, name), v); }

void Shader::setValue(const char* name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(id, name), x, y, z, w); }
void Shader::setValue(const char* name, float x, float y, float z) { glUniform3f(glGetUniformLocation(id, name), x, y, z); }
void Shader::setValue(const char* name, float x, float y) { glUniform2f(glGetUniformLocation(id, name), x, y); }

#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

extern "C"
{
	#include <sndfile.h>
	#include <GLFW/glfw3.h>
	#include <GL/glew.h>
}

#include <shader.hpp>

void readSound(const char* path, int16_t*& pcm, SF_INFO& sndInfo)
{
	SNDFILE* sndFile = sf_open(path, SFM_READ, &sndInfo);
	pcm = (int16_t*)malloc(sizeof(int16_t) * sndInfo.channels * sndInfo.frames);
	sf_count_t framesRead = sf_readf_short(sndFile, pcm, sndInfo.frames);
	if (framesRead == sndInfo.frames)
		printf("Total frames: %lli\n", sndInfo.frames);
	else
		printf("Error\n");
	sf_close(sndFile);
}

int main()
{
	int16_t* pcm;
	SF_INFO sndInfo;
	readSound("../song.wav", pcm, sndInfo);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Audio Plot", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();

	glViewport(0, 0, 1280, 720);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mat4 projection = ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
	Shader shaderLines("../shader/lineStrip.vert", "../shader/lineStrip.frag");

	// Get points
	// Allocate the double size of PCM (X, Y)
	//  x1   y1     x2   y2     x3   y3
	// [0][pcm[0]] [1][pcm[1]] [2][pcm[2]] ...
	int32_t* wave = (int32_t*)malloc(sizeof(int32_t) * sndInfo.frames * 2);
	for (sf_count_t i = 0; i < sndInfo.frames; i++)
	{
		wave[i*2] = i;											// X
		wave[(i*2)+1] = (int32_t)-pcm[i];		// Y
	}
	free(pcm);

	// Wave VAO
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(int32_t) * sndInfo.frames * 2, wave, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 2, GL_INT, 2 * sizeof(int32_t), (void*)(sizeof(int32_t) * 0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// DRAW WAVE

		/* Transformations */
		mat4 model = mat4(1.0f);
		model = translate(model, vec3(0.0f, 360.0f, 0.0f));
		model = scale(model, vec3(0.005f, 0.005f, 0.0f));

		/* Set shader and pass transformations */
		shaderLines.use();
		shaderLines.setValue("model", model);
		shaderLines.setValue("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, sndInfo.frames);
		glBindVertexArray(0);
		//

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	free(wave);

	return 0;
}
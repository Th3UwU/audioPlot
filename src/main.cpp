#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C"
{
	#include <sndfile.h>
	#include <GLFW/glfw3.h>
	#include <GL/glew.h>
}

#include <shader.hpp>

const int windowWidth = 1280;
const int windowHeight = 720;

void readSound(const char* path, int16_t*& pcm, SF_INFO& sndInfo)
{
	SNDFILE* sndFile = sf_open(path, SFM_READ, &sndInfo);
	pcm = (int16_t*)malloc(sizeof(int16_t) * sndInfo.channels * sndInfo.frames);
	sf_count_t framesRead = sf_readf_short(sndFile, pcm, sndInfo.frames);
	if (framesRead == sndInfo.frames)
		printf("Total frames: %lli\nSample rate: %i\n", sndInfo.frames, sndInfo.samplerate);
	else
		printf("Error\n");
	sf_close(sndFile);
}

void getWave(int16_t*& pcm, SF_INFO& sndInfo, float start, float end, int32_t*& wave, size_t& waveSize, uint32_t& lines)
{
	// Get START and END index from PCM[]
	int32_t firstFrame = sndInfo.samplerate * start;
	int32_t lastFrame = sndInfo.samplerate * end;

	// Total size of WAVE[]
	waveSize = 2 * sizeof(int32_t) * (lastFrame - firstFrame);

	// Count of total lines
	lines = lastFrame - firstFrame;

	// Allocate mem and copy corresponding data
	wave = (int32_t*)malloc(waveSize);
	for (int32_t i = firstFrame; i < lastFrame; i++)
	{
		wave[i*2] = i;							// x
		wave[(i*2)+1] = (int32_t)(-pcm[i]);		// y
	}
}

int main()
{
	int16_t* pcm;
	SF_INFO sndInfo;
	readSound("../song.ogg", pcm, sndInfo);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Audio Plot", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	glfwSwapInterval(1);

	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, float(windowWidth), float(windowHeight), 0.0f, -1.0f, 1.0f);
	Shader shaderLines("../shader/lineStrip.vert", "../shader/lineStrip.frag");

	// Get wave
	size_t waveSize;
	uint32_t lines;
	int32_t* wave;
	getWave(pcm, sndInfo, 0.0f, 0.05f, wave, waveSize, lines);
	free(pcm);

	// Wave VAO
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, waveSize, wave, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 2, GL_INT, 2 * sizeof(int32_t), (void*)(sizeof(int32_t) * 0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	free(wave);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// DRAW WAVE

		/* Transformations */
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 360.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.01f, 0.0f));

		/* Set shader and pass transformations */
		shaderLines.use();
		shaderLines.setValue("model", model);
		shaderLines.setValue("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, lines);
		glBindVertexArray(0);
		//

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

#include "LoadShaders.h"

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>


#include "Camera.h"

using namespace std;
using namespace glm;

GLuint VBO;
GLuint textureHandle;

//Shader Programs
GLuint renderHandle;
GLuint computeHandle;

//Declaration of methods
void RenderScene();
void InitStuff();

//Random stuff
int oddNumber;

Camera* m_camera;


void CreateVertexBuffer()
{
	vector<vec3> vertices;
	vertices.push_back(vec3(0, 0, 0));
	vertices.push_back(vec3(1, -1, 0));
	vertices.push_back(vec3(-1, -1, 0));

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
}

GLuint CreateComputeShader()
{
	GLuint shaderProgramHandle = glCreateProgram();
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

	const char *semiOldcomputeShaderText[] =
	{
		"#version 430\n",
		"uniform mat4 viewProj;\
		 writeonly uniform image2D destTex;\
		 layout (local_size_x = 16, local_size_y = 16) in;\
		 void main()\
		 {\
			vec3 position = vec3((gl_globalInvocationID.xy), 0);\
			vec3 direction = viewProj * vec3(0,0,1);\
		 }"
	};

	const char *OLDcomputeShaderText[] = {
		"#version 430\n",
		"uniform float roll;\
		 writeonly uniform image2D destTex;\
		 layout (local_size_x = 16, local_size_y = 16) in;\
		 void main() {\
			 ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);\
			 float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);\
			 float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;\
			 imageStore(destTex, storePos, vec4(1-globalCoef*localCoef, 0, 0, 0.0));\
		 }"
	};

	glShaderSource(computeShader, 2, OLDcomputeShaderText, NULL);
	glCompileShader(computeShader);

	//Check for errors
	int rvalue;
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in compiling the compute shader\n");
		GLchar log[10240];
		GLsizei length;
		glGetShaderInfoLog(computeShader, 10239, &length, log);
		fprintf(stderr, "Compiler log:\n%s\n", log);
	}



	glAttachShader(shaderProgramHandle, computeShader);

	glLinkProgram(shaderProgramHandle);
	glGetProgramiv(shaderProgramHandle, GL_LINK_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in linking compute shader program\n");
		GLchar log[10240];
		GLsizei length;
		glGetProgramInfoLog(shaderProgramHandle, 10239, &length, log);
		fprintf(stderr, "Linker log:\n%s\n", log);
	}

	glUseProgram(shaderProgramHandle);
	glUniform1i(glGetUniformLocation(shaderProgramHandle, "destTex"), 0);

	glDispatchCompute(512 / 16, 512 / 16, 1);

	return shaderProgramHandle;


}
GLuint CreateRenderProgram()
{
	//Create the handles
	GLuint programHandle = glCreateProgram();
	GLuint vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

	//Write the shaders
	const char *vertexShaderText[] = {
		"#version 430\n",
		"in vec2 pos;\
		 out vec2 texCoord;\
		 void main() {\
			 texCoord = pos*0.5f + 0.5f;\
			 gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\
		 }"
	};

	const char *fragmentShaderText[] = {
		"#version 430\n",
		"uniform sampler2D sourceTexture;\
		 in vec2 texCoord;\
		 out vec4 color;\
		 void main() {\
			 float c = texture(sourceTexture, texCoord).x;\
			 color = vec4(0, 1.0, 0, 1.0);\
			 color = texture(sourceTexture, texCoord);\
		 }"
	};

	//Set the source
	glShaderSource(vertexShaderHandle, 2, vertexShaderText, NULL);
	glShaderSource(fragmentShaderHandle, 2, fragmentShaderText, NULL);

	//Compile vertex shader
	glCompileShader(vertexShaderHandle);
	//Check errors
	int rvalue;
	glGetShaderiv(vertexShaderHandle, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in compiling vp\n");
	}
	glAttachShader(programHandle, vertexShaderHandle);

	//Compile fragment shader
	glCompileShader(fragmentShaderHandle);
	//Check errors
	glGetShaderiv(fragmentShaderHandle, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in compiling fp\n");
	}
	glAttachShader(programHandle, fragmentShaderHandle);

	//Link program
	glBindFragDataLocation(programHandle, 0, "color");
	glLinkProgram(programHandle);
	//Check errors
	glGetProgramiv(programHandle, GL_LINK_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in linking sp\n");
	}

	//Use program
	glUseProgram(programHandle);
	//Get texture uniform
	glUniform1i(glGetUniformLocation(programHandle, "sourceTexture"), 0);

	//Create vertex array for draw surface
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	//Create positions for draw surface
	GLuint positionBuffer;
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	float data[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, data, GL_STREAM_DRAW);
	GLint positionUniformHandle = glGetAttribLocation(programHandle, "pos");
	glVertexAttribPointer(positionUniformHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionUniformHandle);

	return programHandle;

}

void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
}

GLuint GenerateTexture()
{
	GLuint textureHandle;
	glGenTextures(1, &textureHandle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(0, textureHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	return textureHandle;

}

void InitStuff()
{
	m_camera = new Camera(vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 0, 0));
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);




	/////////////OUR STUFF GOES HERE//////////////


	//Compute shader stuff
	glUseProgram(computeHandle);


	//oddNumber++;
	//cout << oddNumber << endl;

	//glUniform1f(glGetUniformLocation(computeHandle, "roll"), (float)oddNumber*0.01f);
	glUniform4fv(glGetUniformLocation(computeHandle, "viewProj"), 1, &m_camera->GetViewProj()[0][0]);
	glUniform1i(glGetUniformLocation(computeHandle, "outputTexture"), 0);
	glDispatchCompute(512 / 16, 512 / 16, 1);





	////////////END OUR STUFF/////////////////





	//Render shader stuff
	glUseProgram(renderHandle);
	glUniform1i(glGetUniformLocation(computeHandle, "sourceTexture"), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 3);

	//glDisableVertexAttribArray(0);

	glutSwapBuffers();
}


int main(int argc, char** argv)
{


	//Initialize glut stuff
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Raytracer");

	InitializeGlutCallbacks();

	//Initialize glew
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		cout << "Error initing glew";
	}
	
	//Initialize our stuff
	CreateVertexBuffer();
	InitStuff();
	textureHandle = GenerateTexture();
	renderHandle = CreateRenderProgram();


	ShaderInfo shaderInfo[] = { {GL_COMPUTE_SHADER, "ComputeShader.glsl"} , {GL_NONE, NULL} };

	computeHandle = LoadShaders(shaderInfo);
	//computeHandle = CreateComputeShader();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glutMainLoop();

	return 0;
}

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Onaj fix glm::mat4 model = glm::mat4(1.0f);
// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformModel;
bool direction = true;
float triOffset = 0.0f;
float triOffsetMAX = 0.7f;
float triIncrement = 0.0005f;

// Vertex Shader
static const char* vShader = "						\n\
#version 330										\n\
layout (location = 0) in vec3 pos;					\n\
uniform mat4 model;									\n\
void main()											\n\
{													\n\
													\n\
	gl_Position = model * vec4(0.1*pos.x, 0.5*pos.y, 0.5*pos.z, 1.0); \n\
}";
// Fragment Shader
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
													\n\
	color = vec4(1.0, 0.0, 0.0, 1.0);				\n\
}";

											
void create_triangle() 
{
	GLfloat vertecies[] =
	{
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
	};
	// Bind
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies), vertecies, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind
	glBindVertexArray(0);
}
void add_shader(GLuint the_program, const char* shader_code, GLenum shader_type)
{
	GLuint theShader = glCreateShader(shader_type);

	const GLchar* theCode[1];
	theCode[0] = shader_code;

	GLint codeLenght[1];
	codeLenght[0] = strlen(shader_code);

	glShaderSource(theShader, 1, theCode, codeLenght);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader : '%s'\n", shader_type, eLog);
		return;
	}
	glAttachShader(the_program, theShader);

}
void compile_shaders() 
{
	shader = glCreateProgram();
	if (!shader)
	{
		printf("Error creating shader program\n");
		return;
	}
	add_shader(shader, vShader, GL_VERTEX_SHADER);
	add_shader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}
	uniformModel = glGetUniformLocation(shader, "model");
	
}
int main()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		printf("GLFW initialization failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// if it detects old (deprecated) code it breaks
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	create_triangle();
	compile_shaders();


	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();


		if (direction)
		{
			triOffset += triIncrement;
		}
		else
		{
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triOffsetMAX) 
		{
			direction = !direction;
		}
		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(triOffset, 0, 0));

		glUniform1f(uniformModel, triOffset);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}


	return 0;

}
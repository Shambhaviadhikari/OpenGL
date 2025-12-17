#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>	// file stream
#include <string>
#include <sstream>	//header providing stringstream class


void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::cout << "OPENGL ERROR :" << std::endl << message << std::endl;
	__debugbreak();

}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

 static ShaderProgramSource ParseShader(const std::string &filepath)
{
	std::ifstream stream(filepath);	// puts the file into the buffer with ID stream

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::string line;
	std::stringstream ss[2];	//creating an object of stringstream class
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;

		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();	// same as &source[0]
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char*) malloc (length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs) ;
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		std::cout << "Error initializing glfw" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	window = glfwCreateWindow(640, 480, "CAR COLLISION SIMULATION", NULL, NULL);
	if (!window)
	{
		std::cout << "Error opening window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	// make current window the Opengl rendering context

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)	// glewInit() should be called after a valid OpenGL rendering context is created
		std::cout << "Error while initializing glew" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;	// print opengl version and graphics card version

	float positions[] = {
		-0.5f, -0.5f,	//0
		 0.5f, -0.5f,	//1
		 0.5f,  0.5f,	//2
		-0.5f,  0.5f	//3
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0
	};

	unsigned int buffer;	// creating a variable to store the ID of the vertex buffer object to be created
	glGenBuffers(1, &buffer);   // creates a single buffer object since 1 is passed and keeps its ID in the unsigned int variable passed
	glBindBuffer(GL_ARRAY_BUFFER, buffer);  // selects the buffer with passed ID
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);    //places the data into the selected buffer

	unsigned int vao;	// creating a variable to store the ID of the vertex array object to be created
	glGenVertexArrays(1, &vao);	// creates a single vertex array object since 1 is passed and keeps its ID in the unsigned int variable passed
	glBindVertexArray(vao);	//selects the vertex array with passed ID
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);	// index 0 of currently selected vertex array object will represent currently selected vertex buffer object
	glEnableVertexAttribArray(0);   // of the currently selected vertex array object it enables the given index vertex object

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_Color");
	if (location == -1) std::cout << "couldn't find the uniform variable of the given name!!!!!" << std::endl;
	glUniform4f(location, 0.5f, 0.4f, 0.6f, 0.2f);

	float r = 0.0f;
	float increment = 0.05f;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);	// Enable sticky keys

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform4f(location, r, 0.4f, 0.6f, 0.2f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		
		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}
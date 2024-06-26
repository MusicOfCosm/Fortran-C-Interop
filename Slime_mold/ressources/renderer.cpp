#include "renderer.hpp"
//#pragma warning(disable : 4996)

Object::Object()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

Object::Object(std::vector<GLfloat> &vertices, GLint vertAttribSizes[], GLint attribCount, GLenum usage, GLint instances)
{
	m_verts = GLint(vertices.size() / vertAttribSizes[0]);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), usage);

	int sum = 0;
	for (int i = 0; i < attribCount; ++i) {
		sum += vertAttribSizes[i];
	}

	int n = 0;
	for (int i = 0; i < attribCount; ++i) {
		//printf("glVertexAttribPointer(%d, %d, GL_FLOAT, GL_FALSE, %d * sizeof(GLfloat), (void*)(%d * sizeof(GLfloat)));\n",
		//	i, vertAttribSizes[i], sum, n);
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vertAttribSizes[i], GL_FLOAT, GL_FALSE, sum * sizeof(GLfloat), (void*)(n * sizeof(GLfloat)));
		if (instances != 0)
			glVertexAttribDivisor(i, 1);
		n += vertAttribSizes[i];
	}
	if (instances) m_instances = instances;
}

Object::Object(std::vector<glm::vec3> &vertices, GLint vertAttribSizes[], GLint attribCount, GLenum usage, GLint instances)
{
	m_verts = GLint(vertices.size() / vertAttribSizes[0]);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), usage);

	int sum = 0;
	for (int i = 0; i < attribCount; ++i) {
		sum += 1;
	}

	int n = 0;
	for (int i = 0; i < attribCount; ++i) {
		//printf("glVertexAttribPointer(%d, %d, GL_FLOAT, GL_FALSE, %d * sizeof(GLfloat), (void*)(%d * sizeof(GLfloat)));\n",
		//	i, vertAttribSizes[i], sum, n);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, sum * sizeof(glm::vec3), (void*)(n * sizeof(glm::vec3)));
		glEnableVertexAttribArray(i);
		if (instances != 0)
			glVertexAttribDivisor(i, 1);
		n += 1;
	}
	if (instances) m_instances = instances;
}
//Object::Object(std::vector<GLfloat> &vertices, GLint vertAttribSizes[], GLint attribCount, GLint instances)
//{
//	m_verts = GLint(vertices.size() / vertAttribSizes[0]);
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glGenBuffers(1, &VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
//
//	int sum = 0;
//	for (int i = 0; i < attribCount; ++i) {
//		sum += vertAttribSizes[i];
//	}
//
//	int n = 0;
//	for (int i = 0; i < attribCount; ++i) {
//		//printf("glVertexAttribPointer(%d, %d, GL_FLOAT, GL_FALSE, %d * sizeof(GLfloat), (void*)(%d * sizeof(GLfloat)));\n",
//		//	i, vertAttribSizes[i], sum, n);
//		glVertexAttribPointer(i, vertAttribSizes[i], GL_FLOAT, GL_FALSE, sum * sizeof(GLfloat), (void*)(n * sizeof(GLfloat)));
//		glEnableVertexAttribArray(i);
//		glVertexAttribDivisor(i, 1);
//		n += vertAttribSizes[i];
//	}
//}

void Object::loadShader(GLuint &shader, const char* shaderSourceCode, bool loadFromFile)
{
	GLint shaderType;
	//shaderType takes the value GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER
	glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

	//Reading and loading a shader (glsl file) as a const char* string
	if (loadFromFile == true) {
		std::ifstream File(shaderSourceCode);
		std::string fileContent((std::istreambuf_iterator<char>(File)),
								(std::istreambuf_iterator<char>()));
		if (fileContent == "")
			fprintf(stderr, "ERROR: %s SHADER - FILE FAILED TO LOAD OR EMPTY\n",
					(shaderType == GL_VERTEX_SHADER ? "VERTEX" :
					 shaderType == GL_GEOMETRY_SHADER ? "GEOMETRY" :
					 shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" : "UNKNOWN"));

#ifdef _DEBUG
		fileContent.insert(18, "#pragma debug(on)\n");
#endif

		shaderSourceCode = fileContent.c_str();
		glShaderSource(shader, 1, &shaderSourceCode, NULL); //If it exits the scope, shaderSource becomes a dangling pointer
		glCompileShader(shader);
	}
	else {
		if (shaderSourceCode[0] == '\0')
			fprintf(stderr, "ERROR: %s SHADER - FAILED TO LOAD OR EMPTY\n",
					(shaderType == GL_VERTEX_SHADER ? "VERTEX" :
					 shaderType == GL_GEOMETRY_SHADER ? "GEOMETRY" :
					 shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" : "UNKNOWN"));
		glShaderSource(shader, 1, &shaderSourceCode, NULL);
		glCompileShader(shader);
	}

	//Checking for compiling errors for the shader code
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		GLchar infoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		fprintf(stderr, "ERROR: %s SHADER - COMPILATION FAILED\n%s\n",
				(shaderType == GL_VERTEX_SHADER ? "VERTEX" :
				 shaderType == GL_GEOMETRY_SHADER ? "GEOMETRY" :
				 shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" :
				 shaderType == GL_COMPUTE_SHADER ? "COMPUTE" : "UNKNOWN"),
				 infoLog);
		glDeleteShader(shader);
	}
	else glAttachShader(shaderProgram, shader);
}

void Object::loadProgram()
{
	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success) {
		fprintf(stderr, "WARNING: SHADER -> PROGRAM - ALREADY LINKED\n");
		glUseProgram(shaderProgram);
	}
	else {
		//Linking all the attached shaders to the program
		glLinkProgram(shaderProgram);

		//Checking for errors in the linking process
		GLchar infoLog[1024];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
			fprintf(stderr, "ERROR: SHADER -> PROGRAM - LINKING FAILED\n%s\n", infoLog);
		}
		else //Binding the shader program to tell OpenGL to use it to draw
			glUseProgram(shaderProgram);
		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);
		glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
	}
}

void Object::loadTexture2D(GLenum textureUnit, const char* texFilePath, const char* texUniformName) {
	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(textureUnit); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	//S in like x, T like y, and R like z
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_REPEAT, GL_CLAMP_TO_EDGE/BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //GL_MIRRORED_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR(_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST

	stbi_set_flip_vertically_on_load(true);
	int imgWidth, imgHeight, nColorChannels;
	unsigned char* texData = stbi_load(texFilePath, &imgWidth, &imgHeight, &nColorChannels, 0);
	if (texData) {
		GLenum format = 0;
		if (nColorChannels == 1)
			format = GL_RED;
		else if (nColorChannels == 3)
			format = GL_RGB;
		else if (nColorChannels == 4)
			format = GL_RGBA;
									   //format used by OpenGL			 //actual format of the image
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cerr << "Failed to load texture: " << texFilePath << std::endl;
	stbi_image_free(texData);

	//std::cout << texUniformName << ' ' << textureUnit - GL_TEXTURE0 << '\n';
	SET1I(shaderProgram, texUniformName, textureUnit - GL_TEXTURE0);
}

GLuint Object::loadFramebuffer(GLenum textureUnit, GLsizei WIDTH, GLsizei HEIGHT)
{
	GLuint FBO;
	glGenFramebuffers(1, &FBO);

	//creating a color attachement texture
	GLuint fbo_tex;
	glGenTextures(1, &fbo_tex);
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //if render target is the size of the texture, can use GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error - Status: 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	
	return FBO;
}

void Object::Use()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
}
//void Object::UseInstances()
//{
//	glUseProgram(shaderProgram);
//	glBindVertexArray(VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_instances, &agents[0], GL_STATIC_DRAW);
//}

void Object::Update(GLint dataSize, void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
}

void Object::Draw(GLenum primitive)
{
	glDrawArrays(primitive, 0, m_verts);
}
void Object::DrawInstances(GLenum primitive, GLuint instances)
{
	glDrawArraysInstanced(primitive, 0, m_verts, instances);
}

Object::~Object()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
}
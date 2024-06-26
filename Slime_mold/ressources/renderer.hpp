#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>

#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define DEBUGONLY(lines) (#ifdef _DEBUG\)
#ifndef ABSTRACTIONS
#define ABSTRACTIONS
#define glmVEC2(vec2) vec2.x, vec2.y
#define glmVEC3(vec3) vec3.x, vec3.y, vec3.z
#define glmVEC4(vec4) vec4.x, vec4.y, vec4.z, vec4.w
#define VEC2(n) n, n
#define VEC3(n) n, n, n
#define VEC4(n) n, n, n, n
#define combVEC2(glm, n) glm.x*n, glm.y*n
#define combVEC3(glm, n) glm.x*n, glm.y*n, glm.z*n
#define combVEC4(glm, n) glm.x*n, glm.y*n, glm.z*n, glm.w*n

#define SET1I(program, varName, value ) glUniform1i(glGetUniformLocation(program, varName), value)
#define SET2I(program, varName, values) glUniform2i(glGetUniformLocation(program, varName), values)
#define SET3I(program, varName, values) glUniform3i(glGetUniformLocation(program, varName), values)

#define SET1F(program, varName, value ) glUniform1f(glGetUniformLocation(program, varName), value)
#define SET2F(program, varName, values) glUniform2f(glGetUniformLocation(program, varName), values)
#define SET3F(program, varName, values) glUniform3f(glGetUniformLocation(program, varName), values)
#define SET4F(program, varName, values) glUniform4f(glGetUniformLocation(program, varName), values)

#define SET1D(program, varName, value ) glUniform1d(glGetUniformLocation(program, varName), value)
#define SET2D(program, varName, values) glUniform2d(glGetUniformLocation(program, varName), values)
#define SET3D(program, varName, values) glUniform3d(glGetUniformLocation(program, varName), values)

#define SET3FV(program, varName, instances, values) glUniform3fv(glGetUniformLocation(program, varName), instances, glm::value_ptr(values))
#define SET4FV(program, varName, instances, values) glUniform4fv(glGetUniformLocation(program, varName), instances, glm::value_ptr(values))
#define SETMAT(program, varName, values) glUniformMatrix4fv(glGetUniformLocation(program, varName), 1, GL_FALSE, glm::value_ptr(values))
#endif

//enum Shape {
//	NONE,
//	TRIANGLE,
//	SQUARE,
//	CUBE,
//	CIRCLE
//};

/**
 * @brief Generates a VAO (vertex array object), a VBO (vertex buffer oject), binds them together, and enables the vertex attributes. 
 * @param vertices The data to be stored on the VBO. Must be a vector of GLfloat or vec3.
 * @param vertAttribSizes An array containing the sizes of the vertex attributes.
 * @param attribCount The number of vertex attributes. Input sizeof attribSizes / sizeof vertices[0]
 * @param usage Hint to OpenGL as to how the buffer data should be accessed. Default is GL_STATIC_DRAW.
 * @param instances Number of instances (if using instancing). Default is 0.
 */
class Object
{
public:
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint shaderProgram = glCreateProgram();
	GLint m_verts = 0; //Number of vertices to draw
	GLint m_instances = 0; //If instancing is used

	Object();
	Object(std::vector<GLfloat> &vertices, GLint vertAttribSizes[], GLint attribCount, GLenum usage=GL_STATIC_DRAW, GLint instances = 0);
	Object(std::vector<glm::vec3> &vertices, GLint vertAttribSizes[], GLint attribCount, GLenum usage=GL_STATIC_DRAW, GLint instances = 0);

	/**
	 * @brief Compiles a given shader and attaches it with the program, and checks for errors.
	 * @param shader The shader's ID, either Object.vertex, or Object.fragment.
	 * @param shaderSourceCode The shader code written in glsl.
	 * @param loadFromFile Specifies if the glsl code is from a file or not. Set to true by default.
	 */
	void loadShader(GLuint &shader, const char* shaderSourceCode, bool loadFromFile = true);

	/**
	 * @brief Links all the attached shaders to the program, checks for errors, and binds the shader program to tell OpenGL to use it to draw.
	 */
	void loadProgram();

	/**
	 * @brief Binds a texture to a texture unit, and sets its uniform.
	 * @param textureUnit GL_TEXTUREx, where x is a number beginning at 0. The number of available texture units depends on your GPU.
	 * @param texFilePath The filepath of the image that will be used to generate the texture.
	 * @param texUniformName The name of the uniform to be set in the shader.
	 */
	void loadTexture2D(GLenum textureUnit, const char* texFilePath, const char* texUniformName);


	

	/**
	 * @brief Create a framebuffer, and an associated color attachment texture.
	 * @param textureUnit GL_TEXTUREx, where x is a number beginning at 0. The number of available texture units depends on your GPU. Be mindful other units are already being used by normal textures.
	 * @param WIDTH Width of the framebuffer texture.
	 * @param HEIGHT Height of the framebuffer texture.
	 * @return The ID of the framebuffer object. (You need it in order to bind and unbind it)
	 */
	GLuint loadFramebuffer(GLenum textureUnit, GLsizei WIDTH, GLsizei HEIGHT);

	//Binds a Object VAO and program to be used
	void Use();
	void Update(GLint dataSize, void* data);

	//Draw the Object
	//Draw everything
	void Draw(GLenum primitive);
	//Draw everything with instances
	void DrawInstances(GLenum primitive, GLuint instances);


	~Object();
};

#endif
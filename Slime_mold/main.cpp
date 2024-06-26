#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
// #include <future>
// #include <functional>
#ifdef __APPLE__
	#include <thread>
	#include <ctime>
	#include <chrono>
#endif

#include "ressources/renderer.hpp"
#include "ressources/glDebug.h"


unsigned int seed = (unsigned int)time(0);
std::mt19937 gen(seed);

int randint(int min, int max) {
	std::uniform_int_distribution<int> rdint(min, max);
	return rdint(gen);
}

float uniform(float min, float max) {
	std::uniform_real_distribution<float> rdfloat(min, max);
	return rdfloat(gen);
}

#if defined(_DEBUG) && !defined(__APPLE__)
//Error messaging from OpenGL
void GLAPIENTRY
MessageCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam)
{
	fprintf(stderr, "\n\nGL CALLBACK:\nSource:   %s\nType:     %s\nid:       %d\nSeverity: %s\nMessage:  %s\n\n",
			debugSource(source), debugType(type), id, debugSeverity(severity), message);
	// scanf_s("");
	exit(EXIT_FAILURE);
}
#endif

//glfw error callback
void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}

bool change = false;
GLFWmonitor* monitor;
const GLFWvidmode* mode;
extern "C" int WIDTH, HEIGHT;
int oldWidth, oldHeight;

//Enables to resizes what is drawn along the window, if not used, the shape drawn would always stay the same size
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
#if defined(_DEBUG) || !defined(_WIN32)
	change = true;
#endif
}

//timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

extern "C" {
	float SPEED = uniform(1.f, 2.5f);
	int SENSOR_RANGE = randint(5, 30); // Sensor Offset
	int SENSOR_SIZE = randint(0, 2);
	float sa = uniform(30.0f, 60.0f);
	float SA = (float)M_PI / 180.f * sa; //uniform(pi / 3.5f, pi / 2.0f); //Sensor Angle
	float ra = uniform(5.0f, 60.0f);
	float RA = (float)M_PI / 180.f * ra; //uniform(0, pi / 4.5f); //Rotation Angle
	bool BOUNDARY = randint(0, 1); //0 and 1 are false and true
}
int START = randint(0, 3); //0: random, 1: centre, 2: circle, 3: corner
bool BLUR = randint(0, 1);
//int SPECIES = randint(1, 3);
//bool IGNORE = randint(0, 1);
extern "C" {
	void pass_ptr(GLfloat **arr);
	void movement_f(glm::vec3* array);
	
	int instances, canvas_size;
	std::vector<GLfloat> canvas(1); //Can't point to the first element if it is empty
	GLfloat *canvas_ptr = &canvas[0];
}

void init(glm::vec3 &agent);

void Boundary(float & x, float & y);

bool bind;
void movement(glm::vec3 &agent);
void vec_movement(std::vector<glm::vec3> &agents);


int main()
{
	if (!glfwInit())
		return -1;

	glfwSetErrorCallback(error_callback);

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	WIDTH = mode->width;
	HEIGHT = mode->height;


	//Hints: Tells OpenGL what version to use, if none is specified, it uses the latest
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); //Mac doesn't support beyond 4.1
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* settings = glfwCreateWindow(WIDTH/2, HEIGHT/2, "Settings", NULL, NULL);
	if (settings == NULL) { // glfwCreateWindow returns NULL if an error occurs
		fprintf(stderr, "Failed to initialize GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(settings);
	glfwSwapInterval(1);
	glfwMakeContextCurrent(nullptr);

#if defined(_DEBUG) && !defined(__APPLE__)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(WIDTH / 2, HEIGHT / 2, "Slime mold simulation", NULL, NULL);
#elif defined(__APPLE__)
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Slime mold simulation", NULL, NULL);
#else
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Slime mold simulation", glfwGetPrimaryMonitor(), NULL); //Fullscreen
#endif
	if (window == NULL) { // glfwCreateWindow returns NULL if an error occurs
		fprintf(stderr, "Failed to initialize GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(2);

	// {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		WIDTH = width; HEIGHT = height;
		glViewport(0, 0, WIDTH, HEIGHT);
		canvas.resize(WIDTH * HEIGHT);
		canvas_ptr = &canvas.front();
		canvas_size = canvas.size();
		pass_ptr(&canvas_ptr);
	// }

	//GLFWimage icon[1];
	//icon[0].pixels = stbi_load("ressources/Rosace.png", &icon[0].width, &icon[0].height, 0, 4);
	//glfwSetWindowIcon(window, 1, icon);
	//stbi_image_free(icon[0].pixels);

	// GLFWmonitor* monitor = glfwGetWindowMonitor(window);

	if (glewInit() != GLEW_OK) { //Initializes GLEW, if it's not done properly, terminate program
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(glewInit()));
		glfwTerminate();
		return -2;
	}

#if defined(_DEBUG) && !defined(__APPLE__)
	glEnable(GL_DEBUG_OUTPUT); //see the beginning of the file
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
#ifdef _DEBUG
	//INFO
	printf("Last compiled on %s at %s\n\n", __DATE__, __TIME__);
	printf("GPU Information:\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Shading Language Version: %s\n\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	//These functions are called by glfwPollEvents
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //free movement

	glfwMakeContextCurrent(settings);
	IMGUI_CHECKVERSION();
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(settings, true);
	ImGui_ImplOpenGL3_Init("#version 410");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	glfwMakeContextCurrent(window);

	//Texture max dimension size: 16384
	instances = 100000;
	int newInstances = instances;
	std::vector<glm::vec3> agents(instances);
	for (int i = 0; i < instances; ++i)
		init(agents[i]);

	GLint attribSizes[] = { 1 };
	Object slime(agents, attribSizes, (3 * sizeof attribSizes) / sizeof agents[0], GL_DYNAMIC_DRAW, instances);

	const char* vert =
		#include "shaders/V1.vert"
	;
	const char* frag =
		#include "shaders/V1.frag"
	;

	slime.loadShader(slime.vertexShader, vert, false); slime.loadShader(slime.fragmentShader, frag, false);
	slime.loadProgram();

	glm::mat4 projection = glm::ortho(0.f, (float)WIDTH, 0.f, (float)HEIGHT);
	SETMAT(slime.shaderProgram, "projection", projection);
	//float time;


	std::vector<GLfloat> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f 
	};

	GLint quadAttribSizes[] = { 2, 2 };
	Object screen(quadVertices, quadAttribSizes, sizeof quadAttribSizes / sizeof quadVertices[0], GL_STATIC_DRAW);

	const char* scr_vert =
		#include "shaders/scr_V1.vert"
	;
	const char* scr_frag =
		#include "shaders/scr_V1.frag"
	;
	screen.loadShader(screen.vertexShader, scr_vert, false); screen.loadShader(screen.fragmentShader, scr_frag, false);
	screen.loadProgram();
	SET1I(screen.shaderProgram, "width", WIDTH);
	SET1I(screen.shaderProgram, "height", HEIGHT);

	//screen.Use();
	//SET1I(screen.shaderProgram, "screenTexture", 0);
	//https://learnopengl.com/Advanced-OpenGL/Framebuffers
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//creating a color attachement texture
	GLuint fbo_tex;
	glGenTextures(1, &fbo_tex);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //if render target is the size of the texture, can use GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error - Status: 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));

	
	SET1I(screen.shaderProgram, "width",  WIDTH);
	SET1I(screen.shaderProgram, "height", HEIGHT);

	glm::vec3 color = glm::vec3(1.f);
	bool restart = false;
#ifdef __APPLE__
	clock_t start, finish;
    float iter_time;
	float diff, cap = 1.f/60;
#endif
	float time = (float)glfwGetTime();
	deltaTime = time - lastFrame;
	lastFrame = time;
	float fps = 1 / deltaTime;
	double total_fps = 0.0;
	int iter = 1;
	float avg_fps = total_fps / iter;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while ((!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (!glfwWindowShouldClose(settings) && glfwGetKey(settings, GLFW_KEY_ESCAPE) != GLFW_PRESS)) //Set Esc as a close command
	{
#ifdef __APPLE__
		start = clock();
#endif

		glfwMakeContextCurrent(settings);
		if (glfwGetWindowAttrib(settings, GLFW_FOCUSED))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

			ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
			ImGui::Text("%f fps", avg_fps);
			////ImGui::Text("Pos[0]:\nx=%f\ny=%f\nz=%f", glmVEC3(pos[0]));
			// double run = glfwGetTime();
			// int hours = int(run / 3600);
			// int minutes = int(fmod(run, 3600) / 60); //fmod is like the % operator that can work with floats and doubles
			// double seconds = fmod(fmod(run, 3600), 60);
			// ImGui::Text("Time run: %d hours, %d minutes, and %f seconds", hours, minutes, seconds);

			ImGui::SeparatorText("Variables");

			ImGui::ColorEdit3("Color", &color.x);
			ImGui::SliderFloat("Speed", &SPEED, 1.0f, 2.5f);
			ImGui::SliderInt("Sensor range", &SENSOR_RANGE, 1, 30);
			ImGui::SliderInt("Sensor size", &SENSOR_SIZE, 0, 2);
			ImGui::SliderFloat("Sensor angle", &sa, 0.0f, 180.0f);
			SA = (float)M_PI / 180.f * sa;
			ImGui::SliderFloat("Rotation angle", &ra, 0.0f, 180.0f);
			RA = (float)M_PI / 180.f * ra;
			ImGui::Checkbox("Boundary", &BOUNDARY);
			ImGui::Checkbox("Blur", &BLUR);

			ImGui::Text(" ");
			ImGui::SeparatorText("Reinitialization");

			ImGui::Text("Current instances: %d", instances);
			ImGui::SliderInt("Instances", &newInstances, 0, 500000);
			if (newInstances < 0) newInstances = 0; if (newInstances > 500000) newInstances = 500000;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Command + click to input value");
			
			ImGui::RadioButton("Random", &START, 0); ImGui::SameLine();
			ImGui::RadioButton("Center", &START, 1); ImGui::SameLine();
			ImGui::RadioButton("Circle", &START, 2); ImGui::SameLine();
			ImGui::RadioButton("Corners", &START, 3);

			if (ImGui::Button("Restart")) restart = true;

			ImGui::End();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(settings);
			glfwPollEvents();
		}
		glfwMakeContextCurrent(window);
		// if (instances != newInstances) {
		// 	agents.resize(newInstances);
		// 	if (newInstances > instances) {
		// 		int diff = newInstances - instances;
		// 		for (int i = 0; i < diff; ++i)	agents[instances+i] = agents[randint(0, instances)];
		// 	}
		// 	instances = newInstances;

		// 	glBufferData(GL_ARRAY_BUFFER, agents.size() * sizeof(glm::vec3), agents.data(), GL_DYNAMIC_DRAW);
		// }

		if (restart) {
			instances = newInstances;
			agents.resize(instances);

			for (int i = 0; i < instances; ++i)	init(agents[i]);

			glBufferData(GL_ARRAY_BUFFER, agents.size() * sizeof(glm::vec3), agents.data(), GL_DYNAMIC_DRAW);

			change = true;
		}

		if (change) {
			{
#if defined(_DEBUG) || !defined(_WIN32)
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);
				oldWidth = WIDTH, oldHeight = HEIGHT;
				WIDTH = width; HEIGHT = height;
				glViewport(0, 0, WIDTH, HEIGHT);
				canvas.resize(WIDTH * HEIGHT);
				canvas_ptr = &canvas[0];
				canvas_size = canvas.size();
				pass_ptr(&canvas_ptr);

#endif
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			slime.Use();
			projection = glm::ortho(0.f, (float)WIDTH, 0.f, (float)HEIGHT);
			SETMAT(slime.shaderProgram, "projection", projection);

			canvas.resize(WIDTH * HEIGHT);
			canvas_ptr = &canvas[0];
			canvas_size = canvas.size();
			pass_ptr(&canvas_ptr);

			screen.Use();
			SET1I(screen.shaderProgram, "width",  WIDTH);
			SET1I(screen.shaderProgram, "height", HEIGHT);
#ifdef __APPLE__
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif

			if (!restart) {
				for (int i = 0; i < instances; ++i) {
					agents[i].x = agents[i].x *  WIDTH /  oldWidth;
					agents[i].y = agents[i].y * HEIGHT / oldHeight;
				}
			}
			
			change = false;
		}

		//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
		{
#if !defined(_DEBUG) && defined(_WIN32)
			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
			glfwSetWindowMonitor(window, monitor, NULL, NULL, mode->width, mode->height, mode->refreshRate);
#endif

			time = (float)glfwGetTime();
			deltaTime = time - lastFrame;
			lastFrame = time;
			fps = 1 / deltaTime;
			total_fps += fps;
			iter += 1;
			avg_fps = total_fps / iter;

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
			slime.Use();
			SET3F(slime.shaderProgram, "color", glmVEC3(color));
			SET1I(slime.shaderProgram, "restart", restart);
			//glBindBuffer(GL_ARRAY_BUFFER, slime.VBO);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * agents.size(), &agents[0], GL_STATIC_DRAW);
			slime.Update(GLint(sizeof(glm::vec3)*agents.size()), &agents[0]);
			// glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);

			glDrawArraysInstanced(GL_POINTS, 0, 1, instances);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			screen.Use();
			SET1I(screen.shaderProgram, "blur", BLUR);
			SET1I(screen.shaderProgram, "restart", restart);
			screen.Draw(GL_TRIANGLES);

			//Turns out that this is what causes the most overhead
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
			glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glReadPixels(0, 0, WIDTH, HEIGHT, GL_RED, GL_FLOAT, canvas.data());


			// Using Fortran is not the only thing that I tried
			
			// for (glm::vec3 &agent : agents)	movement(agent); //14 fps
			// vec_movement(agents);
			// std::async(std::launch::async, movement, agents[i]);
			
			/*
			//Async
			// { // By vector  11 fps
			// 	// std::future<void> fut = std::async(std::launch::async, static_cast<void(*)(std::vector<glm::vec3>&)>(movement), std::ref(agents));
			// 	std::future<void> fut = std::async(std::launch::async, vec_movement, std::ref(agents));
			
			// 	// Wait for the task to finish
			// 	fut.get();
			// }
			*/
			/*
			 //By value 4 fps
			// {
			// 	std::vector<std::future<void>> futures;
			// 	for (glm::vec3 &agent : agents)
			// 		futures.push_back(std::async(std::launch::async, movement, std::ref(agent)));
				
			// 	for(auto& fut : futures)
			// 		fut.get();
			// }
			*/

			movement_f(agents.data());

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
		else {
			// glfwSetWindowMonitor(window, NULL, mode->width / 8, mode->height / 4, mode->width / 2, mode->height / 2, GLFW_DONT_CARE);
			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
		}
		restart = false;

		// Buffer swap and I/O processing
#ifdef __APPLE__
		finish = clock();
        iter_time = float(finish - start) / CLOCKS_PER_SEC;
        //std::cout << limit << '\n' << cap << '\n' << iter_time << "\n\n";
        if (iter_time < cap) {
            diff = cap - iter_time;
            //the *1000 converts seconds to milliseconds, I learned way too late that the nano version doesn't work on Windows
            std::this_thread::sleep_for(std::chrono::milliseconds( int(diff * 1000) ));
        }
#endif
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Terminating everyting
	glfwMakeContextCurrent(settings);
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(settings);

	glfwMakeContextCurrent(window);
	slime.~Object();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}


void init(glm::vec3 &agent)
{
	if (START == 0) { //random, the matrix goes from 0 to the designated values-1
		agent.x = uniform(0, float(WIDTH));
		agent.y = uniform(0, float(HEIGHT));
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
	else if (START == 1) { //centre
		agent.x = WIDTH / 2.0f;
		agent.y = HEIGHT / 2.0f;
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
	else if (START == 2) { //circle
		float tmp_pos = uniform(-(float)M_PI, (float)M_PI);
		agent.x = WIDTH / 2.0f + cos(tmp_pos) * uniform(0, float(HEIGHT) / 2.f);
		agent.y = HEIGHT / 2.0f + sin(tmp_pos) * uniform(0, float(HEIGHT) / 2.f);
		agent.z = (float)acos((float(WIDTH) / 2.f - agent.x) / sqrt(pow((float(WIDTH) / 2.f - agent.x), 2) + pow((float(HEIGHT) / 2.f - agent.y), 2)));
		if (agent.y > float(HEIGHT) / 2.f) { agent.z = -agent.z; }
	}
	else { //corner
		agent.x = (BOUNDARY == true) ? float(randint(0, 1) * WIDTH) : 0.F;
		agent.y = (BOUNDARY == true) ? float(randint(0, 1) * HEIGHT) : 0.F;
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
}


void Boundary(float & x, float & y)
{
	if (BOUNDARY == true) {
		if (x > WIDTH) {
			x = float(WIDTH);
		}
		else if (x < 0) {
			x = 0;
		}

		if (y > HEIGHT) {
			y = float(HEIGHT);
		}
		else if (y < 0) {
			y = 0;
		}
	}

	else {
		if (x > WIDTH) {
			x -= WIDTH;
		}
		else if (x < 0) {
			x += WIDTH;
		}

		if (y > HEIGHT) {
			y -= HEIGHT;
		}
		else if (y < 0) {
			y += HEIGHT;
		}
	}
}
void Boundary(int & x, int & y)
{
		if (BOUNDARY == true) {
		if (x > WIDTH) {
			x = WIDTH;
		}
		else if (x < 0) {
			x = 0;
		}

		if (y > HEIGHT) {
			y = HEIGHT;
		}
		else if (y < 0) {
			y = 0;
		}
	}

	else {
		if (x > WIDTH) {
			x -= WIDTH;
		}
		else if (x < 0) {
			x += WIDTH;
		}

		if (y > HEIGHT) {
			y -= HEIGHT;
		}
		else if (y < 0) {
			y += HEIGHT;
		}
	}
}



float sense(float sensorAngleOffset, glm::vec3 agent)
{
	float sensorAngle = agent.z + sensorAngleOffset;
	float sensorDirX = cos(sensorAngle);
	float sensorDirY = sin(sensorAngle);
	int sensorCentreX = (int)round(agent.x + sensorDirX * SENSOR_RANGE);
	int sensorCentreY = (int)round(agent.y + sensorDirY * SENSOR_RANGE);
	int offsetX, offsetY, posX, posY, pos;
	float sum = 0.0f;

	if (SENSOR_SIZE == 0) {
		if (bind)
			Boundary(sensorCentreX, sensorCentreY);
		
		pos = (sensorCentreY) * (WIDTH) + (sensorCentreX); //Convert from 2D array to 1D array
		if (pos >= WIDTH * HEIGHT) pos -= (WIDTH+1);

		sum += canvas[pos];
	} else {
		for (offsetX = -SENSOR_SIZE; offsetX <= SENSOR_SIZE; offsetX++) { //Goes from -SENSOR_SIZE to SENSOR_SIZE
			for (offsetY = -SENSOR_SIZE; offsetY <= SENSOR_SIZE; offsetY++) {
				posX = sensorCentreX + offsetX;
				posY = sensorCentreY + offsetY;
				if (bind)
					Boundary(posX, posY); //resetting posX and posY to be inside the bounds
				
				pos = (posY) * (WIDTH) + (posX); //Convert from 2D array to 1D array
				if (pos >= WIDTH * HEIGHT) pos -= (WIDTH+1);
				//if (pos >= WIDTH * HEIGHT || pos <= 0) printf("posX: %d, posY: %d, pos: %d\n%d\n\n", posX, posY, pos, WIDTH*HEIGHT);

				sum += canvas[pos];
			}
		}
	}
	

	return sum;
}

void movement(glm::vec3 &agent /*, int index, int number*/)
{
	float forward = sense(0.0f, agent);
	float left = sense(SA, agent);
	float right = sense(-SA, agent);

	if (forward >= left && forward >= right) {
		;
	}

	else if (forward < left && forward < right) {
		agent.z += uniform(-RA, RA);
	}

	else if (left > right) {
		agent.z += RA;
	}

	else if (left < right) {
		agent.z += -RA;
	}

	//moving
	agent.x += SPEED * cos(agent.z);
	agent.y += SPEED * sin(agent.z);
	if (agent.x < 0 || agent.x > WIDTH - 1 || agent.y < 0 || agent.y > HEIGHT - 1) {
		if (BOUNDARY == true) {
			agent.z = uniform(-(float)M_PI, (float)M_PI);
		}
		Boundary(agent.x, agent.y);
	}
}

void vec_movement(std::vector<glm::vec3> &agents)
{
	float forward, left, right;
	int sensor = SENSOR_RANGE + SENSOR_SIZE;
	for (glm::vec3 &agent : agents) {
		//true if true, flase if false
		bind = (agent.x - sensor < 0 || agent.x + sensor > WIDTH -1 || agent.y - sensor < 0 || agent.y + sensor > HEIGHT - 1);

		forward = sense(0.0f, agent);
		left = sense(SA, agent);
		right = sense(-SA, agent);


		// Do nothing if forward >= left and right

		if (forward < left && forward < right) {
			agent.z += uniform(-RA, RA);
		}

		else if (left > right) {
			agent.z += RA;
		}

		else if (left < right) {
			agent.z += -RA;
		}
		
		//moving
		agent.x += SPEED * cos(agent.z);
		agent.y += SPEED * sin(agent.z);
		if (agent.x < 0 || agent.x > WIDTH - 1 || agent.y < 0 || agent.y > HEIGHT - 1) {
			if (BOUNDARY == true) {
				agent.z = uniform(-(float)M_PI, (float)M_PI);
			}
			Boundary(agent.x, agent.y);
		}
	}
}

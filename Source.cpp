#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
const int numSegments = 50;  // or any other constant value
float radius = 1.0f;  // Adjust the value based on your needs
float height = 2.0f;  // Adjust the value based on your needs
float vertices[8 * numSegments];

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(2.0f, 1.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;



int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Building a  3D scene", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	};

	//Floor
	float floorVertices[] = {
		-37.5f, -22.5f, -37.5f,   0.0f,  1.0f, 0.0f,  0.0f, 0.0f,
		 37.5f, -22.5f, -37.5f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
		 37.5f, -22.5f,  37.5f,   0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		 37.5f, -22.5f,  37.5f,   0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		-37.5f, -22.5f,  37.5f,   0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		-37.5f, -22.5f, -37.5f,   0.0f,  1.0f, 0.0f,  0.0f, 0.0f,
	};

	// Ceiling
	float ceilingVertices[] = {
		-37.5f, 5.0f, -37.5f,  0.0f, -1.0f, 0.0f,  0.0f,  0.0f,
		 37.5f, 5.0f, -37.5f,  0.0f, -1.0f, 0.0f,  1.0f,  0.0f,
		 37.5f, 5.0f,  37.5f,  0.0f, -1.0f, 0.0f,  1.0f,  1.0f,
		 37.5f, 5.0f,  37.5f,  0.0f, -1.0f, 0.0f,  1.0f,  1.0f,
		-37.5f, 5.0f,  37.5f,  0.0f, -1.0f, 0.0f,  0.0f,  1.0f,
		-37.5f, 5.0f, -37.5f,  0.0f, -1.0f, 0.0f,  0.0f,  0.0f,
	};

	// Wall 1 (vertical plane)
	float wallVertices[] = {
		-37.5f, -22.5f, -37.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.1f,
		-37.5f,  5.0f, -37.5f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		-37.5f,  5.0f,  37.5f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		-37.5f,  5.0f,  37.5f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		-37.5f, -22.5f,  37.5f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-37.5f, -22.5f, -37.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		// Wall 2 (vertical plane)
		37.5f, -22.5f, -37.5f,    -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		37.5f,  5.0f, -37.5f,    -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		37.5f,  5.0f,  37.5f,    -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		37.5f,  5.0f,  37.5f,    -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		37.5f, -22.5f,  37.5f,    -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		37.5f, -22.5f, -37.5f,    -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		// Wall 3 (horizontal plane connecting the end points of Wall 1 and Wall 2) - on the opposite side
		 37.5f,  5.0f, -37.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		-37.5f,  5.0f, -37.5f,    0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
		-37.5f, -22.5f, -37.5f,    0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
		-37.5f, -22.5f, -37.5f,    0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
		 37.5f, -22.5f, -37.5f,    0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
		 37.5f,  5.0f, -37.5f,    0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
	};
		
	
	
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};

	// Configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int floorVAO, floorVBO;
	unsigned int ceilingVAO, ceilingVBO;
	unsigned int wallVAO, wallVBO;

	// Set up buffers and vertex arrays for floor, ceiling, and walls
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glBindVertexArray(wallVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &ceilingVAO);
	glGenBuffers(1, &ceilingVBO);
	glBindVertexArray(ceilingVAO);
	glBindBuffer(GL_ARRAY_BUFFER, ceilingVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ceilingVertices), ceilingVertices, GL_STATIC_DRAW);
	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// Load textures
	unsigned int diffuseMap = loadTexture("floor_texture.jpg");
	unsigned int specularMap = loadTexture("whitelinen.png");
	unsigned int floorTexture = loadTexture("floor_texture.jpg");
	unsigned int wallTexture = loadTexture("wall_texture.jpg");
	unsigned int ceilingTexture = loadTexture("ceiling_texture.jpg");

	// --------------------
	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setInt("material.floorTexture", 2);
	lightingShader.setInt("material.wall_texture", 3);
	lightingShader.setInt("material.ceiling_texture", 4);


	




	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		/*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);


		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, floorTexture);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, wallTexture);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ceilingTexture);

	

		// Render containers
		glBindVertexArray(cubeVAO);



		for (unsigned int i = 0; i < 10; i++)
		{
			// Calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);

			// Adjust the positions and rotations to form a flat face
			float distanceBetweenCubes = 1.0f;
			model = glm::translate(model, glm::vec3(i * distanceBetweenCubes, 0.0f, 0.0f));
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, -15.0f, -30.0f));  // Adjust the position as needed
			model = glm::scale(model, glm::vec3(15.0f)); // Make it a couch sized cube
			
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
						
		}
		lightingShader.use();
		lightingShader.setMat4("model", glm::mat4(1.0f)); // Reset model matrix
		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Render ceiling
		lightingShader.setMat4("model", glm::mat4(1.0f));
		glBindVertexArray(ceilingVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Render walls
		lightingShader.setMat4("model", glm::mat4(1.0f));
		glBindVertexArray(wallVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Render the single light cube
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 5.0f));  // Adjust the position as needed
		model = glm::scale(model, glm::vec3(2.0f)); // Make it a smaller cube
		lightCubeShader.setMat4("model", model);
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 100);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVAO);
	
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);	
	
	glDeleteVertexArrays(1, &wallVAO);
	glDeleteBuffers(1, &wallVBO);

	glDeleteVertexArrays(1, &ceilingVAO);
	glDeleteBuffers(1, &ceilingVBO);

	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &lightCubeVAO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
// Function to load a texture using stb_image
unsigned int loadTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the texture wrapping/filtering options (optional)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

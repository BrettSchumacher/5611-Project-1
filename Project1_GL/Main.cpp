#include"Mesh.h"
#include"Model.h"
#include"SimulationManager.h"

//setup using pdf on canvas page for GLAD and openGL
//with assistance from https://www.youtube.com/watch?v=45MIykWJ-C4 for setting up glfw and getting mesh rendering to work

//globals
//bool fullscreen = false; 
//const unsigned int width = 1080; 
//const unsigned int height = 1920;

int main(int argc, char* argv[])
{
	//initialize glfw
	GLFWwindow* window;

	glfwInit();

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_SAMPLES, 32);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	//create glfw window, size 800x800
	window = glfwCreateWindow(mode->width, mode->height, "Project 1", glfwGetPrimaryMonitor(), NULL);
	//tell us if window fails to load
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//give window to current context
	glfwMakeContextCurrent(window);

	//initialize OpenGL
	gladLoadGL();	

	glViewport(0, 0, mode->width, mode->height);


	//create shader program loading in the default vertex and fragment shaders
	Shader shaderProgram("default.vert", "default.frag");


	glm::vec4 lightColor = 0.8f * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST);

	std::srand(time(NULL));


	
	//many collision logic tests
	SquareCollider test1 = SquareCollider(glm::vec3(1.0f, 0.0f, 0.0f));
	SquareCollider test2 = SquareCollider(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(2.0f, 5.0f), 29.0f);
	std::vector<SquareCollider> obsts;
	obsts.push_back(test2);
	HitInfo hit = HitInfo();
	CollisionLib::colliderBoxCast(obsts, test1, glm::vec2(1.0f, 0.0f), 100.0f, &hit);
	printf("BoxCast test (expect ~3.12): %f\n", hit.dist);
	hit = HitInfo();
	printf("Corner 1: (%f, %f)\n", test2.corner1.x, test2.corner1.y);
	printf("Corner 2: (%f, %f)\n", test2.corner2.x, test2.corner2.y);
	printf("Corner 3: (%f, %f)\n", test2.corner3.x, test2.corner3.y);
	printf("Corner 4: (%f, %f)\n", test2.corner4.x, test2.corner4.y);
	float dist = CollisionLib::distToLine(test2.corner2, test2.corner3, glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 0.0f));
	printf("RayCast test (expect ~3.12): %f\n", dist);
	hit = HitInfo();
	test1.Rotate(21.0f);
	test1.pos += glm::vec3(0.0f, 0.0f, 0.2f);
	hit = HitInfo();
	CollisionLib::colliderBoxCast(obsts, test1, glm::normalize(glm::vec2(3.0f, 1.0f)), 100.0f, &hit);
	printf("BoxCast test (expect ): %f\n", hit.dist);


	/*
	PriorityQueue testQueue = PriorityQueue();
	testQueue.Enqueue(new Node(test1, NULL, 100, 1));
	testQueue.Enqueue(new Node(test1, NULL, 1, 101));
	testQueue.Enqueue(new Node(test1, NULL, 1, 1));
	testQueue.Enqueue(new Node(test1, NULL, 1, 10));
	testQueue.Enqueue(new Node(test1, NULL, 200, 0));

	printf("weight: %f\n", testQueue.Dequeue()->weight);
	printf("weight: %f\n", testQueue.Dequeue()->weight);
	printf("weight: %f\n", testQueue.Dequeue()->weight);
	printf("weight: %f\n", testQueue.Dequeue()->weight);
	printf("weight: %f\n", testQueue.Dequeue()->weight);

	/*
	HitInfo hit;
	CollisionLib::colliderRayCast(test1, glm::vec2(5.0f, 0.2f), glm::vec2(-1.0f, 0.0f), 100.0f, &hit);
	printf("Raycast Test 1 (expect 4.5): %f\n", hit.dist);

	test1.Rotate(44.0f);
	hit = HitInfo();
	CollisionLib::colliderRayCast(test1, glm::vec2(5.0f, 0.0f), glm::vec2(-1.0f, 0.0f), 100.0f, &hit);
	printf("Raycast Test 2 (expect ~4.29): %f\n", hit.dist);

	test2.pos -= glm::vec3(2.0f, 0.0f, 0.0f);
	test2.Rotate(44.0f);
	hit = HitInfo();
	CollisionLib::colliderRayCast(test2, glm::vec2(5.0f, 0.0f), glm::vec2(-1.0f, 0.0f), 100.0f, &hit);
	printf("Raycast Test 3 (expect ~6.29): %f\n", hit.dist);
	
	printf("Collider Test 1 (expect 1): %d\n", CollisionLib::isColliderTouching(test1, test2));
	test2.theta = 45.0f;
	printf("Collider Test 2 (expect 1): %d\n", CollisionLib::isColliderTouching(test1, test2));
	test2.pos += glm::vec3(3.0f, 0.0f, 0.0f);
	printf("Collider Test 3 (expect 0): %d\n", CollisionLib::isColliderTouching(test1, test2));
	test1.theta = 30.0f;
	test1.pos += glm::vec3(2.0f, 0.0f, 0.0f);
	printf("Collider Test 4 (expect 1): %d\n", CollisionLib::isColliderTouching(test1, test2));

	glm::vec3 cornerOffset = glm::vec3(test1.bounds.x / 2.0f, 0.0f, test1.bounds.y / 2.0f);
	cornerOffset = glm::rotate(cornerOffset, glm::radians(test1.theta), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec2 corner1 = CollisionLib::vec3ToVec2(test1.pos + cornerOffset);
	glm::vec2 corner3 = CollisionLib::vec3ToVec2(test1.pos - cornerOffset);
	cornerOffset = glm::rotate(cornerOffset, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec2 corner2 = CollisionLib::vec3ToVec2(test1.pos + cornerOffset);
	glm::vec2 corner4 = CollisionLib::vec3ToVec2(test1.pos - cornerOffset);

	printf("Test Collider 1 corners: (%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", corner1.x, corner1.y, corner2.x, corner2.y, corner3.x, corner3.y, corner4.x, corner4.y);
	*/

	Camera camera(mode->width, mode->height, glm::vec3(0.0f, 15.0f, 40.0f), 18.f, 0.0f);
	Model agentModel = Model("models/agent/agent.gltf");
	std::vector<Model> obstModels;
	obstModels.push_back(Model("models/obstacle/obstacle.gltf"));
	obstModels.push_back(Model("models/obstacle/choral.gltf"));
	Model boidModel = Model("models/boid/boid.gltf");
	Model floorModel = Model("models/floor/floor.gltf");
	Model goalModel = Model("models/goal/goal.gltf");
	Model nodeModel = Model("models/node/node.gltf");
	Model connectModel = Model("models/connection/connection.gltf");
	SimulationManager sim(agentModel, obstModels, boidModel, floorModel, goalModel, nodeModel, connectModel);

	//time for the main game loop
	float t = glfwGetTime();
	float dt;

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			//close Application
			break;
		}

		//process events
		dt = glfwGetTime() - t;
		t = glfwGetTime();

		//prevent lag from ruining stuff
		if (dt > 0.05f)
		{
			dt = 0.05f;
		}

		//draw content
		glClearColor(0.07f, 0.19f, 0.28f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window, dt);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		sim.Update(window, dt);

		sim.Draw(shaderProgram, camera);

		glfwSwapBuffers(window); //double buffering

		glfwPollEvents();

	}

	//wrap things up
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
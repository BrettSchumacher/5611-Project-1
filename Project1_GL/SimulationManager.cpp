#include"SimulationManager.h"

SimulationManager::SimulationManager(Model agentModel, std::vector<Model> obstModels, Model boidModel, Model floorModel, Model goalModel, Model nodeModel, Model connectModel)
{
	SimulationManager::agentModel = agentModel;
	SimulationManager::obstModels = obstModels;
	SimulationManager::boidModel = boidModel;
	SimulationManager::floorModel = floorModel;
	SimulationManager::goalModel = goalModel;
	SimulationManager::nodeModel = nodeModel;
	SimulationManager::connectModel = connectModel;

	SimulationManager::floor = Floor(floorModel);
	ChangeState(SimState::PATH_FINDING);
}

void SimulationManager::Update(GLFWwindow* window, float dt)
{
	HandleInputs(window);

	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].Update(dt);
	}

	switch (state)
	{
	case SimState::PATH_FINDING:
		PathUpdate(dt);
		break;
	case SimState::TTC:
		TTCUpdate(dt);
		break;
	case SimState::COMBINED:
		CombinedUpdate(dt);
		break;
	default: //paused state
		PausedUpdate(dt);
	}
}

void SimulationManager::Draw(Shader& shaderProgram, Camera& camera)
{
	for (int i = 0; i < agents.size(); i++)
	{
		agents[i].Draw(shaderProgram, camera);
	}
	for (int i = 0; i < obsts.size(); i++)
	{
		obsts[i].Draw(shaderProgram, camera);
	}
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].Draw(shaderProgram, camera);
	}
	floor.Draw(shaderProgram, camera);

	if (!drawGraph)
	{
		return;
	}

	for (int i = 0; i < graph.size(); i++)
	{
		nodeModel.Draw(shaderProgram, camera, graph[i]->collider.pos, graph[i]->collider.rot, 0.5f * graph[i]->collider.scale);
	}

	Node* node1;
	Node* node2;
	glm::vec3 pos;
	glm::vec3 dir;
	float theta;
	//draw connections
	for (int i = 0; i < graph.size() - 1; i++)
	{
		node1 = graph[i];
		for (int j = 0; j < node1->neighbors.size(); j++)
		{
			node2 = node1->neighbors[j];
			//so we don't duplicate paths
			if (node2->ID > node1->ID)
			{
				pos = 0.5f * (node1->collider.pos + node2->collider.pos);
				dir = node2->collider.pos - node1->collider.pos;
				theta = acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), glm::normalize(dir)));
				if (dir.x < 0)
				{
					theta *= -1;
				}
				theta = glm::degrees(theta) + 90.0f;
				connectModel.Draw(shaderProgram, camera, pos, glm::quat(glm::vec3(0.0f, glm::radians(theta), 0.0f)), glm::vec3(glm::length(dir) / 2.0f, 1.0f, 0.1f));
			}
		}
	}
}

void SimulationManager::PathUpdate(float dt)
{
	for (int i = 0; i < agents.size(); i++)
	{
		agents[i].Update(dt);
	}
}

void SimulationManager::CombinedUpdate(float dt)
{

}

void SimulationManager::TTCUpdate(float dt)
{

}

void SimulationManager::PausedUpdate(float dt)
{

}

void SimulationManager::ChangeState(SimState newState)
{
	//check if it's just unpausing
	if (state == SimState::PAUSED && prevState == newState)
	{
		state = newState;
		return;
	}
	//if switching states
	else
	{
		switch (newState)
		{
		case SimState::PATH_FINDING:
			SetUpPathFinding();
			break;
		case SimState::TTC:
			break;
		case SimState::COMBINED:
			break;
		default: //pausing
			break;
		}
	}
	prevState = state;
	state = newState;
}

void SimulationManager::SetUpPathFinding()
{
	//for path finding it will be a single agent with obstacles
	//we start by clearing out any remaining things
	agents.clear();
	agentColliders.clear();
	boids.clear();
	boidColliders.clear();
	obsts.clear();
	obstColliders.clear();
	graph.clear();

	//place obstacles
	PlaceObstacles(numObstacles);

	//place agent
	PlaceAgents(numAgents);

	//printf("Generating graph\n");

	/*for (int i = 0; i < obstColliders.size(); i++)
	{
		printf("Obstacle %d: (%f, %f, %f), (%f, %f)\n", i, obstColliders[i].pos.x, obstColliders[i].pos.y, obstColliders[i].pos.z, obstColliders[i].bounds.x, obstColliders[i].bounds.y);
	}*/

	//generate visibility graph
	GenerateGraph();

	//printf("graph generated\n");

	//have agents find their paths
	for (int i = 0; i < agents.size(); i++)
	{
		agents[i].SetupGraph(graph);
		agents[i].GeneratePath();
	}

	/*for (int i = 0; i < obstColliders.size(); i++)
	{
		printf("Obstacle %d: (%f, %f, %f), (%f, %f)\n", i, obstColliders[i].pos.x, obstColliders[i].pos.y, obstColliders[i].pos.z, obstColliders[i].bounds.x, obstColliders[i].bounds.y);
	}*/
}

void SimulationManager::HandleInputs(GLFWwindow* window)
{
	//handle input downs
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !p_pressed)
	{
		//p to pause/unpause
		if (state == SimState::PAUSED)
		{
			ChangeState(prevState);
		}
		else
		{
			ChangeState(SimState::PAUSED);
		}
		p_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !r_pressed)
	{
		//press r to reset current sim, or if paused reset sim and unpause sim
		if (state == SimState::PAUSED)
		{
			ChangeState(prevState);
		}
		else
		{
			ChangeState(state);
		}
		r_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !one_pressed)
	{
		//press 1 to start path finding sim if not in a path finding sim
		if (state != SimState::PATH_FINDING)
		{
			ChangeState(SimState::PATH_FINDING);
		}
		one_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !two_pressed)
	{
		//2 to start crowd sim using TTC avoidance
		if (state != SimState::TTC)
		{
			ChangeState(SimState::TTC);
		}
		two_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !g_pressed)
	{
		//toggle graph drawing
		drawGraph = !drawGraph;
		g_pressed = true;
	}

	//handle releases
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		p_pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
	{
		r_pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
	{
		one_pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
	{
		two_pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		g_pressed = false;
	}
}

void SimulationManager::SetUpTTC()
{

}

void SimulationManager::SetUpCombined()
{

}

void SimulationManager::PlaceObstacles(int num)
{
	float x;
	float y;
	float length;
	float width;
	float theta;
	Obstacle obst;
	SquareCollider collider;
	int numPlaced = 0;

	//place obstacles
	for (int i = 0; i < num; i++)
	{
		length = 1.5f; //RandFloat(1.0f, 1.5f); //rand length between 1.0 and 1.5
		width = 1.5f; // RandFloat(1.0f, 3.0f);
		theta = RandFloat(0.0f, 180.0f);

		obst = Obstacle(&obstModels[rand() % obstModels.size()]);
		collider = FindValidCollider(defaultObstBounds, glm::vec3(length, 1.5f, width), theta);
		obst.collider = collider;
		obsts.push_back(obst);
		obstColliders.push_back(collider);

	}
}

void SimulationManager::PlaceAgents(int num)
{
	float x;
	float y;
	Agent agent;
	SquareCollider collider;
	int numPlaced = 0;

	for (int i = 0; i < num; i++)
	{
		//use obstacle to test if agent fits there cuz i already have a obst collision script
		agent = Agent(&agentModel, &goalModel);
		collider = FindValidCollider(defaultAgentBounds, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
		agent.goal = FindValidCollider(defaultAgentBounds, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
		agent.goal.SetBounds(defaultAgentBounds / 2.0f);
		agent.collider.SetBounds(defaultAgentBounds / 2.0f);
		agent.obstColliders = &obstColliders;

		agent.collider = collider;
		agents.push_back(agent);
		agentColliders.push_back(collider);

	}
}

float SimulationManager::RandFloat(float min, float max)
{
	return (float(std::rand()) / float(RAND_MAX)) * (max - min) + min;
}

SquareCollider SimulationManager::FindValidCollider(glm::vec2 defaultBounds, glm::vec3 scale, float theta)
{
	float x;
	float y;
	SquareCollider* temp;
	glm::vec2 bounds = glm::vec2(defaultBounds.x * scale.x, defaultBounds.y * scale.z);
	int maxAttempts = 20;

	while (true)
	{
		x = RandFloat(-floor.bounds.x / 2.0f, floor.bounds.x / 2.0f);
		y = RandFloat(-floor.bounds.y / 2.0f, floor.bounds.y / 2.0f);
		temp = new SquareCollider(glm::vec3(x, 0.0f, y), scale, bounds * 1.1f, theta);
		if (!CollisionLib::isColliderTouchingList(obstColliders, *temp))
		{
			temp->SetBounds(temp->GetBounds() / 1.1f);
			return *temp;
		}
		maxAttempts--;
	}

	//temp->scale = glm::vec3(0.0f, 0.0f, 0.0f);

	return *temp;
}

void SimulationManager::GenerateGraph()
{
	SquareCollider* obst;
	SquareCollider* tempNode;
	Node* node;
	glm::vec3 cornerOffset;
	glm::vec3 center;
	glm::vec3 option1;
	glm::vec3 option2;
	glm::vec3 option3;
	glm::vec3 option4;
	glm::quat rot;
	float theta;
	float bound = fminf(defaultAgentBounds.x, defaultAgentBounds.y) / 1.5f;
	Node* node1;
	Node* node2;
	Node* node3;
	Node* node4;
	HitInfo hit;
	glm::vec2 dir;

	for (int i = 0; i < obstColliders.size(); i ++)
	{
		obst = &obstColliders[i];
		center = obst->pos;
		//make sure to account for agent size when generating new node
		cornerOffset = glm::vec3(obst->GetBounds().x / 2.0f + bound, 0.0f, obst->GetBounds().y / 2.0f + bound);
		cornerOffset = glm::rotate(cornerOffset, glm::radians(obst->GetTheta()), glm::vec3(0.0f, 1.0f, 0.0f));
		node1 = NULL;
		node2 = NULL;
		node3 = NULL;
		node4 = NULL;

		//test option 1
		option1 = obst->pos + cornerOffset;
		//try a node at the corner
		tempNode = new SquareCollider(option1, glm::vec3(1.0f, 1.0f, 1.0f), defaultAgentBounds, 0.0f);
		//see if this is a valid node
		if (IsValidNodePos(tempNode))
		{
			//printf("Found valid node: (%f, %f, %f)\n", tempNode->pos.x, tempNode->pos.y, tempNode->pos.z);
			tempNode->SetBounds(defaultAgentBounds / 2.0f);
			node = new Node(*tempNode);
			node->ID = graph.size();
			node->parentObst = obst;
			node1 = node;
			graph.push_back(node);
		}

		option3 = obst->pos - cornerOffset;
		tempNode = new SquareCollider(option3, glm::vec3(1.0f, 1.0f, 1.0f), defaultAgentBounds, 0.0f);
		if (IsValidNodePos(tempNode))
		{
			//printf("Found valid node: (%f, %f, %f)\n", tempNode->pos.x, tempNode->pos.y, tempNode->pos.z);
			tempNode->SetBounds(defaultAgentBounds / 2.0f);
			node = new Node(*tempNode);
			node->ID = graph.size();
			node->parentObst = obst;
			node3 = node;
			graph.push_back(node);
		}

		cornerOffset = glm::vec3(-obst->GetBounds().x / 2.0f - bound, 0.0f, obst->GetBounds().y / 2.0f + bound);
		cornerOffset = glm::rotate(cornerOffset, glm::radians(obst->GetTheta()), glm::vec3(0.0f, 1.0f, 0.0f));
		option2 = obst->pos + cornerOffset;
		tempNode = new SquareCollider(option2, glm::vec3(1.0f, 1.0f, 1.0f), defaultAgentBounds, 0.0f);
		if (IsValidNodePos(tempNode))
		{
			//printf("Found valid node: (%f, %f, %f)\n", tempNode->pos.x, tempNode->pos.y, tempNode->pos.z);
			tempNode->SetBounds(defaultAgentBounds / 2.0f);
			node = new Node(*tempNode);
			node->ID = graph.size();
			node->parentObst = obst;
			node2 = node;
			graph.push_back(node);
		}
		
		option4 = obst->pos - cornerOffset;
		tempNode = new SquareCollider(option4, glm::vec3(1.0f, 1.0f, 1.0f), defaultAgentBounds, 0.0f);
		if (IsValidNodePos(tempNode))
		{
			//printf("Found valid node: (%f, %f, %f)\n", tempNode->pos.x, tempNode->pos.y, tempNode->pos.z);
			tempNode->SetBounds(defaultAgentBounds / 2.0f);
			node = new Node(*tempNode);
			node->ID = graph.size();
			node->parentObst = obst;
			node4 = node;
			graph.push_back(node);
		}

		//connect adjacent sides if possible
		if (node1 != NULL && node2 != NULL)
		{
			hit = HitInfo();
			dir = CollisionLib::vec3ToVec2(node2->collider.pos - node1->collider.pos);
			CollisionLib::colliderBoxCast(obstColliders, node1->collider, glm::normalize(dir), glm::length(dir), &hit, obst);
			if (!hit.hit)
			{
				node1->neighbors.push_back(node2);
				node2->neighbors.push_back(node1);
			}

		}

		if (node2 != NULL && node3 != NULL)
		{
			hit = HitInfo();
			dir = CollisionLib::vec3ToVec2(node3->collider.pos - node2->collider.pos);
			CollisionLib::colliderBoxCast(obstColliders, node2->collider, glm::normalize(dir), glm::length(dir), &hit, obst);
			if (!hit.hit)
			{
				node2->neighbors.push_back(node3);
				node3->neighbors.push_back(node2);
			}
		}

		if (node3 != NULL && node4 != NULL)
		{
			hit = HitInfo();
			dir = CollisionLib::vec3ToVec2(node4->collider.pos - node3->collider.pos);
			CollisionLib::colliderBoxCast(obstColliders, node3->collider, glm::normalize(dir), glm::length(dir), &hit, obst);
			if (!hit.hit)
			{
				node3->neighbors.push_back(node4);
				node4->neighbors.push_back(node3);
			}
		}

		if (node4 != NULL && node1 != NULL)
		{
			hit = HitInfo();
			dir = CollisionLib::vec3ToVec2(node4->collider.pos - node1->collider.pos);
			CollisionLib::colliderBoxCast(obstColliders, node4->collider, glm::normalize(dir), glm::length(dir), &hit, obst);
			if (!hit.hit)
			{
				node4->neighbors.push_back(node1);
				node1->neighbors.push_back(node4);
			}
		}
	}

	
	for (int i = 0; i < graph.size(); i++)
	{
		for (int j = i + 1; j < graph.size(); j++)
		{
			if (graph[i]->parentObst == graph[j]->parentObst)
			{
				continue;
			}
			hit = HitInfo();
			dir = CollisionLib::vec3ToVec2(graph[j]->collider.pos - graph[i]->collider.pos);
			CollisionLib::colliderBoxCast(obstColliders, graph[i]->collider, glm::normalize(dir), glm::length(dir), &hit);
			if (!hit.hit)
			{
				graph[i]->neighbors.push_back(graph[j]);
				graph[j]->neighbors.push_back(graph[i]);
			}
		}
	}
}

bool SimulationManager::IsValidNodePos(SquareCollider* tempNode)
{
	return !CollisionLib::isColliderTouchingList(obstColliders, *tempNode);
}
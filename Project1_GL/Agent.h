#ifndef AGENT_CLASS_H
#define AGENT_CLASS_H

#include "PriorityQueue.h"
#include "CollisionLib.h"

class Agent
{
public:
	SquareCollider collider;
	SquareCollider goal;
	glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);
	float moveSpeed = 6.0f;
	float maxAcc = 30.0f;
	float turnSpeed = 250.0f;
	std::vector<Node*> graph;
	std::vector<glm::vec3> path;
	std::vector<SquareCollider>* obstColliders;

	Agent
	(
		Model* agentModel = NULL,
		Model* goalModel = NULL,
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec2 bounds = glm::vec2(1.0f, 1.0f)
	);

	void Update(float dt);
	void Draw(Shader& shaderProgram, Camera& camera);
	void GeneratePath();
	void SetupGraph(std::vector<Node*> nodes);

private:
	Model* agentModel;
	Model* goalModel;
	bool reachedGoal = false;
	bool foundPath = false;
	bool firstTurn = true;

	bool CanSeeGoal(Node* node);

};

#endif
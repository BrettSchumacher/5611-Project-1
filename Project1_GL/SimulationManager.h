#ifndef SIMULATIONMANAGER_CLASS_H
#define SIMULATIONMANAGER_CLASS_H

#include"CollisionLib.h"
#include"PriorityQueue.h"

enum class SimState
{
	PAUSED,
	PATH_FINDING,
	TTC,
	COMBINED,
	NONE
};

class SimulationManager
{
public:
	SimState state = SimState::PAUSED;
	SimState prevState = SimState::NONE;
	std::vector<Agent> agents;
	std::vector<SquareCollider> agentColliders;
	std::vector<Obstacle> obsts;
	std::vector<SquareCollider> obstColliders;
	std::vector<Boid> boids;
	std::vector<SquareCollider> boidColliders;

	Floor floor;
	int numObstacles = 20;
	int numAgents = 5;
	glm::vec2 defaultObstBounds = glm::vec2(2.2f, 2.2f);
	glm::vec2 defaultAgentBounds = glm::vec2(2.0f, 2.0f);

	SimulationManager(Model agentModel, std::vector<Model> obstModels, Model boidModel, Model floorModel, Model goalModel, Model nodeModel, Model connectModel);

	void Update(GLFWwindow* window, float dt);
	void Draw(Shader& shaderProgram, Camera& camera);
	void ChangeState(SimState newState);

private:
	Model agentModel;
	std::vector<Model> obstModels;
	Model boidModel;
	Model floorModel;
	Model goalModel;
	Model nodeModel;
	Model connectModel;

	bool p_pressed = false;
	bool r_pressed = false;
	bool g_pressed = false;
	bool one_pressed = false;
	bool two_pressed = false;

	bool drawGraph = false;

	std::vector<Node*> graph;

	void PausedUpdate(float dt);
	void PathUpdate(float dt);
	void TTCUpdate(float dt);
	void CombinedUpdate(float dt);
	void SetUpPathFinding();
	void SetUpTTC();
	void SetUpCombined();
	void HandleInputs(GLFWwindow *window);
	void PlaceObstacles(int num);
	void PlaceAgents(int num);
	void GenerateGraph();

	bool IsValidNodePos(SquareCollider* tempNode);

	SquareCollider FindValidCollider(glm::vec2 bounds, glm::vec3 scale, float theta);

	float RandFloat(float min, float max);
};

#endif
#ifndef OBSTACLE_CLASS_H
#define OBSTACLE_CLASS_H

#include"SquareCollider.h"

class Obstacle : public SquareCollider
{
public:
	SquareCollider collider;

	Obstacle(
		Model* obstacleModel = NULL,
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		float theta = 0.0f,
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec2 bounds = glm::vec2(1.0f, 1.0f)
	);

	void Draw(Shader& shaderProgram, Camera& camera);

private:
	Model* obstacleModel;
};

#endif

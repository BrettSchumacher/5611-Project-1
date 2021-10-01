#include"Obstacle.h"

Obstacle::Obstacle(Model* obstacleModel, glm::vec3 pos, float theta, glm::vec3 scale, glm::vec2 bounds)
{
	collider = SquareCollider(pos, scale, bounds, theta);

	Obstacle::obstacleModel = obstacleModel;
}

void Obstacle::Draw(Shader& shaderProgram, Camera& camera)
{
	if (obstacleModel != NULL)
	{
		obstacleModel->Draw(shaderProgram, camera, collider.pos, collider.rot, collider.scale);
	}
}
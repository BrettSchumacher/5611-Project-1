#ifndef BOID_CLASS_H
#define BOID_CLASS_H

#include"Model.h"

class Boid
{
public:
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;
	glm::vec3 vel;

	Boid(
		Model* boidModel = NULL,
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	);

	void Update(float dt);
	void Draw(Shader& shaderProgram, Camera& camera);

private:
	Model* boidModel;
};

#endif
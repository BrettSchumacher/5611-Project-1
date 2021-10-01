#ifndef FLOOR_CLASS_H
#define FLOOR_CLASS_H

#include"Model.h"

class Floor
{
public:
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;
	glm::vec2 bounds = glm::vec2(30.0f, 30.0f);

	Floor();

	Floor(
		Model floor,
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	);

	void Draw(Shader& shaderProgram, Camera& camera);

private:
	Model floorModel;
};

#endif
#ifndef SQUARECOLLIDER_CLASS_H
#define SQUARECOLLIDER_CLASS_H

#include "Model.h"

class SquareCollider
{
public: 
	glm::vec3 pos;
	glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale;
	glm::vec2 corner1;
	glm::vec2 corner2;
	glm::vec2 corner3;
	glm::vec2 corner4;


	SquareCollider
	(
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec2 bounds = glm::vec2(1.0f, 1.0f),
		float theta = 0.0f
	);

	void Rotate(float degrees);
	void SetTheta(float degrees);
	void SetBounds(glm::vec2 newBounds);

	float GetTheta();
	glm::vec2 GetBounds();

private:
	glm::vec2 vec3ToVec2(glm::vec3 vec);
	void CalculateCorners();

	float theta = 0.0f;
	glm::vec2 bounds;
};

#endif

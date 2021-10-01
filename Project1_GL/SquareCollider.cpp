#include"SquareCollider.h"

SquareCollider::SquareCollider(glm::vec3 pos, glm::vec3 scale, glm::vec2 bounds, float theta)
{
	SquareCollider::pos = pos;
	SquareCollider::rot = glm::quat(glm::vec3(0.0f, glm::radians(theta), 0.0f));
	SquareCollider::scale = scale;
	SquareCollider::bounds = bounds;
	SquareCollider::theta = theta;

	CalculateCorners();
}

void SquareCollider::Rotate(float degrees)
{
	SquareCollider::theta += degrees;
	SquareCollider::rot = glm::quat(glm::vec3(0.0f, glm::radians(theta), 0.0f));
	CalculateCorners();
}

void SquareCollider::SetTheta(float degrees)
{
	SquareCollider::theta = degrees;
	SquareCollider::rot = glm::quat(glm::vec3(0.0f, glm::radians(theta), 0.0f));
	CalculateCorners();
}

glm::vec2 SquareCollider::vec3ToVec2(glm::vec3 vec)
{
	return glm::vec2(vec.x, vec.z);
}

void SquareCollider::CalculateCorners()
{
	glm::vec2 cornerOffset = bounds / 2.0f;
	cornerOffset = glm::rotate(cornerOffset, glm::radians(theta));
	corner1 = vec3ToVec2(pos) + cornerOffset;
	corner3 = vec3ToVec2(pos) - cornerOffset;
	cornerOffset = glm::vec2(-bounds.x / 2.0f, bounds.y / 2.0f);
	cornerOffset = glm::rotate(cornerOffset, glm::radians(theta));
	corner2 = vec3ToVec2(pos) + cornerOffset;
	corner4 = vec3ToVec2(pos) - cornerOffset;
}

void SquareCollider::SetBounds(glm::vec2 newBounds)
{
	bounds = newBounds;
	CalculateCorners();
}

float SquareCollider::GetTheta()
{
	return theta;
}

glm::vec2 SquareCollider::GetBounds()
{
	return bounds;
}
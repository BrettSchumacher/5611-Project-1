#include"CollisionLib.h"

glm::vec2 CollisionLib::vec3ToVec2(glm::vec3 vec)
{
	return glm::vec2(vec.x, vec.z);
}

bool CollisionLib::inBounds(float low, float high, float val)
{
	return (val < high) && (val > low);
}

bool CollisionLib::inCollider(SquareCollider obst, glm::vec2 point)
{
	//put relative to obstacle
	glm::vec2 offset = point - vec3ToVec2(obst.pos);
	//rotate point around obstacle
	glm::vec2 rotatedPoint = glm::rotate(offset, glm::radians(-obst.GetTheta()));
	
 	bool withinX = glm::abs(rotatedPoint.x) <= obst.GetBounds().x / 2.0f;
	bool withinY = glm::abs(rotatedPoint.y) <= obst.GetBounds().y / 2.0f;
	return withinX && withinY;
}

bool CollisionLib::inColliderList(std::vector<SquareCollider> obstList, glm::vec2 point)
{
	for (int i = 0; i < obstList.size(); i++)
	{
		if (inCollider(obstList[i], point))
		{
			return true;
		}
	}
	return false;
}

float CollisionLib::distToCollider(SquareCollider obst, glm::vec2 point, glm::vec2 dir)
{
	/*
	float dist = distToLine(obst.corner1, obst.corner2, point, dir);
	float temp = distToLine(obst.corner2, obst.corner3, point, dir);

	//if dist is -1, go wtih temp, otherwise only go with temp if it's not -1, but less than dist
	if (dist < 0.0f || ((temp < dist) && temp > 0.0f))
	{
		dist = temp;
	}

	temp = distToLine(obst.corner3, obst.corner4, point, dir);
	if (dist < 0.0f || ((temp < dist) && temp > 0.0f))
	{
		dist = temp;
	}

	temp = distToLine(obst.corner4, obst.corner1, point, dir);
	if (dist < 0.0f || ((temp < dist) && temp > 0.0f))
	{
		dist = temp;
	}

	return dist;
	*/

	glm::vec2 relPoint = point - vec3ToVec2(obst.pos);
	relPoint = glm::rotate(relPoint, glm::radians(-obst.GetTheta()));
	glm::vec2 relDir = glm::rotate(dir, glm::radians(-obst.GetTheta()));

	float bestT = 999999.0f;
	float h = obst.GetBounds().y / 2.0f;
	float w = obst.GetBounds().x / 2.0f;

	//check bottom
	float t = (-h - relPoint.y) / relDir.y;
	if (inBounds(-w, w, relPoint.x + relDir.x * t) && t > 0.0f)
	{
		bestT = t;
	}

	//check top
	t += 2.0 * h / relDir.y;
	if (inBounds(-w, w, relPoint.x + relDir.x * t) && t > 0.0f && t < bestT)
	{
		bestT = t;
	}

	//check left
	t = (-w - relPoint.x) / relDir.x;
	if (inBounds(-h, h, relPoint.y + relDir.y * t) && t > 0.0f && t < bestT)
	{
		bestT = t;
	}

	t += 2.0 * w / relDir.x;
	if (inBounds(-h, h, relPoint.y + relDir.y * t) && t > 0.0f && t < bestT)
	{
		bestT = t;
	}

	if (bestT < 90000.0f)
	{
		return bestT;
	}

	return -1.0f;
}

float CollisionLib::distToLine(glm::vec2 p1, glm::vec2 p2, glm::vec2 point, glm::vec2 dir)
{
	glm::vec2 offset = point - p1;
	glm::vec2 lineDir = p1 - p2;

	float norm = -dir.x * lineDir.y + dir.y * lineDir.x;

	//distance along line seg to hit, normalized to [0,1] from p1 to p2
	float u = (dir.x * offset.y - dir.y * offset.x) / norm;

	//if outside [0,1] then hit is not along line segment
	if (u < 0.0f || u > 1.0f)
	{
		return -1.0f;
	}

	//distance along ray to collision, discard if < 0
	float t = (offset.x * lineDir.y - offset.y * lineDir.x) / norm;
	if (t > 0.0f)
	{
		return t;
	}
}

float CollisionLib::distToNearestCollider(std::vector<SquareCollider> obstList, glm::vec2 point, glm::vec2 dir)
{
	float dist = -1.0f;
	float temp;

	for (int i = 0; i < obstList.size(); i++)
	{
		temp = distToCollider(obstList[i], point, dir);
		if (temp > 0.0f && (temp < dist || dist < 0.0f))
		{
			dist = temp;
		}
	}

	return dist;
}

void CollisionLib::colliderRayCast(SquareCollider obst, glm::vec2 point, glm::vec2 dir, float max_dist, HitInfo* outHit)
{
	float dist = distToCollider(obst, point + 0.01f * dir, dir);
	//update if there's an actual hit, within max_dist, and closer than current Hit
	if (dist > 0.0f && dist < max_dist && (outHit->dist < 0.0f || dist < outHit->dist))
	{
		outHit->dist = dist;
		outHit->hit = true;
	}
}

void CollisionLib::colliderListRayCast(std::vector<SquareCollider> obstList, glm::vec2 point, glm::vec2 dir, float max_dist, HitInfo* outHit, SquareCollider* ignore)
{
	for (int i = 0; i < obstList.size(); i++)
	{
		if (ignore == NULL || glm::length(obstList[i].pos - ignore->pos) > 0.1)
		{
			colliderRayCast(obstList[i], point, dir, max_dist, outHit);
		}
	}
}

bool CollisionLib::isColliderTouching(SquareCollider obst1, SquareCollider obst2)
{
	//check if centers collide
	bool colliding = inCollider(obst2, vec3ToVec2(obst1.pos)) || inCollider(obst1, vec3ToVec2(obst2.pos));
	if (colliding)
	{
		return true;
	}

	colliding = inCollider(obst2, obst1.corner1) || inCollider(obst2, obst1.corner2) || inCollider(obst2, obst1.corner3) || inCollider(obst2, obst1.corner4);
	if (colliding)
	{
		return true;
	}

	return inCollider(obst1, obst2.corner1) || inCollider(obst1, obst2.corner2) || inCollider(obst1, obst2.corner3) || inCollider(obst1, obst2.corner4);
}

bool CollisionLib::isColliderTouchingList(std::vector<SquareCollider> obstList, SquareCollider obst)
{
	SquareCollider other;
	for (int i = 0; i < obstList.size(); i++)
	{
		other = obstList[i];
		if (isColliderTouching(obst, other))
		{
			return true;
		}
	}
	return false;
}

void CollisionLib::colliderBoxCast(std::vector<SquareCollider> obstList, SquareCollider box, glm::vec2 dir, float max_dist, HitInfo* outHit, SquareCollider* ignore)
{
	colliderListRayCast(obstList, box.corner1, dir, max_dist, outHit);
	colliderListRayCast(obstList, box.corner2, dir, max_dist, outHit);
	colliderListRayCast(obstList, box.corner3, dir, max_dist, outHit);
	colliderListRayCast(obstList, box.corner4, dir, max_dist, outHit);
	colliderListRayCast(obstList, vec3ToVec2(box.pos), dir, max_dist, outHit, ignore);
}
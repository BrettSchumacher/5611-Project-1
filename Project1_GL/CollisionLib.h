#ifndef COLLISIONLIB_CLASS_H
#define COLLISIONLIB_CLASS_H

#include"Model.h"
#include"Agent.h"
#include"Obstacle.h"
#include"Boid.h"
#include"Floor.h"

struct HitInfo
{
	bool hit = false;
	float dist = 9999999.0f;
};

class CollisionLib
{
public:
	static bool inCollider(SquareCollider obst, glm::vec2 point);
	static bool inColliderList(std::vector<SquareCollider> obstList, glm::vec2 point);
	static bool isColliderTouching(SquareCollider obst1, SquareCollider obst2);
	static bool isColliderTouchingList(std::vector<SquareCollider> obstList, SquareCollider obst);
	static void colliderRayCast(SquareCollider obst, glm::vec2 point, glm::vec2 dir, float maxDist, HitInfo* outHit);
	static void colliderListRayCast(std::vector<SquareCollider> obstList, glm::vec2 point, glm::vec2 dir, float maxDist, HitInfo* outHit, SquareCollider* ignore = NULL);
	static void colliderBoxCast(std::vector<SquareCollider> obstList, SquareCollider box, glm::vec2 dir, float maxDist, HitInfo* outHit, SquareCollider* ignore = NULL);
	static glm::vec2 vec3ToVec2(glm::vec3 vec);
	static float distToLine(glm::vec2 p1, glm::vec2 p2, glm::vec2 point, glm::vec2 dir);
	static float distToCollider(SquareCollider obst, glm::vec2 point, glm::vec2 dir);
	static float distToNearestCollider(std::vector<SquareCollider> obstList, glm::vec2 point, glm::vec2 dir);
	static bool inBounds(float low, float high, float val);
};


#endif
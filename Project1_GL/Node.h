#ifndef NODE_CLASS_H
#define NODE_CLASS_H

#include"SquareCollider.h"

enum class NodeType
{
	START,
	END,
	REGULAR
};

class Node
{
public:
	SquareCollider collider;
	Node* parent;
	float cost;
	float heuristic;
	float weight;
	bool seen = false;
	bool visited = false;
	SquareCollider* parentObst = NULL;
	NodeType type = NodeType::REGULAR;
	std::vector<Node*> neighbors;
	int ID;

	Node(SquareCollider collider = SquareCollider(), Node* parent = NULL, float cost = -1.0f, float heuristic = -1.0f);
	void SetWeight(float cost, float heuristic);
};

#endif

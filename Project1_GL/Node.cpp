#include"Node.h"

Node::Node(SquareCollider collider, Node* parent, float cost, float heuristic)
{
	Node::collider = collider;
	Node::parent = parent;
	Node::cost = cost;
	Node::heuristic = heuristic;
	Node::weight = cost + heuristic;
}

void Node::SetWeight(float cost, float heuristic)
{
	Node::cost = cost;
	Node::heuristic = heuristic;
	Node::weight = cost + heuristic;
}
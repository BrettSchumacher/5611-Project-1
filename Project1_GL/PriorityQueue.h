#ifndef PRIORITYQUEUE_CLASS_H
#define PRIORITYQUEUE_CLASS_H

#include"Node.h"

class PriorityQueue
{
public:
	void Enqueue(Node* newNode);
	Node* Dequeue();
	void Remove(Node* toRemove);
	int getSize();

private:
	std::vector<Node*> queue = std::vector<Node*>();
	int Parent(int ind);
	int Left(int ind);
	int Right(int ind);
	void FixQueue(int ind);
};

#endif 

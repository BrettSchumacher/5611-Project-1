#include"PriorityQueue.h"

int PriorityQueue::Parent(int ind)
{
	return int(floor((ind - 1) / 2));
}

int PriorityQueue::Left(int ind)
{
	return 2 * ind + 1;
}

int PriorityQueue::Right(int ind)
{
	return 2 * ind + 2;
}

void PriorityQueue::Enqueue(Node* newNode)
{
	queue.push_back(newNode);
	int i = queue.size() - 1;
	Node* temp;
	while (i > 0 && queue[Parent(i)]->weight > queue[i]->weight)
	{
		temp = queue[Parent(i)];
		queue[Parent(i)] = newNode;
		queue[i] = temp;
		i = Parent(i);
	}
}

Node* PriorityQueue::Dequeue()
{
	Node* max = queue[0];
	queue[0] = queue[queue.size() - 1];
	queue.erase(queue.begin() + queue.size() - 1);
	FixQueue(0);

	return max;
}

void PriorityQueue::Remove(Node* toRemove)
{
	int ind = -1;
	for (int i = 0; i < queue.size(); i++)
	{
		if (queue[i] == toRemove)
		{
			ind = i;
			break;
		}
	}

	if (ind > -1)
	{
		queue[ind] = queue[queue.size() - 1];
		queue.pop_back();
		FixQueue(ind);
	}
}

void PriorityQueue::FixQueue(int ind)
{
	int size = queue.size();
	int left = Left(ind);
	int right = Right(ind);
	int largest;
	Node* temp;

	if (left < size && queue[left]->weight < queue[ind]->weight)
	{
		largest = left;
	}
	else
	{
		largest = ind;
	}

	if (right < size && queue[right]->weight < queue[largest]->weight)
	{
		largest = right;
	}

	if (largest != ind)
	{
		temp = queue[largest];
		queue[largest] = queue[ind];
		queue[ind] = temp;
		FixQueue(largest);
	}
}

int PriorityQueue::getSize()
{
	return queue.size();
}
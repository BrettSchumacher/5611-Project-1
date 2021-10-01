#include"Agent.h"

Agent::Agent(Model *agentModel, Model* goalModel, glm::vec3 pos, glm::vec3 scale, glm::vec2 bounds)
{
	Agent::collider = SquareCollider(pos, scale, bounds, 0.0f);
	Agent::vel = glm::vec3();
	Agent::goalModel = goalModel;

	Agent::agentModel = agentModel;
}

void Agent::Update(float dt)
{
	if (!foundPath || reachedGoal)
	{
		return;
	}


	//HitInfo hit = HitInfo();
	//glm::vec3 to;
	////see if we can see any of the other nodes (not including the next one)
	//for (int i = path.size() - 1; i > 0; i--)
	//{
	//	to = path[i] - collider.pos;
	//	CollisionLib::colliderBoxCast(*obstColliders, collider, glm::normalize(to), glm::length(to), &hit);
	//	if (hit.hit)
	//	{
	//		path.erase(path.begin(), path.begin() + i - 1);
	//		break;
	//	}
	//}

	glm::vec3 currGoal = path[0];
	glm::vec3 desiredVel = currGoal - collider.pos;

	//check if hit current goal
	if (glm::length(desiredVel) < 0.05f)
	{
		//printf("Reached node (%f, %f, %f)\n", currGoal.x, currGoal.y, currGoal.z);
		path.erase(path.begin());
		if (path.size() < 1)
		{
			reachedGoal = true;
			return;
		}
		currGoal = path[0];
		desiredVel = currGoal - collider.pos;
		//printf("New node: (%f, %f, %f)\n", currGoal.x, currGoal.y, currGoal.z);
	}

	vel = moveSpeed * glm::normalize(desiredVel);
	collider.pos += vel * dt;
	//check if passed the goal
	if (glm::dot(currGoal - collider.pos, desiredVel) < 0)
	{
		collider.pos = currGoal;
	}
	//printf("New pos: (%f, %f, %f)\n", collider.pos.x, collider.pos.y, collider.pos.z);


	//now set rotation
	float theta = glm::acos(glm::dot(glm::normalize(vel), glm::vec3(0.0f, 0.0f, 1.0f)));
	//make sure it's not neg
	if (vel.x < 0)
	{
		theta *= -1.0f;
	}
	theta = glm::degrees(theta) + 90.0f;
	if (firstTurn)
	{
		collider.SetTheta(theta);
		firstTurn = false;
	}
	else
	{
		float dtheta = theta - collider.GetTheta();
		float change;
		while (abs(dtheta) > 180)
		{
			dtheta -= 360 * glm::sign(dtheta);
		}
		change = turnSpeed * glm::sign(dtheta) * dt;

		if (abs(change) > abs(dtheta))
		{
			change = dtheta;
		}

		collider.Rotate(change);
	}
}

void Agent::Draw(Shader& shaderProgram, Camera& camera)
{
	agentModel->Draw(shaderProgram, camera, collider.pos, collider.rot, collider.scale);

	if (!reachedGoal)
	{
		goalModel->Draw(shaderProgram, camera, goal.pos, goal.rot, goal.scale);
	}
}

void Agent::GeneratePath()
{
	//printf("Agent pos: (%f, %f, %f)\n", collider.pos.x, collider.pos.y, collider.pos.z);
	//printf("Goal pos: (%f, %f, %f)\n", goal.pos.x, goal.pos.y, goal.pos.z);
	PriorityQueue queue = PriorityQueue();
	firstTurn = true;

	//add start and end to graph
	Node* start = new Node(collider);
	start->type = NodeType::START;
	start->ID = graph.size();
	graph.push_back(start);
	Node* end = new Node(goal);
	end->type = NodeType::END;
	end->ID = graph.size();
	graph.push_back(end);

	//see if start and end see eachother
	HitInfo hit = HitInfo();
	glm::vec2 dir = CollisionLib::vec3ToVec2(end->collider.pos - start->collider.pos);
	CollisionLib::colliderBoxCast(*obstColliders, start->collider, glm::normalize(dir), glm::length(dir), &hit);

	if (!hit.hit)
	{
		path.push_back(start->collider.pos);
		path.push_back(end->collider.pos);
		//printf("2EZ\n");
		foundPath = true;
		return;
	}

	//now hook start and goal up to list
	for (int i = 0; i < graph.size(); i++)
	{
		//fist see if start can see node
		hit = HitInfo();
		dir = CollisionLib::vec3ToVec2(graph[i]->collider.pos - start->collider.pos);
		CollisionLib::colliderBoxCast(*obstColliders, start->collider, glm::normalize(dir), glm::length(dir), &hit);
		if (!hit.hit)
		{
			start->neighbors.push_back(graph[i]);
		}

		//now see if end can
		hit = HitInfo();
		dir = CollisionLib::vec3ToVec2(graph[i]->collider.pos - end->collider.pos);
		CollisionLib::colliderBoxCast(*obstColliders, end->collider, glm::normalize(dir), glm::length(dir), &hit);
		if (!hit.hit)
		{
			graph[i]->neighbors.push_back(end);
		}
	}

	Node* curr = start;

	start->SetWeight(0.0f, glm::length(start->collider.pos - end->collider.pos));
	queue.Enqueue(start);

	Node* next;
	//perform A* going through priority queue
	while (queue.getSize() > 0)
	{
		curr = queue.Dequeue();
		curr->visited = true;
		if (curr->type == NodeType::END)
		{
			break;
		}
		for (int i = 0; i < curr->neighbors.size(); i++)
		{
			next = curr->neighbors[i];
			if (next->visited)
			{
				continue;
			}
			float dist = glm::length(next->collider.pos - curr->collider.pos);
			float toEnd = glm::length(next->collider.pos - end->collider.pos);
			float weight = curr->cost + dist + toEnd;
			if (weight < next->weight || !next->seen)
			{
				next->parent = curr;
				next->SetWeight(curr->cost + dist, toEnd);
				next->seen = true;
				queue.Remove(next);
				queue.Enqueue(next);
			}
		}
	}

	if (curr->type != NodeType::END)
	{
		printf("Darnnit\n");
		return;
	}
	
	//walk back through path
	//start by setting ends parent to the best path found
	//printf("Path:\n");
	while (curr != NULL)
	{
		//printf("(%f, %f, %f)\n", curr->collider.pos.x, curr->collider.pos.y, curr->collider.pos.z);
		path.insert(path.begin(), curr->collider.pos);
		curr = curr->parent;
	}

	foundPath = true;
}

bool Agent::CanSeeGoal(Node* node)
{
	for (int i = 0; i < node->neighbors.size(); i++)
	{
		if (node->neighbors[i]->type == NodeType::END)
		{
			return true;
		}
	}

	return false;
}

void Agent::SetupGraph(std::vector<Node*> nodes)
{
	//copy over nodes
	Node* temp;
	for (int i = 0; i < nodes.size(); i++)
	{
		temp = new Node(nodes[i]->collider);
		graph.push_back(temp);
	}

	//copy over neighbors
	for (int i = 0; i < nodes.size(); i++)
	{
		for (int j = 0; j < nodes[i]->neighbors.size(); j++)
		{
			graph[i]->neighbors.push_back(graph[nodes[i]->neighbors[j]->ID]);
		}
	}
}
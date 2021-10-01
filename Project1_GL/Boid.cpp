#include"Boid.h"

Boid::Boid(Model *boidModel, glm::vec3 pos, glm::quat rot, glm::vec3 scale)
{
	Boid::pos = pos;
	Boid::rot = rot;
	Boid::scale = scale;
	Boid::vel = glm::vec3();

	Boid::boidModel = boidModel;
}

void Boid::Update(float dt)
{

}

void Boid::Draw(Shader& shaderProgram, Camera& camera)
{
	if (boidModel != NULL)
	{
		boidModel->Draw(shaderProgram, camera, pos, rot, scale);
	}
}
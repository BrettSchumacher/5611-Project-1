#include"Floor.h"

Floor::Floor()
{

}

Floor::Floor(Model floorModel, glm::vec3 pos, glm::quat rot, glm::vec3 scale)
{
	Floor::pos = pos;
	Floor::rot = rot;
	Floor::scale = glm::vec3(bounds.x / 2.0f + 5.0f, 1.0f, bounds.y / 2.0f + 5.0f);

	Floor::floorModel = floorModel;
}

void Floor::Draw(Shader& shaderProgram, Camera& camera){
	floorModel.Draw(shaderProgram, camera, pos, rot, scale);
}
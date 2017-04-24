#include "Scene.h"

Scene::Scene()
{
	ambientLight = glm::vec3(0.13f);
}

void Scene::AddObject(Mesh &object) {
	objects.push_back(&object);
}

void Scene::AddLight(Light &light) {
	switch (light.GetType()){
	case 0:
		if (dirPoint.size() < num_max){
			lights.push_back(&light);
			dirPoint.push_back(light.GetPosition());
		}
		break;
	case 1:
		if (dirSpot.size() < num_max){
			lights.push_back(&light);
			dirSpot.push_back(light.GetPosition());
		}
		break;
	case 2:
		if (dirDirect.size() < num_max){
			lights.push_back(&light);
			dirDirect.push_back(light.GetPosition());
		}
	}
	
}

void Scene::AddCamera(Camera &camera) {
	cameras.push_back(&camera);
}

void Scene::Render(){
	for (int i = 0; i < objects.size(); i++){
		(*objects.at(i)).InitRender((*cameras.at(0)));
		//(*objects.at(i)).DefaultMeshRender();
		(*objects.at(i)).Render();

	}
}

void Scene::Animation(){
	//Cambio la matriz model
	static float angle = 0.0;
	angle += 0.001f;

	//cube1.Rotation(angle, glm::vec3(1.0f, 1.0f, 0));
	/*(*objects.at(1)).Orbit(angle, angle, glm::vec3(-3.0f, 0, 0));
	(*objects.at(2)).Orbit(angle, angle, glm::vec3(3.0f, 0, 0));
	(*objects.at(3)).Orbit(angle, angle, glm::vec3(0, 3.0f, 0));
	(*objects.at(4)).Orbit(angle, angle, glm::vec3(0, -3.0f, 0));*/
}

void Scene::Destroy()
{
	
}

Mesh Scene::getObject(int i){
	return (*objects.at(i));
}

Camera Scene::getCamera(int i){
	return (*cameras.at(i));
}
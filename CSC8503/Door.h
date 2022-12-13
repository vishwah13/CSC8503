#pragma once
#include "GameObject.h"

class GameManager;

 namespace NCL {
	 class MeshGeometry;
	 //class GameWorld;
	 namespace Rendering {
		 class ShaderBase;
		 class TextureBase;
	 }
	 namespace CSC8503 {
		 class GameWorld;

		 class Door :public GameObject {
		 public:
			 GameManager* gameManager;
			 GameWorld* gameWorld;
			 Door(GameManager* gameManager,GameWorld* gameWorld);
			 ~Door();

			 GameObject* AddDoorToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, NCL::MeshGeometry* cubeMesh, NCL::Rendering::ShaderBase* basicShader, NCL::Rendering::TextureBase* basicTex);

			 virtual void OnCollisionBegin(GameObject* otherObject);
			 virtual void OnCollisionEnd(GameObject* otherObject);

		 private:

		 };

	 }
};



#pragma once
#include "GameObject.h"

class GameManager;
class ScoreManager;

 namespace NCL {
	 class MeshGeometry;
	 //class GameWorld;
	 namespace Rendering {
		 class ShaderBase;
		 class TextureBase;
	 }
	 namespace CSC8503 {
		 class StateMachine;
		 class GameWorld;

		 class Door :public GameObject {
		 public:
			 GameManager* gameManager;
			 ScoreManager* scoreManager;
			 GameWorld* gameWorld;
			 StateMachine* stateMachine;

			 Door(GameManager* gameManager,GameWorld* gameWorld, ScoreManager* scoreManager);
			 ~Door();

			 GameObject* AddDoorToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, NCL::MeshGeometry* cubeMesh, NCL::Rendering::ShaderBase* basicShader, NCL::Rendering::TextureBase* basicTex);
			 void Update(float dt);
			 virtual void OnCollisionBegin(GameObject* otherObject);
			 virtual void OnCollisionEnd(GameObject* otherObject);

		 private:
			 bool bisLocked = false;
		 };

	 }
};



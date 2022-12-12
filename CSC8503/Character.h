#pragma once
#include "Vector3.h"
class ScoreManager;

namespace NCL {
	class MeshGeometry;
	//class GameWorld;
	namespace Rendering {
		class ShaderBase;
	}
	namespace CSC8503 {
		class GameObject;
		class Character : public GameObject {


		public:
			float forceToMove = 20;
			float damageForce = 50;
			float rotationSpeed = 2;
			float charFriction = 1.0f;
			float jumpForce = 200.f;
			bool bJump = false;
			float waitTime;

			ScoreManager* scoreManager;
			GameWorld* gameWorld;

			Character(ScoreManager* scoreManager, GameWorld* world);
			virtual GameObject* Init(string name,const  NCL::Maths::Vector3& position, MeshGeometry* charMesh, NCL::Rendering::ShaderBase* basicShader, GameWorld* world);
			virtual void Update(float dt, GameWorld* world);
			void Jump();

			virtual void OnCollisionBegin(GameObject* otherObject);
			virtual void OnCollisionEnd(GameObject* otherObject);

			void DestroyGameObject(GameTimer* timer, float timeToDestroy, GameObject* objectToDestroy);
			
			

		protected:
			float meshSize = 1.0f;
			float inverseMass = 0.5f;
			Vector3 lockedOffset = Vector3(0, 14, 20);
		};
	}
}
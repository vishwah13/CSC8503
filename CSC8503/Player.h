#pragma once
#include "Vector3.h"

namespace NCL {
	class MeshGeometry;
	class GameWorld;
	namespace Rendering {
		class ShaderBase;
	}
	namespace CSC8503 {
		class GameObject;
		class Player : public GameObject {


		public:
			float force = 0;

			Player();
			GameObject* PlayerInit(const  NCL::Maths::Vector3& position, MeshGeometry* charMesh, NCL::Rendering::ShaderBase* basicShader, GameWorld* world);
			void PlayerUpdate(float dt, GameWorld* world, GameObject* obj);

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;

		protected:
		};
	}
}
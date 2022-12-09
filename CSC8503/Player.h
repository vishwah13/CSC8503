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

		class Player {


		public:

			Player() {}
			GameObject* PlayerInit(const  NCL::Maths::Vector3& position, MeshGeometry* charMesh, NCL::Rendering::ShaderBase* basicShader, GameWorld* world);
			void PlayerUpdate(float dt);
		};
	}
}
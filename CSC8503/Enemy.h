#pragma once
#include "GameObject.h"

namespace NCL {

	class MeshGeometry;
	//class GameWorld;
	namespace Rendering {
		class ShaderBase;
	}
	namespace CSC8503 {
		class GameWorld;
		class NavigationGrid;
		class NavigationPath;
		class StateMachine;
		class Enemy : public GameObject
		{
		public:
			GameWorld* world;
			Enemy(GameWorld* world);
			GameObject* Init(std::string name, const  NCL::Maths::Vector3& position, MeshGeometry* charMesh, NCL::Rendering::ShaderBase* basicShader, NCL::CSC8503::GameWorld* world);
			void Update(float dt);

			StateMachine* stateMachine;
			void findPath(Transform target,float dt);
			void patrol();
			void chasePlayer(float dt);

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;

		protected:
			float distanceToPlayer = 0;
			Transform target;
			Vector3 currentPos;
			Vector3 nextPos;
			std::vector <Vector3> nodes;
			std::vector <Vector3> patrolPoints;
		};

	}
}


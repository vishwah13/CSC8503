#pragma once
#include "GameObject.h"
#include "BehaviourNode.h"

//class BehaviourState;
class BehaviourSelector;
class BehaviourSequence;

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
			void Update(float dt,Transform target, Transform powerUp);

			StateMachine* stateMachine;
			void findPath(Transform target);
			void patrol();
			void lookForBonus();
			void chaseTarget(float dt, Transform target);

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;

		protected:
			float timer = 0;
			float distanceToPlayer = 0;
			Transform target;
			Transform powerUpObj;
			Vector3 currentPos;
			Vector3 nextPos;
			std::vector <Vector3> nodes;
			std::vector <Vector3> patrolPoints;

			BehaviourState state;
			BehaviourSelector* selection;
			BehaviourSequence* sequence;
			BehaviourSequence* rootSequence;
		};

	}
}


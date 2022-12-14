#include "Enemy.h"
#include "AABBVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"
#include "NavigationGrid.h"
#include "NavigationPath.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;
using namespace Rendering;


NCL::CSC8503::Enemy::Enemy(GameWorld* world)
{
	this->world = world;
}

GameObject* NCL::CSC8503::Enemy::Init(std::string name, const NCL::Maths::Vector3& position, MeshGeometry* enemyMesh, NCL::Rendering::ShaderBase* basicShader, GameWorld* world)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;
	patrolPoints.push_back(Vector3(41,-17,15));
	patrolPoints.push_back(Vector3(34, -17, 43));
	patrolPoints.push_back(Vector3(81,-17,42));
	patrolPoints.push_back(Vector3(76,-17,10));

	//state Machine stuff
	stateMachine = new StateMachine();

	State* patrolState = new State([&](float dt)->void
		{
			std::cout << "I’m in Patrol state!\n" << std::endl;
			patrol();
		}
	);

	State* chaseState = new State([&](float dt)->void
		{
			std::cout << "I’m in chase state !\n" << std::endl;
			chasePlayer(dt);
		}
	);

	StateTransition* patrolToChase = new StateTransition(patrolState, chaseState, [&](void)->bool
		{
			return distanceToPlayer < 35.0f;
		}
	);
	StateTransition* chaseTopatrol = new StateTransition(chaseState, patrolState, [&](void)->bool
		{
			return distanceToPlayer > 35.0f;
		}
	);

	stateMachine->AddState(patrolState);
	stateMachine->AddState(chaseState);

	stateMachine->AddTransition(patrolToChase);
	stateMachine->AddTransition(chaseTopatrol);
	


	//normal gameObject stuff
	SetName(name);
	bTriggerDelete = true;

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	SetRenderObject(new RenderObject(&GetTransform(), enemyMesh, nullptr, basicShader));
	GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	physicsObject = new PhysicsObject(&GetTransform(), GetBoundingVolume());
	physicsObject->SetElasticity(0.0f);

	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitSphereInertia();

	GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(this);

	return this;
}

void NCL::CSC8503::Enemy::Update(float dt)
{
	stateMachine->Update(dt);
}

void NCL::CSC8503::Enemy::findPath(Transform target, float dt)
{
	NavigationGrid grid("TestGrid1.txt");
	NavigationPath outPath;

	Vector3 startPos(transform.GetPosition());
	Vector3 endPos(target.GetPosition());

	distanceToPlayer = (target.GetPosition() - transform.GetPosition()).Length();

	bool foundPath = grid.FindPath(startPos, endPos, outPath);


	while (outPath.PopWaypoint(currentPos))
	{
		nodes.push_back(currentPos);
	}
	
}

void NCL::CSC8503::Enemy::patrol()
{
	int randIndex = rand() % patrolPoints.size();
	Vector3 dir = patrolPoints[randIndex] - transform.GetPosition();
	physicsObject->AddForce(dir * 25);
}

void NCL::CSC8503::Enemy::chasePlayer(float dt)
{ 
	for (int i = 1; i < nodes.size(); ++i) {
		Vector3 a = nodes[i - 1];
		Vector3 b = nodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}

	Vector3 direction = nodes.back() - transform.GetPosition();

	if (direction.Length() > 0.1f) {

		direction.Normalise();
		physicsObject->AddForce(direction * 20);
	}	
}

void NCL::CSC8503::Enemy::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetName() == "Goaty") {

		world->RemoveGameObject(otherObject, false);
	}
}

void NCL::CSC8503::Enemy::OnCollisionEnd(GameObject* otherObject)
{
}



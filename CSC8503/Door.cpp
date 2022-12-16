#include "Door.h"
#include "AABBVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "MeshGeometry.h"
#include "GameManager.h"
#include "GameWorld.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "ScoreManager.h"


using namespace NCL;
using namespace CSC8503;
using namespace Rendering;

Door::Door(GameManager* gameManager, GameWorld* gameWorld,ScoreManager* scoreManager)
{
	this->gameManager = gameManager;
	this->scoreManager = scoreManager;
	this->gameWorld = gameWorld;
	this->bTriggerDelete = true;

	bisLocked = true;
}

Door::~Door()
{
	
}

GameObject* NCL::CSC8503::Door::AddDoorToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, NCL::MeshGeometry* cubeMesh, NCL::Rendering::ShaderBase* basicShader, NCL::Rendering::TextureBase* basicTex) {

	stateMachine = new StateMachine();

	State* LockState = new State([&](float dt)->void
		{
			renderObject->SetColour(Debug::MAGENTA);
			std::cout << "I’m in Locked state!\n" << std::endl;
		}
	);

	State* unLockedState = new State([&](float dt)->void
		{
			renderObject->SetColour(Debug::GREEN);
			std::cout << "I’m in UnLocked state!\n" << std::endl;
			gameWorld->RemoveGameObject(this, false);

			Debug::Print("You WON !!!", Vector2(15,50));
			Debug::Print("Your Score is: " + std::to_string(scoreManager->GetScore()), Vector2(15,60));

		}
	);

	StateTransition* unLockedToLocked = new StateTransition(unLockedState, LockState, [&](void)->bool
		{
			if (bisLocked)
				return true;
			else
				return false;
			
		}
	);
	StateTransition* lockedToUnLocked = new StateTransition(LockState, unLockedState, [&](void)->bool
		{
			if (!bisLocked)
				return true;
			else
				return false;
		}
	);

	stateMachine->AddState(LockState);
	stateMachine->AddState(unLockedState);

	stateMachine->AddTransition(unLockedToLocked);
	stateMachine->AddTransition(lockedToUnLocked);
	
	
	SetName("door");
	
	AABBVolume* volume = new AABBVolume(dimensions);
	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	SetRenderObject(new RenderObject(&GetTransform(), cubeMesh, basicTex, basicShader));
	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitCubeInertia();
	GetPhysicsObject()->SetElasticity(0.0f);
	gameWorld->AddGameObject(this);
	return this;
}

void NCL::CSC8503::Door::Update(float dt)
{
	stateMachine->Update(dt);
}

void Door::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetName() == "Goaty" && gameManager->bisKeyCollected) {
		bisLocked = false;
	}
}

void Door::OnCollisionEnd(GameObject* otherObject)
{

}

#include "Door.h"
#include "AABBVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "MeshGeometry.h"
#include "GameManager.h"
#include "GameWorld.h"

using namespace NCL;
using namespace CSC8503;
using namespace Rendering;

Door::Door(GameManager* gameManager, GameWorld* gameWorld)
{
	this->gameManager = gameManager;
	this->gameWorld = gameWorld;
	this->bTriggerDelete = true;
}

Door::~Door()
{
}

GameObject* NCL::CSC8503::Door::AddDoorToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, NCL::MeshGeometry* cubeMesh, NCL::Rendering::ShaderBase* basicShader, NCL::Rendering::TextureBase* basicTex) {

	
	SetName("door");
	
	AABBVolume* volume = new AABBVolume(dimensions);
	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	SetRenderObject(new RenderObject(&GetTransform(), cubeMesh, basicTex, basicShader));
	GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitCubeInertia();
	GetPhysicsObject()->SetElasticity(0.0f);
	gameWorld->AddGameObject(this);
	return this;
}

void Door::OnCollisionBegin(GameObject* otherObject)
{
	/*if (otherObject->GetName() == "Goaty" && gameManager->bisKeyCollected) {
		gameWorld->RemoveGameObject(this, true);
	}*/
}

void Door::OnCollisionEnd(GameObject* otherObject)
{

}

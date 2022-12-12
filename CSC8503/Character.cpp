#include "GameTechRenderer.h"
#include "GameWorld.h"
#include "SphereVolume.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "ShaderBase.h"
#include "Character.h"
#include "Maths.h"
#include "ScoreManager.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;
using namespace Rendering;

Character::Character(ScoreManager* scoreManager,GameWorld* world) {

	this->scoreManager = scoreManager;
	this->gameWorld = world;
}

GameObject* NCL::CSC8503::Character::Init(string name,const NCL::Maths::Vector3& position, NCL::MeshGeometry* charMesh, NCL::Rendering::ShaderBase* basicShader, NCL::CSC8503::GameWorld* world)
{
	forceToMove = 10.f;
	damageForce = 200.f;
	rotationSpeed = 5;
	charFriction = 1.0f;
	jumpForce = 500.0f;
	
	meshSize = 1.0f;
	inverseMass = 0.5f;

	SphereVolume* volume = new SphereVolume(1.0f);

	SetName(name);
	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	SetRenderObject(new RenderObject(&GetTransform(), charMesh, nullptr, basicShader));
	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
	GetPhysicsObject()->friction = charFriction;

	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitSphereInertia();

	GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(this);

	return this;
}

void Character::Update(float dt, NCL::CSC8503::GameWorld* world)
{
	if (this != nullptr) {
		Vector3 objPos = GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler();

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	Vector3 movement;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))movement.z = -1;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))movement.z = 1;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))movement.x = -1;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))movement.x = 1;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE) && bJump) Jump();
		
	float targetAngle = atan2(-movement.x, -movement.z);
	targetAngle = RadiansToDegrees(targetAngle) + world->GetMainCamera()->GetYaw();
	
	if (movement.Length() != 0) {
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(0, 0, -1) * forceToMove);
		Quaternion newRot = Quaternion::EulerAnglesToQuaternion(0, targetAngle, 0);
		GetTransform().SetOrientation(Quaternion::Slerp(GetTransform().GetOrientation(), newRot, rotationSpeed * dt));
	}
}

void NCL::CSC8503::Character::Jump()
{
	GetPhysicsObject()->AddForce(Vector3(0, 1, 0) * jumpForce);
}

void Character::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetPhysicsObject() && otherObject->GetName() != "floor" && Window::GetKeyboard()->KeyDown(KeyboardKeys::E)) {
		otherObject->GetPhysicsObject()->AddForce(otherObject->GetTransform().GetPosition() - transform.GetPosition() * damageForce);
		scoreManager->AddScore(5);

	}

	if (otherObject->GetName() == "floor") {
		bJump = true;
	}
}

void Character::OnCollisionEnd(GameObject* otherObject)
{
	if (otherObject->GetName() == "floor") {
		bJump = false;
	}
}

void NCL::CSC8503::Character::DestroyGameObject(GameTimer* timer, float timeToDestroy, GameObject* objectToDestroy)
{
	/*waitTime += timer->GetTimeDeltaSeconds;

	if (waitTime > timeToDestroy) {
		gameWorld->RemoveGameObject(objectToDestroy, true);
		waitTime = 0;
		std::cout << "Object Destroyed";
	}*/
}

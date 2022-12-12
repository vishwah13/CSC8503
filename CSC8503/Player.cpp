#include "GameTechRenderer.h"
#include "GameWorld.h"
#include "SphereVolume.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "ShaderBase.h"
#include "Player.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;
using namespace Rendering;

Player::Player() {

	float force = 20.f;
}

GameObject* NCL::CSC8503::Player::PlayerInit(const NCL::Maths::Vector3& position, NCL::MeshGeometry* charMesh, NCL::Rendering::ShaderBase* basicShader, NCL::CSC8503::GameWorld* world)
{
	
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume = new SphereVolume(1.0f);

	character->SetName("Goaty");
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(character);

	return character;

}

void Player::PlayerUpdate(float dt, NCL::CSC8503::GameWorld* world,GameObject* obj)
{
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0));

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		obj->GetPhysicsObject()->AddForce(fwdAxis * 20);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		obj->GetPhysicsObject()->AddForce(-fwdAxis * 20);
	}
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::LEFT)) {
		obj->GetTransform().SetOrientation(Quaternion::Slerp(obj->GetTransform().GetOrientation(), Quaternion(0.0f, 360.0f, 0.0f, 1.0f), 1000 * dt));
	}
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::RIGHT)) {
		obj->GetTransform().SetOrientation(Quaternion::Slerp(obj->GetTransform().GetOrientation(), Quaternion(0.0f, -360.0f, 0.0f, 1.0f), 1000 * dt));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		obj->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}

void NCL::CSC8503::Player::OnCollisionBegin(GameObject* otherObject)
{
	if (!otherObject->GetPhysicsObject()) {

		std::cout << "Player hit something !";
	}
}

void NCL::CSC8503::Player::OnCollisionEnd(GameObject* otherObject)
{
}

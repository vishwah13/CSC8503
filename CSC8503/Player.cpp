//#include "Player.h"
//#include "SphereVolume.h"
//#include "GameWorld.h"
//#include "PhysicsObject.h"
//#include "RenderObject.h"
//
//using namespace NCL;
//using namespace CSC8503;
//
//GameObject* NCL::CSC8503::Player::PlayerInit(Vector3 position, MeshGeometry* charMesh, ShaderBase* basicShader, GameWorld* world)
//{
//	float meshSize = 1.0f;
//	float inverseMass = 0.5f;
//
//	GameObject* character = new GameObject();
//	SphereVolume* volume = new SphereVolume(1.0f);
//
//	character->SetBoundingVolume((CollisionVolume*)volume);
//
//	character->GetTransform()
//		.SetScale(Vector3(meshSize, meshSize, meshSize))
//		.SetPosition(position);
//
//	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
//	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));
//
//	character->GetPhysicsObject()->SetInverseMass(inverseMass);
//	character->GetPhysicsObject()->InitSphereInertia();
//
//	character->GetPhysicsObject()->SetElasticity(0.0f);
//
//	world->AddGameObject(character);
//
//	return character;
//}
//
//void NCL::CSC8503::Player::PlayerUpdate(float dt)
//{
//}

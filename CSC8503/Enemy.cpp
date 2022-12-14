#include "Enemy.h"
#include "AABBVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"
#include "NavigationGrid.h"
#include "NavigationPath.h"

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

void NCL::CSC8503::Enemy::Update(float dt, GameWorld* world)
{

}

void NCL::CSC8503::Enemy::findPath(Transform target, float dt)
{
	NavigationGrid grid("TestGrid1.txt");
	NavigationPath outPath;

	Vector3 startPos(transform.GetPosition());
	Vector3 endPos(target.GetPosition());

	bool foundPath = grid.FindPath(startPos, endPos, outPath);

	while (outPath.PopWaypoint(currentPos))
	{
		nodes.push_back(currentPos);
	}	
	
}

void NCL::CSC8503::Enemy::move(float dt)
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



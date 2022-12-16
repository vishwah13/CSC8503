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

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;
using namespace Rendering;


NCL::CSC8503::Enemy::Enemy(GameWorld* world,bool bUseStateMachine)
{
	this->world = world;
	this->bUseStateMachine = bUseStateMachine;
	this->bGameOver = false;
}

GameObject* NCL::CSC8503::Enemy::Init(std::string name, const NCL::Maths::Vector3& position, MeshGeometry* enemyMesh, NCL::Rendering::ShaderBase* basicShader, GameWorld* world)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	patrolPoints.push_back(Vector3(41,-17,15));
	patrolPoints.push_back(Vector3(34, -17, 43));
	patrolPoints.push_back(Vector3(81,-17,42));
	patrolPoints.push_back(Vector3(76,-17,10));

	
	
	if (bUseStateMachine) {

		//state Machine stuff
		stateMachine = new StateMachine();

		State* patrolState = new State([&](float dt)->void
			{
				std::cout << "I’m in Patrol state!\n" << std::endl;
				renderObject->SetColour(Debug::BLUE);
				patrol();
			}
		);

		State* chaseState = new State([&](float dt)->void
			{
				std::cout << "I’m in chase state !\n" << std::endl;
				renderObject->SetColour(Debug::RED);
				chaseTarget(dt, target);
			}
		);

		State* pickUpPower = new State([&](float dt)->void
			{
				std::cout << "I’m in PickUpPower state!\n" << std::endl;
				renderObject->SetColour(Debug::YELLOW);
				chaseTarget(dt, powerUpObj);
			}
		);

		StateTransition* patrolToChase = new StateTransition(patrolState, chaseState, [&](void)->bool
			{
				return distanceToPlayer < 30.0f;
			}
		);
		StateTransition* chaseTopatrol = new StateTransition(chaseState, patrolState, [&](void)->bool
			{
				return distanceToPlayer > 30.0f;
			}
		);
		StateTransition* patrolToPickUpPower = new StateTransition(patrolState, pickUpPower, [&](void)->bool
			{
				//bool chances = (rand() % 100) < 90;
				return distanceToPlayer > 50.0f;
			}
		);
		StateTransition* PickUpPowerToPatrol = new StateTransition(pickUpPower, patrolState, [&](void)->bool
			{
				//bool chances = (rand() % 100) < 90;
				return distanceToPlayer < 50.0f;
			}
		);
		StateTransition* PickUpPowerToChase = new StateTransition(pickUpPower, chaseState, [&](void)->bool
			{
				//bool chances = (rand() % 100) < 90;
				return distanceToPlayer < 30.0f;
			}
		);

		stateMachine->AddState(patrolState);
		stateMachine->AddState(chaseState);
		stateMachine->AddState(pickUpPower);

		stateMachine->AddTransition(patrolToChase);
		stateMachine->AddTransition(chaseTopatrol);
		stateMachine->AddTransition(patrolToPickUpPower);
		stateMachine->AddTransition(PickUpPowerToPatrol);
		stateMachine->AddTransition(PickUpPowerToChase);
	}
	else {
		//Behaviour tree stuff
		BehaviourAction* findPowerUpAction = new BehaviourAction("Find power up",
			[&](float dt, BehaviourState state)->BehaviourState {
				if (state == Initialise) {
					std::cout << "Looking for a powerUp!\n";
					renderObject->SetColour(Debug::YELLOW);
					state = Ongoing;
				}
				else if (state == Ongoing) {
					std::cout << "I’m in PickUpPower state!\n" << std::endl;
					if (distanceToPlayer < 30.0f) {

						chaseTarget(dt, powerUpObj);
					}

				}
				return state;
			}
		);

		BehaviourAction* patrolAction = new BehaviourAction("patrol",
			[&](float dt, BehaviourState state)->BehaviourState {
				if (state == Initialise) {
					std::cout << "started patrolling!\n";
					renderObject->SetColour(Debug::BLUE);
					state = Ongoing;
				}
				else if (state == Ongoing) {
					std::cout << "I’m patroling!\n" << std::endl;
					patrol();
					if (distanceToPlayer < 30.0f) {

						std::cout << "Bahaviour tree patrol success" << std::endl;
						return Success;
					}

				}
				return state; //will be ’ongoing ’ until success
			}
		);

		BehaviourAction* chaseAction = new BehaviourAction("Find power up",
			[&](float dt, BehaviourState state)->BehaviourState {
				if (state == Initialise) {
					std::cout << "Started chasing target!\n";
					renderObject->SetColour(Debug::RED);
					state = Ongoing;
				}
				else if (state == Ongoing) {
					std::cout << "I’m chase target!\n" << std::endl;
					chaseTarget(dt, target);
					if (distanceToPlayer < 2.0f) {
						std::cout << "Bahaviour tree chase success" << std::endl;
						return Success;
					}
				}
				return state; //will be ’ongoing ’ until success
			}
		);

		sequence = new BehaviourSequence("Sequence");
		sequence->AddChild(patrolAction);
		sequence->AddChild(chaseAction);


		selection = new BehaviourSelector("Selection");
		selection->AddChild(findPowerUpAction);

		rootSequence = new BehaviourSequence("Root Sequence");
		rootSequence->AddChild(sequence);
		rootSequence->AddChild(selection);

		state = Ongoing;
		std::cout << "We’re going on an adventure !\n";
	}

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

void NCL::CSC8503::Enemy::Update(float dt,Transform target,Transform powerUp)
{
	if (bUseStateMachine) {
		stateMachine->Update(dt);
	}
	else {
		state = rootSequence->Execute(dt);


		if (state == Success) {
			std::cout << "Got the goat success!\n";
		}
		else if (state == Failure) {
			std::cout << "Goat Escaped!\n";
			rootSequence->Reset();
		}
	}

	this->target = target;
	this->powerUpObj = powerUp;
	sight(dt,target);
	//distanceToPlayer = (target.GetPosition() - transform.GetPosition()).Length();
	
	if(bGameOver)
		Debug::Print("You Lost !!!!", Vector2(15, 50));

	
}

void NCL::CSC8503::Enemy::findPath(Transform target)
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

void NCL::CSC8503::Enemy::patrol()
{
	int randIndex = rand() % patrolPoints.size();
	Vector3 dir = patrolPoints[randIndex] - transform.GetPosition();
	physicsObject->AddForce(dir * 10);
}

void NCL::CSC8503::Enemy::lookForBonus()
{
	std::cout << "going to pick up power";
	findPath(powerUpObj);
}

void NCL::CSC8503::Enemy::chaseTarget(float dt,Transform target)
{ 
	findPath(target);

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

void NCL::CSC8503::Enemy::sight(float dt, Transform target)
{
	Ray ray(transform.GetPosition(),(target.GetPosition() - transform.GetPosition()).Normalised());
	RayCollision collision;

	world->Raycast(ray, collision,true,this);
	distanceToPlayer = (((GameObject*)collision.node)->GetTransform().GetPosition() - transform.GetPosition()).Length();

	//Debug::DrawLine(ray.GetPosition(), collision.collidedAt, Debug::BLUE, .1f);
}

void NCL::CSC8503::Enemy::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetName() == "Goaty") {

		bGameOver = true;
		world->RemoveGameObject(otherObject, false);
	}

	if (otherObject->GetName() == "bonus") {
		
		world->RemoveGameObject(otherObject, false);
	}
}

void NCL::CSC8503::Enemy::OnCollisionEnd(GameObject* otherObject)
{
}



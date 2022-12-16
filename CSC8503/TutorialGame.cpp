#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "StateMachine.h"
#include "GameTimer.h"
#include "Assets.h"
#include "MenuManager.h"

#include <fstream>

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	myDeltaTime = 0;
	force = 10;
	playerRotateSpeed = 0.25f;
	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = false;
	scoreManager = new ScoreManager();
	menuManager = new MenuManager();
	gameManager = new GameManager();
	door = new Door(gameManager,world,scoreManager);
	//coins = new Coin(world);
	player = new Character(gameManager,scoreManager,world);
	

	InitialiseAssets();
}

void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	gooseMesh	= renderer->LoadMesh("goose.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	coinMesh = renderer->LoadMesh("coin.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
	

	damping = 0.4f;

	menuManager->Init();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete coinMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;

	delete scoreManager;
	delete menuManager;
	delete player;
}


void TutorialGame::UpdateGame(float dt) {

	if (menuManager) {
		menuManager->Update(dt);
	}

	if (menuManager->bPauesd) return;

	myDeltaTime += dt;
	world->GetMainCamera()->UpdateCamera(dt);
	//UpdateKeys(myDeltaTime);

	//SelectObject();
	//MoveSelectedObject();

	Debug::Print("Time: " + std::to_string(Window::GetTimer()->GetTotalTimeSeconds()), Vector2(35, 5));
	Debug::Print("Score: " + std::to_string(scoreManager->GetScore()), Vector2(35, 10));

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	float cameraX = cameraDist * cos((world->GetMainCamera()->GetYaw() + 270) * 3.14f / 180) + player->GetTransform().GetPosition().x;
	float cameraY = cameraDist * sin((world->GetMainCamera()->GetPitch()) * 3.14f / 180);
	float cameraZ = cameraDist * sin((world->GetMainCamera()->GetYaw() - 270) * 3.14f / 180) + player->GetTransform().GetPosition().z;

	world->GetMainCamera()->SetPosition(Vector3(cameraX, -cameraY + player->GetTransform().GetPosition().y, cameraZ));

	renderer->Render();
	Debug::UpdateRenderables(dt);


	if (player) {
		player->Update(dt, world);
	}
	if (enemy) {
		//enemy->findPath(player->GetTransform(), dt);
		//enemy->chasePlayer(dt);
		enemy->Update(dt,player->GetTransform(),PowerUpObj->GetTransform());
	}
	if (goose) {
		goose->Update(dt, player->GetTransform(), PowerUpObj->GetTransform());
	}
	if (door) {
		door->Update(dt);
	}

}

void TutorialGame::UpdateKeys(float myDeltaTime) {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
		physics->SetGravity(Vector3(0, -9.8f, 0));
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::C))
	{
		damping++;
		physics->SetDamping(damping);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	//world->GetMainCamera()->SetPitch(-15.0f);
	//world->GetMainCamera()->SetYaw(315.0f);
	//world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();
	physics->UseGravity(true);

	InitMixedGridWorld(4,4, 3.0f, 3.0f);

	InitGameExamples();
	InitDefaultFloor();

	testStateObject = AddStateObjectToWorld(Vector3(0, 30, 0));
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	floor->SetName("floor");
	floor->bTriggerDelete = true;
	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();
	sphere->SetName("sphere");
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	//sphere->bTriggerDelete = true;

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* NCL::CSC8503::TutorialGame::AddCollectableToWorld(const Vector3& position, float radius, float inverseMass)
{
	GameObject* coin = new GameObject();
	coin->SetName("coin");

	coin->bTriggerDelete = true;

	Vector3 sphereSize = Vector3(radius, radius, radius);
	AABBVolume* volume = new AABBVolume(sphereSize);
	coin->SetBoundingVolume((CollisionVolume*)volume);

	coin->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	coin->SetRenderObject(new RenderObject(&coin->GetTransform(), coinMesh, basicTex, basicShader));
	coin->SetPhysicsObject(new PhysicsObject(&coin->GetTransform(), coin->GetBoundingVolume()));

	coin->GetPhysicsObject()->SetInverseMass(inverseMass);
	coin->GetPhysicsObject()->InitSphereInertia();
	coin->GetPhysicsObject()->SetElasticity(0.0f);



	world->AddGameObject(coin);

	return coin;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();
	cube->SetName("cube");
	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(cube);

	return cube;
}

GameObject* NCL::CSC8503::TutorialGame::AddKeyToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* key = new GameObject();
	key->SetName("key");
	key->bTriggerDelete = true;
	AABBVolume* volume = new AABBVolume(dimensions);
	key->SetBoundingVolume((CollisionVolume*)volume);

	key->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	key->SetRenderObject(new RenderObject(&key->GetTransform(), cubeMesh, basicTex, basicShader));
	key->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	key->SetPhysicsObject(new PhysicsObject(&key->GetTransform(), key->GetBoundingVolume()));

	key->GetPhysicsObject()->SetInverseMass(inverseMass);
	key->GetPhysicsObject()->InitCubeInertia();
	key->GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(key);

	return key;
}

//GameObject* NCL::CSC8503::TutorialGame::AddDoorToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
//{
//	GameObject* door = new GameObject();
//	door->SetName("door");
//	door->bTriggerDelete = true;
//	AABBVolume* volume = new AABBVolume(dimensions);
//	door->SetBoundingVolume((CollisionVolume*)volume);
//
//	door->GetTransform()
//		.SetPosition(position)
//		.SetScale(dimensions * 2);
//
//	door->SetRenderObject(new RenderObject(&door->GetTransform(), cubeMesh, basicTex, basicShader));
//	door->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
//	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));
//
//	door->GetPhysicsObject()->SetInverseMass(inverseMass);
//	door->GetPhysicsObject()->InitCubeInertia();
//	door->GetPhysicsObject()->SetElasticity(0.0f);
//
//	world->AddGameObject(door);
//
//	return door;
//}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume  = new SphereVolume(1.0f);

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

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();
	character->SetName("Enemy");
	character->bTriggerDelete = true;

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {

	GameObject* bonus = new GameObject();
	bonus->SetName("bonus");

	SphereVolume* volume = new SphereVolume(2.0f);
	bonus->SetBoundingVolume((CollisionVolume*)volume);
	bonus->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), sphereMesh, nullptr, basicShader));
	bonus->GetRenderObject()->SetColour(Debug::YELLOW);
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume()));

	bonus->GetPhysicsObject()->SetInverseMass(1.0f);
	bonus->GetPhysicsObject()->InitSphereInertia();

	bonus->GetPhysicsObject()->SetElasticity(0.0f);

	world->AddGameObject(bonus);

	return bonus;
}

StateGameObject* NCL::CSC8503::TutorialGame::AddStateObjectToWorld(const Vector3& position)
{
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -20, 0));
}

void TutorialGame::InitGameExamples() {
	//gameManager->bisKeyCollected = true;
	door->AddDoorToWorld(Vector3(10, -15, 15), Vector3(2, 4, 2), 0.0f, cubeMesh, basicShader, basicTex);
	CreateMaze("TestGrid1.txt");
	AddKeyToWorld(Vector3(15, -15, 20), Vector3(1, 1, 1), 1.0f);
	//AddDoorToWorld(Vector3(10, -15, 0), Vector3(1, 2, 1), 0.0f);
	//coins->InitCollectableGridWorld(2, 2, 10, 10, .2f, this);
	player->Init("Goaty",Vector3(80, 0, 50), charMesh, basicShader, world);
	player->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	
	enemy = new Enemy(world,true);
	goose = new Enemy(world,false);
	//AddEnemyToWorld(Vector3(5, 5, 0));
	enemy->Init("Enemy", Vector3(80, 0, 10), enemyMesh, basicShader, world);
	goose->Init("Goose", Vector3(50, 0, 10), gooseMesh, basicShader, world);
	
	//enemy->findPath();
	InitCollectableGridWorld(3, 3, 40, 40, 0.2f);
	//AddCollectableToWorld(Vector3(10, 2, 0), .2f, 1.0f);
	PowerUpObj = AddBonusToWorld(Vector3(60, 0, 60));

	BridgeConstraintTest();
}

void NCL::CSC8503::TutorialGame::CreateMaze(const std::string& filename)
{
	std::ifstream infile(Assets::DATADIR + filename);
	int nodeSize, gridWidth, gridHeight;

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {

			char type = 0;
			infile >> type;
			Vector3 position = Vector3((float)(x * nodeSize), -13, (float)(y * nodeSize));
			if (type == 'x') {
				AddCubeToWorld(position, Vector3(nodeSize / 2, nodeSize / 2, nodeSize / 2), 0.0f);
			}
		}
	}
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void NCL::CSC8503::TutorialGame::InitCollectableGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius)
{
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCollectableToWorld(position, radius, 1.0f);
		}
	}
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x + 10 * colSpacing, 5.0f, z + 10 * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Debug::RED, 10.0f);
				return true;
			}
			else {
				return false;
			}
		}
		/*if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}*/
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U))
	{
		selectionObject->GetPhysicsObject()->AddForceAtPosition(Vector3(0,1,0)* forceMagnitude, selectionObject->GetTransform().GetPosition());
	}
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(2,2,2);
	
	float invCubeMass = 2; //how heavy the middle pieces are
	int numLinks = 6;
	float maxDistance = 10; // constraint distance
	float cubeDistance = 8; // distance between links
	
	Vector3 startPos = Vector3(0,20,0);
	
	GameObject * start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject * end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject * previous = start;
	
	for (int i = 0; i < numLinks; ++i) {
		GameObject * block = AddCubeToWorld(startPos + Vector3((i + 1) *
			cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint * constraint = new PositionConstraint(previous,
		block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint * constraint = new PositionConstraint(previous,
		end, maxDistance);
		world->AddConstraint(constraint);
}

NCL::CSC8503::Coin::Coin(GameWorld* gameWorld)
{
	this->gameWorld = gameWorld;
}

void NCL::CSC8503::Coin::InitCollectableGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius, TutorialGame* game)
{
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCollectableToWorld(position, radius,1.0f, game->coinMesh,game->basicShader,game->basicTex);
		}
	}
}

GameObject* NCL::CSC8503::Coin::AddCollectableToWorld(const Vector3& position, float radius, float inverseMass,MeshGeometry* coinMesh,ShaderBase* basicShader,TextureBase* basicTex)
{
	//coin->SetName("coin");
	this->bTriggerDelete = true;

	Vector3 sphereSize = Vector3(radius, radius, radius);
	AABBVolume* volume = new AABBVolume(sphereSize);
	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	SetRenderObject(new RenderObject(&GetTransform(), coinMesh, basicTex, basicShader));
	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitSphereInertia();
	GetPhysicsObject()->SetElasticity(0.0f);



	gameWorld->AddGameObject(this);

	return this;
}

void NCL::CSC8503::Coin::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetName() == "Goaty") {
		gameWorld->RemoveGameObject(this, false);
	}
}

void NCL::CSC8503::Coin::OnCollisionEnd(GameObject* otherObject)
{
}

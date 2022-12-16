#include "MenuManager.h"
#include "GameTechRenderer.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "Debug.h"

using namespace NCL;
using namespace CSC8503;



void MenuManager::Init()
{
	bPauesd = true;

	stateMachine = new StateMachine();

	State* startScreen = new State([&](float dt)->void
		{
			Debug::Print("Press F to play", Vector2(5, 85));
			Debug::Print("Press Esc to Quit", Vector2(5, 85));
			bPauesd = true;
			//system("pause");
		}
	);

	State* playGame = new State([&](float dt)->void
		{
			bPauesd = false;
		}
	);


	State* pauseGame = new State([&](float dt)->void
		{
			Debug::Print("Game is Paused, Press ENTER ", Vector2(5, 85));
			bPauesd = true;
			//system("pause");
		}
	);

	State* unPauseGame = new State([&](float dt)->void
		{
			std::cout << "Game is Unpauesed" << std::endl;
			bPauesd = false;
		}
	);

	StateTransition* unPausetoPauseScreen = new StateTransition(unPauseGame, pauseGame, [&](void)->bool
		{
			return Window::GetKeyboard()->KeyDown(KeyboardKeys::P);
		}
	);

	StateTransition* pausetoUnPauseScreen = new StateTransition(pauseGame, unPauseGame, [&](void)->bool
		{
			return Window::GetKeyboard()->KeyDown(KeyboardKeys::U);
		}
	);

	StateTransition* startScreenTogame = new StateTransition(startScreen, playGame, [&](void)->bool
		{
			return Window::GetKeyboard()->KeyDown(KeyboardKeys::F);
		}
	);

	StateTransition* gameToPause = new StateTransition(playGame, pauseGame, [&](void)->bool
		{
			return Window::GetKeyboard()->KeyDown(KeyboardKeys::P);
		}
	);

	stateMachine->AddState(startScreen);
	stateMachine->AddState(playGame);
	stateMachine->AddState(unPauseGame);
	stateMachine->AddState(pauseGame);
	
	

	stateMachine->AddTransition(pausetoUnPauseScreen);
	stateMachine->AddTransition(unPausetoPauseScreen);
	stateMachine->AddTransition(startScreenTogame);
	stateMachine->AddTransition(gameToPause);
	

}

void NCL::CSC8503::MenuManager::Update(float dt)
{
	stateMachine->Update(dt);
}

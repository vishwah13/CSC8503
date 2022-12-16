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
	stateMachine = new StateMachine();

	State* pauseGame = new State([&](float dt)->void
		{
			Debug::Print("Game is Paused, Press ENTER ", Vector2(35, 15));
			system("pause");
		}
	);

	State* unPauseGame = new State([&](float dt)->void
		{
			std::cout << "Game is Unpauesed" << std::endl;
		}
	);

	StateTransition* unPausetoPauseScreen = new StateTransition(unPauseGame, pauseGame, [&](void)->bool
		{
			return Window::GetKeyboard()->KeyDown(KeyboardKeys::P);
		}
	);

	StateTransition* pausetoUnPauseScreen = new StateTransition(pauseGame, unPauseGame, [&](void)->bool
		{
			return Window::GetKeyboard()->KeyDown(KeyboardKeys::RETURN);
		}
	);

	stateMachine->AddState(unPauseGame);
	stateMachine->AddState(pauseGame);
	

	stateMachine->AddTransition(pausetoUnPauseScreen);
	stateMachine->AddTransition(unPausetoPauseScreen);
	

}

void NCL::CSC8503::MenuManager::Update(float dt)
{
	stateMachine->Update(dt);
}

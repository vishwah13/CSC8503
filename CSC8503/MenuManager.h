#pragma once

namespace NCL {

	namespace CSC8503 {

		class StateMachine;
		class MenuManager {

		public:
			StateMachine* stateMachine;

			void Init();
			void Update(float dt);
		};
	}
}



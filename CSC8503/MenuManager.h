#pragma once

namespace NCL {

	namespace CSC8503 {

		class StateMachine;
		class MenuManager {

		public:
			StateMachine* stateMachine;
			bool bPauesd = false;

			void Init();
			void Update(float dt);
		};
	}
}



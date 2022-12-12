#pragma once
class ScoreManager
{
public:
	void AddScore(int n) {
		Score += n;
	}
	void SubScore(int n) {
		Score += n;
	}
	int GetScore() {
		return Score;
	}
protected:
	int Score = 0;
};


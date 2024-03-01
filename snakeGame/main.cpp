#include <iostream>
#include "raylib.h"
#include <deque>
#include <raymath.h>

using namespace std;

Color background = { 255, 255, 255, 255 };
Color darkGreen = { 43, 51, 24, 255 };
Color red = { 165, 0, 0, 255 };

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
	for (unsigned int i = 0; i < deque.size(); i++) {
		if (Vector2Equals(deque[i], element)) {
			return true;
		}
	}
	return false;
}

bool eventTriggered(double interval) {
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval) {
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snake
{

public:
	// snake is collection of cells
	deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
	Vector2 direction = { 1,0 };
	bool addSegment = false;

	void Draw() {
		// cellCount 를 기준으로 충돌 판정하기 때문에 offset을 더해준다.
		for (unsigned int i = 0; i < body.size(); i++) {
			int x = body[i].x;
			int y = body[i].y;
			Rectangle segment = Rectangle{ (float)offset + x * cellSize, (float)offset + y * cellSize, (float)cellSize, (float)cellSize };
			DrawRectangleRounded(segment, 0.2, 6, darkGreen);
		}
	}

	void Update() {
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment == true) {
			addSegment = false;
		}
		else {
			body.pop_back();
		}
	}

	void Reset() {
		body = { Vector2{6,9}, Vector2{5, 9}, Vector2{4, 9} };
		direction = { 1,0 };
	}
};

class Food {

public:
	Vector2 position = { 5, 6 };
	//Texture2D texture;

	Food(deque<Vector2> snakeBody) {
		//Image image = LoadImage("Graphics/food.png");
		//texture = LoadTextureFromImage(image);
		//UnloadImage(image);

		position = GenerateRandomPos(snakeBody);
	}

	~Food() {
		//UnloadTexture(texture);
	}

	void Draw() {
		Rectangle segment = Rectangle{ (float)offset + position.x * cellSize, (float)offset + position.y * cellSize, (float)cellSize, (float)cellSize };
		DrawRectangleRounded(segment, 0.5, 6, red);
		//DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
		//DrawRectangle(position.x * cellSize, position.y* cellSize, cellSize, cellSize, darkGreen);
	}
	Vector2 GenerateRandomCell() {
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{ x, y };
	}
	Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
		Vector2 position = GenerateRandomCell();
		while (ElementInDeque(position, snakeBody)) {
			position = GenerateRandomCell();
		}
		return position;
	}
};

class Game {

public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;
	Sound eatSound;
	Sound wallSound;

	Game() {
		InitAudioDevice();
		eatSound = LoadSound("Sounds/eat.mp3");
		wallSound = LoadSound("Sounds/death.mp3");
	}

	~Game() {
		UnloadSound(eatSound);
		UnloadSound(wallSound);
		CloseAudioDevice();
	}

	void Draw() {
		food.Draw();
		snake.Draw();
	}
	void Update() {
		if (running) {
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdges();
			CheckCollisionWithBody();
		}
	}

	void CheckCollisionWithFood() {
		if (Vector2Equals(snake.body[0], food.position)) {
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score++;
			PlaySound(eatSound);
		}
	}

	void CheckCollisionWithEdges() {
		if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
			GameOver();
		}
		if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
			GameOver();
		}
	}

	void CheckCollisionWithBody() {
		// 반복문에 비해 장점: 길이가 길어질수록 ElementInDeque 메서드로 검토하는 것이 효율적이다. 
		deque<Vector2> headlessBody = snake.body;
		headlessBody.pop_front();
		if (ElementInDeque(snake.body[0], headlessBody))
		{
			GameOver();
		}
	}

	void GameOver() {
		PlaySound(wallSound);
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		running = false;
		score = 0;
	}
};

int main() {
	cout << "Starting the game" << endl;
	InitWindow(2*offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");
	SetTargetFPS(60);

	Game game = Game();

	while (WindowShouldClose() == false)
	{
		BeginDrawing();

		if (eventTriggered(0.15)) {
			game.Update();
		}

		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
			game.snake.direction = { 0, -1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
			game.snake.direction = { 0, 1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
			game.snake.direction = { -1, 0 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
			game.snake.direction = { 1, 0 };
			game.running = true;
		}

		// Drawing
		ClearBackground(background);
		// 두께 5이므로 시작위치 -5, 높이와 길이 + 10
		DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount + 10}, 5, darkGreen);
		DrawText("Snake Game", offset - 5, 20, 40, darkGreen);
		DrawText(TextFormat("%i", game.score), offset + cellSize * cellCount-40, 20, 40, darkGreen);
		game.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
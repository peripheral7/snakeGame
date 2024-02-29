#include <iostream>
#include "raylib.h"
#include <deque>
#include <raymath.h>

using namespace std;

Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };

int cellSize = 30;
int cellCount = 25;

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

	void Draw() {
		for (unsigned int i = 0; i < body.size(); i++) {
			int x = body[i].x;
			int y = body[i].y;
			DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, darkGreen);
		}
	}

	void Update() {
		body.pop_back();
		body.push_front(Vector2Add(body[0], direction));
	}
};

class Food {

public:
	Vector2 position = { 5, 6 };
	Texture2D texture;

	Food(deque<Vector2> snakeBody) {
		Image image = LoadImage("Graphics/food.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPos(snakeBody);
	}

	~Food() {
		UnloadTexture(texture);
	}

	void Draw() {
		DrawTexture(texture, position.x * cellSize, position.y * cellSize, WHITE);
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

	void Draw()	{
		food.Draw();
		snake.Draw();
	}
	void Update() {
		snake.Update();
	}

	void CheckCollisionWithFood() {
		if (Vector2Equals(snake.body[0], food.position)) {
			food.position = food.GenerateRandomPos(snake.body);
		}
	}
};

int main() {
	cout << "Starting the game" << endl;
	InitWindow(cellSize * cellCount, cellSize * cellCount, "Retro Snake");
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
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
			game.snake.direction = { 0, 1 };
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
			game.snake.direction = { -1, 0 };
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
			game.snake.direction = { 1, 0 };
		}

		ClearBackground(green);
		game.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
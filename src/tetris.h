#pragma once
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


// Tetris dimensions: 12 x 22 (including borders)
extern const int BOARD_WIDTH;
extern const int BOARD_HEIGHT;

extern const uint16_t ROW_EMPTY;
extern const uint16_t ROW_TOP;
extern const uint16_t ROW_BOTTOW;

enum Color { Red, Green, Blue, Cyan, Yellow, Orange, Magenta, Grey, COUNT };

extern const sf::Color block_colors[];

enum MoveDir { LEFT, RIGHT, DOWN, ROTATE };

struct Tetromino
{
	uint16_t shape_current[4];
	uint16_t shape_rotations[16];
	uint16_t y = 1; // Top row
	uint16_t x = 6; // Shifted to right 
	uint16_t rot = 0;
	sf::Color color = sf::Color::Red;
};

extern const uint16_t* shapes[5];
extern bool paused;
extern float timer;


#pragma region Shapes

// S shape
extern const uint16_t shape_S[16];

// L shape
extern const uint16_t shape_L[16];

// I shape
extern const uint16_t shape_I[16];

// T shape
extern const uint16_t shape_T[16];

// O shape
extern const uint16_t shape_O[16];
#pragma endregion

// Board
extern uint16_t board[];


bool CollisionCheck(Tetromino& tet, uint16_t* board, MoveDir dir, uint16_t rot);

void SpawnNewTet(Tetromino& tet);

void SetTetRot(Tetromino& tet, int offset, uint16_t rot);

void ResetBoard(uint16_t* board, sf::Image& board_image);

void GameOver(Tetromino& tet, uint16_t* board, sf::Image& board_image);

void MergeTetToBoard(Tetromino& tet, uint16_t* board, sf::Image& board_image);

void ClearRow(uint16_t y, uint16_t* board, sf::Image& board_image);

void DropReprojection(Tetromino& tet, uint16_t* board, sf::Vector2i* drop_proj_pixels);

void HandleKeyPressed(sf::RenderWindow& target_window, sf::Keyboard::Key key, Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels);

void HandleInputEvent(sf::RenderWindow& target_window, sf::Event& event, Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels);

void CopyPixelRow(sf::Image& board_image, uint16_t src_y, uint16_t dest_y);

void Update(Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels);

void DrawTet(Tetromino& tet, sf::Image& tet_image, sf::Vector2i* drop_proj_pixels);
#include <iostream>
#include "tetris.h"

// Tetris dimensions: 12 x 22 (including borders)
const int BOARD_WIDTH = 12;
const int BOARD_HEIGHT = 22;

const uint16_t ROW_EMPTY = 0b0010000000000100;
const uint16_t ROW_TOP = 0b0011111111111100;
const uint16_t ROW_BOTTOW = 0b0011111111111100;

const sf::Color block_colors[]
{
	sf::Color(255, 0, 0),
	sf::Color(0, 255, 0),
	sf::Color(0, 0, 255),
	sf::Color(0, 204, 204),
	sf::Color(204, 204, 0),
	sf::Color(204, 102, 0),
	sf::Color(153, 0, 204),
	sf::Color(119, 119, 119),
	sf::Color(0, 0, 0)
};

const uint16_t* shapes[5];
bool paused;
float timer;

#pragma region Shapes

// S shape
const uint16_t shape_S[16]
{
	// Rot 0
	0b0000000000000000,
	0b0000000000000000,
	0b0000000110000000,
	0b0000001100000000,

	// Rot 1
	0b0000000000000000,
	0b0000000100000000,
	0b0000000110000000,
	0b0000000010000000,

	// Rot 2
	0b0000000000000000,
	0b0000000000000000,
	0b0000000110000000,
	0b0000001100000000,

	// Rot 3
	0b0000000000000000,
	0b0000000100000000,
	0b0000000110000000,
	0b0000000010000000,
};


// L shape
const uint16_t shape_L[16]
{
	// Rot 0
	0b0000000000000000,
	0b0000000000000000,
	0b0000001110000000,
	0b0000000010000000,

	// Rot 1
	0b0000000000000000,
	0b0000000100000000,
	0b0000000100000000,
	0b0000001100000000,

	// Rot 2
	0b0000000000000000,
	0b0000001000000000,
	0b0000001110000000,
	0b0000000000000000,

	// Rot 3
	0b0000000000000000,
	0b0000000110000000,
	0b0000000100000000,
	0b0000000100000000,
};


// I shape
const uint16_t shape_I[16]
{
	// Rot 0
	0b0000000000000000,
	0b0000000000000000,
	0b0000001111000000,
	0b0000000000000000,

	// Rot 1
	0b0000000100000000,
	0b0000000100000000,
	0b0000000100000000,
	0b0000000100000000,

	// Rot 2
	0b0000000000000000,
	0b0000000000000000,
	0b0000001111000000,
	0b0000000000000000,

	// Rot 3
	0b0000000100000000,
	0b0000000100000000,
	0b0000000100000000,
	0b0000000100000000,
};


// T shape
const uint16_t shape_T[16]
{
	// Rot 0
	0b0000000000000000,
	0b0000000000000000,
	0b0000001110000000,
	0b0000000100000000,

	// Rot 1
	0b0000000000000000,
	0b0000000100000000,
	0b0000001100000000,
	0b0000000100000000,

	// Rot 2
	0b0000000000000000,
	0b0000000100000000,
	0b0000001110000000,
	0b0000000000000000,

	// Rot 3
	0b0000000000000000,
	0b0000000100000000,
	0b0000000110000000,
	0b0000000100000000,
};


// O shape
const uint16_t shape_O[16]
{
	// Rot 0
	0b0000000000000000,
	0b0000000110000000,
	0b0000000110000000,
	0b0000000000000000,

	// Rot 1
	0b0000000000000000,
	0b0000000110000000,
	0b0000000110000000,
	0b0000000000000000,

	// Rot 2
	0b0000000000000000,
	0b0000000110000000,
	0b0000000110000000,
	0b0000000000000000,

	// Rot 3
	0b0000000000000000,
	0b0000000110000000,
	0b0000000110000000,
	0b0000000000000000,
};

#pragma endregion

// Board
uint16_t board[BOARD_HEIGHT]
{
	0b0011111111111100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0010000000000100,
	0b0011111111111100
};

bool CollisionCheck(Tetromino& tet, uint16_t* board, MoveDir dir, uint16_t rot = 0)
{
	size_t y = 0;
	int offset = (int)tet.x - 6;
	//std::cout << "offset:" << offset << std::endl;

	for (size_t i = 0; i < 4; i++, y++)
	{
		switch (dir)
		{
		case LEFT:
			if ((tet.shape_current[i] << 1) & board[tet.y + y])
			{
				std::cout << "Collision when moving to the left!\n";
				return true;
			}
			break;
		case RIGHT:
			if ((tet.shape_current[i] >> 1) & board[tet.y + y])
			{
				std::cout << "Collision when moving to the right!\n";
				return true;
			}
			break;
		case DOWN:
			if ((tet.shape_current[i]) & board[tet.y + y + 1])
			{
				std::cout << "Collision when moving down!\n";
				return true;
			}
			break;
		case ROTATE:
		{
			// Select rotation
			uint16_t* rot_rep = tet.shape_rotations;
			switch (rot)
			{
			case 0:
				rot_rep = tet.shape_rotations;
				break;
			case 1:
				rot_rep = tet.shape_rotations + 4;
				break;
			case 2:
				rot_rep = tet.shape_rotations + 8;
				break;
			case 3:
				rot_rep = tet.shape_rotations + 12;
				break;
			default:
				rot_rep = tet.shape_rotations;
				break;
			}


			// Offset rotation and check collision
			if (offset > 0)
			{
				if ((rot_rep[i] >> offset) & board[tet.y + y])
				{
					std::cout << "Collision when rotating!\n";
					return true;
				}
			}
			else
			{
				if ((rot_rep[i] << abs(offset)) & board[tet.y + y])
				{
					std::cout << "Collision when rotating!\n";
					return true;
				}
			}
			break;
		}
		default:
			break;
		}
	}
	return false;
}

void SpawnNewTet(Tetromino& tet)
{
	int shape_index = rand() % 5;
	std::copy(shapes[shape_index], shapes[shape_index] + 4, tet.shape_current);
	std::copy(shapes[shape_index], shapes[shape_index] + 16, tet.shape_rotations);
	tet.x = 6;
	tet.y = 1;
	tet.rot = 0;
	tet.color = block_colors[rand() % (COUNT - 1)];
	std::cout << "New Tetromino!\n";
}

void SetTetRot(Tetromino& tet, int offset, uint16_t rot)
{
	// Set rotation
	switch (rot)
	{
	case 0:
		std::copy(tet.shape_rotations, tet.shape_rotations + 4, tet.shape_current);
		break;
	case 1:
		std::copy(tet.shape_rotations + 4, tet.shape_rotations + 8, tet.shape_current);
		break;
	case 2:
		std::copy(tet.shape_rotations + 8, tet.shape_rotations + 12, tet.shape_current);
		break;
	case 3:
		std::copy(tet.shape_rotations + 12, tet.shape_rotations + 16, tet.shape_current);
		break;
	default:
		break;
	}

	// Set shift
	for (size_t i = 0; i < 4; i++)
	{
		if (offset > 0)
		{
			tet.shape_current[i] >>= offset;
		}
		else
		{
			tet.shape_current[i] <<= abs(offset);
		}
	}
}

void ResetBoard(uint16_t* board, sf::Image& board_image)
{
	for (size_t y = 1; y < BOARD_HEIGHT - 1; y++)
	{
		board[y] = 0b0010000000000100;
		for (size_t x = 1; x < BOARD_WIDTH - 1; x++)
		{
			board_image.setPixel(x, y, sf::Color::Black);
		}
	}
	std::cout << "Board Reset!\n";
}

void GameOver(Tetromino& tet, uint16_t* board, sf::Image& board_image)
{
	std::cout << "\nGame Over!\n\n";
	ResetBoard(board, board_image);
	timer = 0;
	std::cout << "New Game!\n";
	SpawnNewTet(tet);
}

void MergeTetToBoard(Tetromino& tet, uint16_t* board, sf::Image& board_image)
{
	for (size_t i = 0; i < 4; i++)
	{
		board[tet.y + i] |= tet.shape_current[i];
	}

	for (size_t y = 0; y < BOARD_HEIGHT; y++)
	{
		for (size_t x = 0; x < BOARD_WIDTH; x++) // Should we loop from 0 - 16 instead?
		{
			if (y >= tet.y && y < tet.y + 4)
			{
				if (tet.shape_current[(y - tet.y)] & (1 << (BOARD_WIDTH - x + 1)))
				{
					//game_window_image.setPixel(x, y, current_tet.color);
					board_image.setPixel(x, y, tet.color);
				}
			}
		}
	}
}

void ClearRow(uint16_t y, uint16_t* board, sf::Image& board_image)
{
	board[y] = 0b0010000000000100;
	for (size_t x = 1; x < BOARD_WIDTH - 1; x++)
	{
		board_image.setPixel(x, y, sf::Color::Black);
	}
	std::cout << "Row " << y << " cleared!\n";
}

void HandleKeyPressed(sf::RenderWindow& target_window, sf::Keyboard::Key key, Tetromino& tet, uint16_t* board, sf::Image& board_image)
{
	using namespace sf;

	if (key == Keyboard::Escape)
	{
		target_window.close();
		std::cout << "Window closed!\n";
	}

	if (key == Keyboard::Enter)
	{
		paused = !paused;
		std::cout << "Paused!\n";
	}

	if (key == Keyboard::BackSpace)
	{
		ResetBoard(board, board_image);
	}

	if (!paused)
	{
		if (key == Keyboard::Right)
		{
			std::cout << "Move Right!\n";

			bool collision = CollisionCheck(tet, board, RIGHT);

			if (!collision)
			{
				for (size_t i = 0; i < 4; i++)
				{
					tet.shape_current[i] >>= 1;
				}
				tet.x += 1;
			}
			std::cout << "X: " << tet.x << std::endl;;
		}

		if (key == Keyboard::Left)
		{
			std::cout << "Move Left!\n";
			bool collision = CollisionCheck(tet, board, LEFT);

			if (!collision)
			{
				for (size_t i = 0; i < 4; i++)
				{
					tet.shape_current[i] <<= 1;
				}
				tet.x -= 1;
			}
			std::cout << "X: " << tet.x << std::endl;
		}

		if (key == Keyboard::Up)
		{
			std::cout << "Rotate!\n";

			int temp_rot = tet.rot;
			if (temp_rot < 3)
			{
				temp_rot += 1;
			}
			else
			{
				temp_rot = 0;
			}

			bool collision = CollisionCheck(tet, board, ROTATE, temp_rot);
			int offset = (int)tet.x - 6;
			//std::cout << "offset:" << offset << std::endl;
			if (!collision)
			{
				tet.rot = temp_rot;
				SetTetRot(tet, offset, tet.rot);
			}
			std::cout << "Current rotation " << tet.rot << std::endl;
		}


		if (key == Keyboard::Down)
		{
			std::cout << "Move Down!\n";
			bool collision = CollisionCheck(tet, board, DOWN);

			if (!collision)
			{
				tet.y += 1;
				timer = 0;
				// Restart timer?
			}
			else
			{
				MergeTetToBoard(tet, board, board_image);

				SpawnNewTet(tet);
			}
		}

		if (key == Keyboard::Space)
		{
			std::cout << "Drop!\n";
			bool collision = false;
			collision = CollisionCheck(tet, board, DOWN);
			while (!collision)
			{
				tet.y += 1;

				collision = CollisionCheck(tet, board, DOWN);
			}

			if (collision)
			{
				std::cout << "Collision when dropping!\n";
				MergeTetToBoard(tet, board, board_image);

				SpawnNewTet(tet);
			}
		}
	}
}

void HandleInputEvent(sf::RenderWindow& target_window, sf::Event& event, Tetromino& tet, uint16_t* board, sf::Image& board_image)
{
	using namespace sf;

	if (event.type == Event::KeyPressed)
	{
		Keyboard::Key key = event.key.code;
		HandleKeyPressed(target_window, key, tet, board, board_image);
	}
	else if (event.type == Event::Closed)
	{
		target_window.close();
		std::cout << "Window closed!\n";
	}
}

void CopyPixelRow(sf::Image& board_image, uint16_t src_y, uint16_t dest_y)
{
	for (size_t i = 0; i < BOARD_WIDTH; i++)
	{
		board_image.setPixel(i, dest_y, board_image.getPixel(i, src_y));
	}
}

void Update(Tetromino& tet, uint16_t* board, sf::Image& board_image)
{
	// Check for full rows
	for (size_t y = BOARD_HEIGHT - 2; y > 1; y--)
	{
		if (board[y] == 0b0011111111111100) // 16380
		{
			ClearRow(y, board, board_image);
		}
	}

	// Check for hanging rows NOTE: Need to copy row pixels for drawing
	for (size_t y = BOARD_HEIGHT - 2; y > 0; y--)
	{
		//std::cout << "Checking row " << y << std::endl;
		uint16_t* row_below = &board[y + 1];
		if (board[y] != 0b0010000000000100 && *row_below == 0b0010000000000100)
		{
			uint16_t new_y = y;
			while (*row_below == 0b0010000000000100)
			{
				row_below += 1;
				new_y += 1;
			}

			*(row_below - 1) = board[y];
			CopyPixelRow(board_image, y, new_y);
			ClearRow(y, board, board_image);
		}
	}

	// Collision check down
	bool collision = CollisionCheck(tet, board, DOWN);
	

	if (collision)
	{
		if (tet.y != 1)
		{
			MergeTetToBoard(tet, board, board_image);

			SpawnNewTet(tet);
		}
		else
		{
			GameOver(tet, board, board_image);
			return;
		}
	}
	else
	{
		tet.y += 1;
		std::cout << "Move down\n";
	}

	std::cout << "Y:" << tet.y << std::endl;

	timer = 0;
}
#include <iostream>
#include "tetris.h"

const int WINDOW_WIDTH = 360;
const int WINDOW_HEIGHT = 660;

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
bool paused = false;
float timer = 0;
uint32_t score = 0;
uint16_t level = 1;
uint16_t lines = 0;

sf::SoundBuffer sound_buffers[8];
sf::Sound sound_effect;
sf::Sound sound_effect_move;

sf::Font retro_font;
sf::Text score_text;
sf::Text level_text;
sf::Text lines_text;
sf::Text pause_text;


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

	for (size_t i = 0; i < 4; i++, y++)
	{
		switch (dir)
		{
		case LEFT:
			if ((tet.shape_current[i] << 1) & board[tet.y + y])
			{
				//std::cout << "Collision when moving to the left!\n";
				return true;
			}
			break;
		case RIGHT:
			if ((tet.shape_current[i] >> 1) & board[tet.y + y])
			{
				//std::cout << "Collision when moving to the right!\n";
				return true;
			}
			break;
		case DOWN:
			if ((tet.shape_current[i]) & board[tet.y + y + 1])
			{
				//std::cout << "Collision when moving down!\n";
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
					//std::cout << "Collision when rotating!\n";
					return true;
				}
			}
			else
			{
				if ((rot_rep[i] << abs(offset)) & board[tet.y + y])
				{
					//std::cout << "Collision when rotating!\n";
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
	tet.y = 0;
	tet.rot = 0;
	tet.color = block_colors[rand() % (COUNT - 1)];
	timer = 0;
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

void GameOver(Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels)
{
	std::cout << "\nGame Over!\n\n";
	sound_effect.setBuffer(sound_buffers[(int)Sound::game_over]);
	sound_effect.play();
	std::cout << "Score: " << score << std::endl;

	GameStart(tet, board, board_image, drop_proj_pixels);
}

void GameStart(Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels)
{
	ResetBoard(board, board_image);
	timer = 0;
	score = 0;
	level = 1;
	lines = 0;
	paused = false;
	/*sound_effect.setBuffer(sound_buffers[new_game]);
	sound_effect.play();*/

	std::cout << "New Game!\n";
	score_text.setString("SCORE\n" + std::to_string(score));
	level_text.setString("LEVEL\n" + std::to_string(level));
	lines_text.setString("LINES\n" + std::to_string(lines));
	SpawnNewTet(tet);
	DropReprojection(tet, board, drop_proj_pixels);
}


void MergeTetToBoard(Tetromino& tet, uint16_t* board, sf::Image& board_image)
{
	for (size_t i = 0; i < 4; i++)
	{
		board[tet.y + i] |= tet.shape_current[i];
	}

	for (size_t y = 0; y < BOARD_HEIGHT; y++)
	{
		for (size_t x = 0; x < BOARD_WIDTH; x++)
		{
			if (y >= tet.y && y < tet.y + 4)
			{
				if (tet.shape_current[(y - tet.y)] & (1 << (BOARD_WIDTH - x + 1)))
				{
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

void DropReprojection(Tetromino& tet, uint16_t* board, sf::Vector2i* drop_proj_pixels)
{
	uint16_t org_y = tet.y;
	bool collision = false;
	collision = CollisionCheck(tet, board, DOWN);
	while (!collision)
	{
		tet.y += 1;

		collision = CollisionCheck(tet, board, DOWN);
	}

	int i = 0;
	for (size_t y = 0; y < BOARD_HEIGHT; y++)
	{
		for (size_t x = 0; x < BOARD_WIDTH; x++)
		{
			if (y >= tet.y && y < tet.y + 4)
			{
				if (tet.shape_current[(y - tet.y)] & (1 << (BOARD_WIDTH - x + 1)))
				{
					drop_proj_pixels[i] = sf::Vector2i(x, y);
					i += 1;
				}
			}
		}
	}

	tet.y = org_y;
}

void HandleKeyPressed(sf::RenderWindow& target_window, sf::Keyboard::Key key, Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels)
{
	if (key == sf::Keyboard::Escape)
	{
		target_window.close();
		std::cout << "Window closed!\n";
	}

	if (key == sf::Keyboard::Enter)
	{
		paused = !paused;
		std::cout << (paused ? "Paused!\n" : "Unpaused!\n");
	}

	if (key == sf::Keyboard::BackSpace)
	{
		ResetBoard(board, board_image);
		DropReprojection(tet, board, drop_proj_pixels);
	}

	if (!paused)
	{
		if (key == sf::Keyboard::Right)
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

				sound_effect_move.setBuffer(sound_buffers[(int)Sound::move]);
				sound_effect_move.play();
			}
			std::cout << "X: " << tet.x << std::endl;
		}

		if (key == sf::Keyboard::Left)
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

				sound_effect_move.setBuffer(sound_buffers[(int)Sound::move]);
				sound_effect_move.play();
			}
			std::cout << "X: " << tet.x << std::endl;
		}

		if (key == sf::Keyboard::Up)
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
			if (!collision)
			{
				tet.rot = temp_rot;
				SetTetRot(tet, offset, tet.rot);

				sound_effect_move.setBuffer(sound_buffers[(int)Sound::rotate]);
				sound_effect_move.play();
			}
			std::cout << "Current rotation " << tet.rot << std::endl;
		}


		if (key == sf::Keyboard::Down)
		{
			std::cout << "Move Down!\n";
			bool collision = CollisionCheck(tet, board, DOWN);

			if (!collision)
			{
				tet.y += 1;
				timer = 0;
			}
			else
			{
				MergeTetToBoard(tet, board, board_image);

				sound_effect_move.setBuffer(sound_buffers[(int)Sound::place]);
				sound_effect_move.play();

				SpawnNewTet(tet);
			}
		}

		if (key == sf::Keyboard::Space)
		{
			std::cout << "Drop!\n";
			bool collision = false;
			collision = CollisionCheck(tet, board, DOWN);
			uint16_t start_y = tet.y;

			while (!collision)
			{
				tet.y += 1;

				collision = CollisionCheck(tet, board, DOWN);
			}

			if (collision)
			{
				MergeTetToBoard(tet, board, board_image);

				score += (tet.y - start_y);
				score_text.setString("SCORE\n" + std::to_string(score));

				SpawnNewTet(tet);
			}

			sound_effect_move.setBuffer(sound_buffers[(int)Sound::place]);
			sound_effect_move.play();
		}
		DropReprojection(tet, board, drop_proj_pixels);
	}
}

void HandleInputEvent(sf::RenderWindow& target_window, sf::Event& event, Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels)
{
	using namespace sf;

	if (event.type == Event::KeyPressed)
	{
		Keyboard::Key key = event.key.code;
		HandleKeyPressed(target_window, key, tet, board, board_image, drop_proj_pixels);
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

void Update(Tetromino& tet, uint16_t* board, sf::Image& board_image, sf::Vector2i* drop_proj_pixels)
{
	bool line_cleared = false;
	uint16_t lines_cleared = 0;
	bool level_cleared = false;
	// Check for full rows
	for (size_t y = BOARD_HEIGHT - 2; y > 1; y--)
	{
		if (board[y] == 0b0011111111111100) // 16380
		{
			ClearRow(y, board, board_image);
			line_cleared = true;
			lines_cleared += 1;
		}
	}

	if (line_cleared)
	{
		if (lines_cleared > 3)
		{
			sound_effect.setBuffer(sound_buffers[(int)Sound::tetris]);
			score += 1200 * (level + 1);
		}
		else if(lines_cleared == 3)
		{
			sound_effect.setBuffer(sound_buffers[(int)Sound::line]);
			score += 300 * (level + 1);
		}
		else if (lines_cleared == 2)
		{
			sound_effect.setBuffer(sound_buffers[(int)Sound::line]);
			score += 100 * (level + 1);
		}
		else if (lines_cleared == 1)
		{
			sound_effect.setBuffer(sound_buffers[(int)Sound::line]);
			score += 40 * (level + 1);
		}

		sound_effect.play();

		if (lines < (level * 10) && (lines + lines_cleared ) >= (level * 10))
		{
			level += 1;
			level_text.setString("LEVEL\n" + std::to_string(level));
			sound_effect.setBuffer(sound_buffers[(int)Sound::next_level]);
			sound_effect.play();
		}

		lines += lines_cleared;
	}

	
	// Check for hanging rows NOTE: Need to copy row pixels for drawing
	for (size_t y = BOARD_HEIGHT - 2; y > 0; y--)
	{
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
		if (tet.y != 0)
		{
			MergeTetToBoard(tet, board, board_image);

			SpawnNewTet(tet);
		}
		else
		{
			GameOver(tet, board, board_image, drop_proj_pixels);
			return;
		}

		sound_effect.setBuffer(sound_buffers[(int)Sound::place]);
		sound_effect.play();
	}
	else
	{
		tet.y += 1;
		std::cout << "Move down\n";
	}

	DropReprojection(tet, board, drop_proj_pixels);

	

	std::cout << "Y:" << tet.y << std::endl;
	std::cout << "Score: " << score << std::endl;
	score_text.setString("SCORE\n" + std::to_string(score));
	lines_text.setString("LINES\n" + std::to_string(lines));
	timer = 0;
}

void DrawTet(Tetromino& tet, sf::Image& tet_image, sf::Vector2i* drop_proj_pixels)
{
	sf::Vector2i masked_proj_pixels[4];
	int i = 0;
	// Draw currently handled tetromino
	for (size_t y = 0; y < BOARD_HEIGHT; y++)
	{
		for (size_t x = 0; x < BOARD_WIDTH; x++)
		{
			if (y >= tet.y && y < tet.y + 4)
			{
				if (tet.shape_current[(y - tet.y)] & (1 << (BOARD_WIDTH - x + 1)))
				{
					tet_image.setPixel(x, y, tet.color);
					masked_proj_pixels[i] = sf::Vector2i(x, y);
					i += 1;
				}
				else
				{
					tet_image.setPixel(x, y, sf::Color::Transparent);
				}
			}
			else
			{
				tet_image.setPixel(x, y, sf::Color::Transparent);
			}

		}
	}

	for (size_t i = 0; i < 4; i++)
	{
		if (drop_proj_pixels[i] != sf::Vector2i(0, 0))
		{
			tet_image.setPixel(drop_proj_pixels[i].x, drop_proj_pixels[i].y, tet.color);
		}
	}

	// Mark masked projection pixels
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			if (drop_proj_pixels[i] == masked_proj_pixels[j])
			{
				drop_proj_pixels[i] = sf::Vector2i(0, 0);
			}
		}
	}
}

void LoadAudio()
{
	if (!sound_buffers[0].loadFromFile("resources/audio/soundfx/tetris_game_over.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	if (!sound_buffers[1].loadFromFile("resources/audio/soundfx/tetris_line_clear.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	if (!sound_buffers[2].loadFromFile("resources/audio/soundfx/tetris_move.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	if (!sound_buffers[3].loadFromFile("resources/audio/soundfx/tetris_new_game.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	if (!sound_buffers[4].loadFromFile("resources/audio/soundfx/tetris_next_level.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	if (!sound_buffers[5].loadFromFile("resources/audio/soundfx/tetris_place.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	if (!sound_buffers[6].loadFromFile("resources/audio/soundfx/tetris_rotate.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	if (!sound_buffers[7].loadFromFile("resources/audio/soundfx/tetris_tetris.wav"))
	{
		std::cout << "Couldnt load sound file!\n";
	}

	sf::Listener::setGlobalVolume(20.0f);
}

void Init()
{
	shapes[0] = shape_S;
	shapes[1] = shape_L;
	shapes[2] = shape_I;
	shapes[3] = shape_T;
	shapes[4] = shape_O;

	LoadAudio();

	if (!retro_font.loadFromFile("resources/font/retro_gaming.ttf"))
	{
		std::cout << "Couldn't load font!\n";
	}

	uint32_t char_size = 35 * (WINDOW_WIDTH / 360.0f);
	std::cout << "Character size: " << char_size << std::endl;
	score_text.setFont(retro_font);
	score_text.setCharacterSize(char_size);
	score_text.setFillColor(sf::Color::White);
	score_text.setString("SCORE\n" + std::to_string(score));

	level_text.setFont(retro_font);
	level_text.setCharacterSize(char_size);
	level_text.setFillColor(sf::Color::White);
	level_text.setString("LEVEL \n" + std::to_string(level));

	lines_text.setFont(retro_font);
	lines_text.setCharacterSize(char_size);
	lines_text.setFillColor(sf::Color::White);
	lines_text.setString("LINES \n" + std::to_string(lines));

	pause_text.setFont(retro_font);
	pause_text.setCharacterSize(char_size);
	pause_text.setFillColor(sf::Color::White);
	pause_text.setString("GAME PAUSED\n");


	score_text.setPosition(WINDOW_WIDTH, 0);
	level_text.setPosition(WINDOW_WIDTH, score_text.getGlobalBounds().top + score_text.getGlobalBounds().height * 1.5f);
	lines_text.setPosition(WINDOW_WIDTH, level_text.getGlobalBounds().top + level_text.getGlobalBounds().height * 1.5f);
	pause_text.setPosition(WINDOW_WIDTH / 2.0f - pause_text.getGlobalBounds().width / 2.0f, WINDOW_HEIGHT / 2.0f);
}

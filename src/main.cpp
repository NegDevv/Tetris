#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "tetris.h"

#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 660


int main()
{
	float SPRITE_SCALE = (float)WINDOW_WIDTH / BOARD_WIDTH;

	sf::RenderWindow tetris_window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tetris");

	sf::Image game_window_image;
	sf::Image board_image;
	sf::Image tet_image;

	game_window_image.create(BOARD_WIDTH, BOARD_HEIGHT);
	board_image.create(BOARD_WIDTH, BOARD_HEIGHT);
	tet_image.create(BOARD_WIDTH, BOARD_HEIGHT);

	sf::Texture game_window_texture;
	sf::Texture board_texture;
	sf::Texture tet_texture;

	game_window_texture.create(BOARD_WIDTH, BOARD_HEIGHT);
	board_texture.create(BOARD_WIDTH, BOARD_HEIGHT);
	tet_texture.create(BOARD_WIDTH, BOARD_HEIGHT);

	sf::Sprite game_window_sprite;
	sf::Sprite board_sprite;
	sf::Sprite tet_sprite;

	game_window_texture.update(game_window_image);
	board_texture.update(board_image);
	tet_texture.update(tet_image);

	game_window_sprite.setScale(sf::Vector2f(SPRITE_SCALE, SPRITE_SCALE));
	board_sprite.setScale(sf::Vector2f(SPRITE_SCALE, SPRITE_SCALE));
	tet_sprite.setScale(sf::Vector2f(SPRITE_SCALE, SPRITE_SCALE));

	game_window_sprite.setTexture(game_window_texture);
	board_sprite.setTexture(game_window_texture);
	tet_sprite.setTexture(game_window_texture);


	//uint16_t* representations[5];
	shapes[0] = shape_S;
	shapes[1] = shape_L;
	shapes[2] = shape_I;
	shapes[3] = shape_T;
	shapes[4] = shape_O;

	sf::Clock clock;
	int tickRate = 1;
	float updateInterval = 1.0f / tickRate;
	timer = 0;
	paused = false;

	Tetromino current_tet;
	SpawnNewTet(current_tet);

	// Initialize images
	for (size_t y = 0; y < BOARD_HEIGHT; y++)
	{
		for (size_t x = 0; x < BOARD_WIDTH; x++)
		{
			if (board[y] & (1 << (BOARD_WIDTH - x) + 1))
			{
				//game_window_image.setPixel(x, y, block_colors[Grey]);
				board_image.setPixel(x, y, block_colors[Grey]);
			}
			tet_image.setPixel(x, y, sf::Color::Transparent);
		}
	}

	// Application loop
	while (tetris_window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		sf::Event event;

		// Event handling
		while (tetris_window.pollEvent(event))
		{
			HandleInputEvent(tetris_window, event, current_tet, board, board_image);
		}

		// Update
		if (timer > updateInterval && !paused)
		{
			Update(current_tet, board, board_image);
		}

		// Draw currently handled tetromino
		for (size_t y = 0; y < BOARD_HEIGHT; y++)
		{
			for (size_t x = 0; x < BOARD_WIDTH; x++)
			{
				if (y >= current_tet.y && y < current_tet.y + 4)
				{
					if (current_tet.shape_current[(y - current_tet.y)] & (1 << (BOARD_WIDTH - x + 1)))
					{
						tet_image.setPixel(x, y, current_tet.color);
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

		//game_window_texture.update(game_window_image);
		board_texture.update(board_image);
		tet_texture.update(tet_image);

		//game_window_sprite.setTexture(game_window_texture);
		board_sprite.setTexture(board_texture);
		tet_sprite.setTexture(tet_texture);

		// Draw
		tetris_window.clear();
		//tetris_window.draw(game_window_sprite);
		tetris_window.draw(board_sprite);
		tetris_window.draw(tet_sprite);
		tetris_window.display();
	}

	return 0;
}
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "tetris.h"
#include <iostream>


int main()
{
	float SPRITE_SCALE = (float)WINDOW_WIDTH / BOARD_WIDTH;

	sf::RenderWindow tetris_window(sf::VideoMode(WINDOW_WIDTH + WINDOW_WIDTH / 2, WINDOW_HEIGHT), "Tetris");

	sf::Image game_window_image;
	sf::Image board_image;
	sf::Image tet_image;

	game_window_image.create(BOARD_WIDTH, BOARD_HEIGHT);
	board_image.create(BOARD_WIDTH, BOARD_HEIGHT);
	tet_image.create(BOARD_WIDTH, BOARD_HEIGHT);

	sf::Texture game_window_texture;
	sf::Texture board_texture;
	sf::Texture tet_texture;
	sf::Texture block_proj_texture;

	game_window_texture.create(BOARD_WIDTH, BOARD_HEIGHT);
	board_texture.create(BOARD_WIDTH, BOARD_HEIGHT);
	tet_texture.create(BOARD_WIDTH, BOARD_HEIGHT);
	block_proj_texture.create(1, 1);

	sf::Sprite game_window_sprite;
	sf::Sprite board_sprite;
	sf::Sprite tet_sprite;
	sf::Sprite drop_proj_sprite;
	sf::Sprite block_proj_sprite;

	game_window_texture.update(game_window_image);
	board_texture.update(board_image);
	tet_texture.update(tet_image);

	sf::Uint8 pixels[4] = { 255, 255, 255, 255 };
	block_proj_texture.update(pixels);

	game_window_sprite.setScale(sf::Vector2f(SPRITE_SCALE, SPRITE_SCALE));
	board_sprite.setScale(sf::Vector2f(SPRITE_SCALE, SPRITE_SCALE));
	tet_sprite.setScale(sf::Vector2f(SPRITE_SCALE, SPRITE_SCALE));

	float block_proj_scale = 0.9f;
	block_proj_sprite.setScale(sf::Vector2f(SPRITE_SCALE, SPRITE_SCALE) * block_proj_scale);

	game_window_sprite.setTexture(game_window_texture);
	board_sprite.setTexture(board_texture);
	tet_sprite.setTexture(game_window_texture);

	block_proj_sprite.setTexture(block_proj_texture);
	block_proj_sprite.setColor(sf::Color::Black);

	Init();

	sf::Clock clock;
	int tick_rate = 1;
	float update_interval = 1.0f / tick_rate;
	timer = 0;
	paused = false;
	score = 0;
	level = 1;

	const int pixels_per_block = WINDOW_WIDTH / BOARD_WIDTH;
	const int pixel_offset = pixels_per_block * (1.0f - block_proj_scale) / 2.0f;

	Tetromino current_tet;
	SpawnNewTet(current_tet);

	sf::Vector2i drop_proj_pixels[4];

	// Initialize images
	for (size_t y = 0; y < BOARD_HEIGHT; y++)
	{
		for (size_t x = 0; x < BOARD_WIDTH; x++)
		{
			if (board[y] & (1 << (BOARD_WIDTH - x + 1)))
			{
				board_image.setPixel(x, y, block_colors[Grey]);
			}
			tet_image.setPixel(x, y, sf::Color::Transparent);
		}
	}

	Update(current_tet, board, board_image, drop_proj_pixels);
	DropReprojection(current_tet, board, drop_proj_pixels);

	DrawTet(current_tet, tet_image, drop_proj_pixels);

	tetris_window.clear();
	
	board_texture.update(board_image);
	tet_texture.update(tet_image);

	board_sprite.setTexture(board_texture);
	tet_sprite.setTexture(tet_texture);

	tetris_window.draw(board_sprite);
	tetris_window.draw(tet_sprite);

	tetris_window.draw(score_text);
	tetris_window.draw(level_text);
	tetris_window.draw(lines_text);
	tetris_window.draw(top_score_text);

	for (size_t i = 0; i < 4; i++)
	{
		block_proj_sprite.setPosition(drop_proj_pixels[i].x * pixels_per_block + pixel_offset, drop_proj_pixels[i].y * pixels_per_block + pixel_offset);
		tetris_window.draw(block_proj_sprite);
	}

	tetris_window.display();

	// Application loop
	while (tetris_window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		if (!paused)
		{
			timer += time;
		}
		

		sf::Event event;

		// Event handling
		while (tetris_window.pollEvent(event))
		{
			if (event.type != sf::Event::MouseMoved)
			{
				HandleInputEvent(tetris_window, event, current_tet, board, board_image, drop_proj_pixels);


				DrawTet(current_tet, tet_image, drop_proj_pixels);

				tetris_window.clear();

				board_texture.update(board_image);
				tet_texture.update(tet_image);

				board_sprite.setTexture(board_texture);
				tet_sprite.setTexture(tet_texture);

				tetris_window.draw(board_sprite);
				tetris_window.draw(tet_sprite);

				// Draw projection pixel masks
				for (size_t i = 0; i < 4; i++)
				{
					if (drop_proj_pixels[i] != sf::Vector2i(0, 0))
					{
						block_proj_sprite.setPosition(drop_proj_pixels[i].x * pixels_per_block + pixel_offset, drop_proj_pixels[i].y * pixels_per_block + pixel_offset);
						tetris_window.draw(block_proj_sprite);
					}
				}

				tetris_window.draw(score_text);
				tetris_window.draw(level_text);
				tetris_window.draw(lines_text);
				tetris_window.draw(top_score_text);

				if (paused)
				{
					tetris_window.draw(pause_text);
				}

				tetris_window.display();
			}
		}

		// Update
		if (timer > update_interval && !paused)
		{
			Update(current_tet, board, board_image, drop_proj_pixels);

			DrawTet(current_tet, tet_image, drop_proj_pixels);

			tetris_window.clear();

			board_texture.update(board_image);
			tet_texture.update(tet_image);

			board_sprite.setTexture(board_texture);
			tet_sprite.setTexture(tet_texture);

			tetris_window.draw(board_sprite);
			tetris_window.draw(tet_sprite);

			for (size_t i = 0; i < 4; i++)
			{
				if (drop_proj_pixels[i] != sf::Vector2i(0, 0))
				{
					block_proj_sprite.setPosition(drop_proj_pixels[i].x * pixels_per_block + pixel_offset, drop_proj_pixels[i].y * pixels_per_block + pixel_offset);
					tetris_window.draw(block_proj_sprite);
				}
			}

			tetris_window.draw(score_text);
			tetris_window.draw(level_text);
			tetris_window.draw(lines_text);
			tetris_window.draw(top_score_text);

			tetris_window.display();
		}
	}

	return 0;
}
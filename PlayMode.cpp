#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include "load_save_png.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"
#include "PPU466.hpp"

#include <fstream>
#include <iostream>
#include <random>

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

    std::string filename = data_path("data/car.png"); // Replace with your actual PNG file path
    glm::uvec2 ss_size;
    std::vector<glm::u8vec4> ss_data; // Vector to hold the RGBA pixel data

    load_png(filename, &ss_size, &ss_data, UpperLeftOrigin);
	
	std::vector<glm::u8vec4> unique_colors;

	for (const auto& pixel : ss_data) {
        if (std::find(unique_colors.begin(), unique_colors.end(), pixel) == unique_colors.end()) {
            unique_colors.push_back(pixel);
        }
    }

	assert(unique_colors.size() <= 4);
	
	uint8_t index = 0;
	for (const auto& color : unique_colors) {
        ppu.palette_table[0][index] = color;
        index++;
    }
    
    const uint32_t TILE_SIZE = 8;  

    uint32_t tiles_x = ss_size.x / TILE_SIZE;
    uint32_t tiles_y = ss_size.y / TILE_SIZE;

    for (uint32_t tile_y = 0; tile_y < tiles_y; ++tile_y) {
        for (uint32_t tile_x = 0; tile_x < tiles_x; ++tile_x) {
            PPU466::Tile tile;
            std::fill(std::begin(tile.bit0), std::end(tile.bit0), 0);  
            std::fill(std::begin(tile.bit1), std::end(tile.bit1), 0);  

            for (uint32_t row = 0; row < TILE_SIZE; ++row) {
                uint8_t bit0 = 0;
                uint8_t bit1 = 0;

                for (uint32_t col = 0; col < TILE_SIZE; ++col) {
                    uint32_t pixel_x = tile_x * TILE_SIZE + col;
                    uint32_t pixel_y = tile_y * TILE_SIZE + row;
                    glm::u8vec4 pixel = ss_data[pixel_y * ss_size.x + pixel_x];

					if (pixel == ppu.palette_table[0][0]) {         
						
					} else if (pixel == ppu.palette_table[0][1]) {  
						bit0 = bit0 | 1;
					} else if (pixel == ppu.palette_table[0][2]) {     
						bit1 = bit1 | 1;
					} else {                        
						bit0 = bit0 | 1;
						bit1 = bit1 | 1;
					}

					if(col < 7){
						bit0 = bit0 << 1;
						bit1 = bit1 << 1;
					}
                }

                tile.bit0[row] = bit0;
                tile.bit1[row] = bit1;
            }

            uint32_t tile_index = tile_y * tiles_x + tile_x;
            ppu.tile_table[tile_index] = tile;  // Store the tile in the tile_table
        }
    }

	for (uint8_t y = 0; y < PPU466::BackgroundHeight; ++y) {
        for (uint8_t x = 0; x < PPU466::BackgroundWidth; ++x) {
            uint8_t tile_index = x;
            uint8_t palette_index = 0;   

            uint16_t background_value = (palette_index << 8) | tile_index;

            ppu.background[y * PPU466::BackgroundWidth + x] = background_value;
        }
    }
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	ppu.draw(drawable_size);
}

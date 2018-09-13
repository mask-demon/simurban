#include <cstdio>
#include <cstdlib>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <cmath>

#include <SDL2/SDL.h>

int windowWidth = 256, windowHight = 256;
int hight = 256, width = 256;
int interval = 100;
const int minSquare = 4000, maxSquare = 10000;

SDL_Window *window;
SDL_Renderer *renderer;

int init() {
	SDL_Init(SDL_INIT_VIDEO| SDL_INIT_EVENTS);
	window = SDL_CreateWindow( "SimUrban", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHight, SDL_WINDOW_RESIZABLE);
	if(!window) {
		fprintf(stderr, "Could not create a window: %s\n", SDL_GetError());
		return 1;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		fprintf(stderr, "Could not create a renderer: %s\n", SDL_GetError());
		return 2;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	printf("Simurban.\n");
	fflush(stdout);
	if(init()) return 1;
	SDL_Surface *surface = SDL_CreateRGBSurface(0, width, hight, 32, 0, 0, 0, 0);
	SDL_Rect rect;
	rect.h = rect.w = 1;
	/*
	for(int i = 0; i < 256; i++) {
		rect.y = i;
		for(int j = 0; j < 256; j++) {
			rect.x = j;
			SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, (i + 128) / 2, (j + 128) / 2, (255 - (i + j) / 2 + 128) / 2));
		}
	}
	*/
	for(int i = 0; i < 256; i++) {
		rect.y = i;
		for(int j = 0; j < 256; j++) {
			rect.x = j;
			int distSquare = (i * 2 - 255) * (i * 2 - 255) + (j * 2 - 255) * (j * 2 - 255);
			if(maxSquare * 4 < distSquare) SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
			else {
				double ang = atan2(i * 2 - 255, j * 2 - 255);
				SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, std::min(255, (int)(128 * (1 + sin(ang)))), std::min(255, (int)(128 * (1 + sin(ang + M_PI * 2 / 3)))), std::min(255, (int)(128 * (1 + sin(ang + M_PI * 4 / 3))))));
			}
		}
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	int h, w;
	SDL_GetRendererOutputSize(renderer, &w, &h);
	rect.y = rect.x = 0;
	rect.h = rect.w = std::min(h, w);
	SDL_RenderCopy(renderer, texture, 0, &rect);
	SDL_RenderPresent(renderer);

	SDL_Event event;
	while(true) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) return 0;
			if(event.type == SDL_WINDOWEVENT) {
				SDL_GetRendererOutputSize(renderer, &w, &h);
				rect.y = rect.x = 0;
				rect.h = rect.w = std::min(h, w);
				SDL_RenderCopy(renderer, texture, 0, &rect);
				SDL_RenderPresent(renderer);
			}
		}
		SDL_Delay(interval);
	}
	return 0;
}

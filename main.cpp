#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int windowWidth = 256, windowHight = 256;
int hight = 256, width = 256;
const int unitLength = 256;
const int iconSize = 32;
const int interval = 40;
const int numLevel = 15;
const int sizes[numLevel] = {16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048};

double angularVelocity = 0.1;
const int maxSquare = 10000;

SDL_Window *window;
SDL_Renderer *renderer;

int init() {
	SDL_Init(SDL_INIT_VIDEO| SDL_INIT_EVENTS);
	IMG_Init(IMG_INIT_PNG);
	window = SDL_CreateWindow( "SimUrban", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHight, SDL_WINDOW_RESIZABLE);
	if(!window) {
		fprintf(stderr, "Could not create a window: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetWindowMinimumSize(window, iconSize, iconSize);
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		fprintf(stderr, "Could not create a renderer: %s\n", SDL_GetError());
		return 2;
	}
	return 0;
}

void my_RenderCopy(SDL_Renderer *R, SDL_Texture *T, const int Y, const int X, const int S) { //(Y, X): coordination of the center
	if(S < 0 || S >= numLevel) return;
	SDL_Rect rectR, rectT;
	int rh, rw, th, tw;
	SDL_GetRendererOutputSize(R, &rw, &rh);
	SDL_QueryTexture(T, 0, 0, &tw, &th);
	
	//vertical
	rectT.y = Y - rh * unitLength / sizes[S] / 2;
	rectT.h = rh * unitLength / sizes[S];
	if(Y * sizes[S] * 2 >= rh * unitLength) rectR.y = 0;
	else rectR.y = rh / 2 - Y * sizes[S] / unitLength;
	if((th - Y) * sizes[S] * 2 >= rh * unitLength) rectR.h = rh - rectR.y;
	else rectR.h = rh / 2 + (th - Y) * sizes[S] / unitLength - rectR.y;
	
	//horizontal
	rectT.x = X - rw * unitLength / sizes[S] / 2;
	rectT.w = rw * unitLength / sizes[S];
	if(X * sizes[S] * 2 >= rw * unitLength) rectR.x = 0;
	else rectR.x = rw / 2 - X * sizes[S] / unitLength;
	if((tw - X) * sizes[S] * 2 >= rw * unitLength) rectR.w = rw - rectR.x;
	else rectR.w = rw / 2 + (tw - X) * sizes[S] / unitLength - rectR.x;

	SDL_RenderCopy(R, T, &rectT, &rectR);
}

int main(int argc, char *argv[]) {
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
	
	SDL_Surface *surfaceIconOn  = IMG_Load("icon/start_on.png"); 
	SDL_Surface *surfaceIconOff = IMG_Load("icon/start_off.png"); 
	SDL_Texture *textureIconOn  = SDL_CreateTextureFromSurface(renderer, surfaceIconOn);
	SDL_Texture *textureIconOff = SDL_CreateTextureFromSurface(renderer, surfaceIconOff);

	int size = 8;
	int ypos = hight / 2, xpos = width / 2;
	bool startOn = false;
	bool rewrite = true;
	double phase = 0;
	SDL_Event event;
	while(true) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) return 0;
			if(event.type == SDL_MOUSEWHEEL) {
				size -= event.wheel.y;
				if(size >= numLevel) size = numLevel - 1;
				if(size < 0) size = 0;
				rewrite = true;
			} else if(event.type == SDL_MOUSEMOTION) {
				if(event.motion.state & SDL_BUTTON_LMASK) {
					ypos -= event.motion.yrel * unitLength / sizes[size];
					xpos -= event.motion.xrel * unitLength / sizes[size];
					rewrite = true;
				}
			} else if(event.type == SDL_KEYUP) {
				if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					startOn = !startOn;
					rewrite = true;
				}
			} else if(event.type == SDL_WINDOWEVENT) rewrite = true;
			if(rewrite) {
				SDL_RenderClear(renderer);
				my_RenderCopy(renderer, texture, ypos, xpos, size);
				rect.y = rect.x = 0;
				rect.w = rect.h = iconSize;
				SDL_RenderCopy(renderer, startOn? textureIconOn: textureIconOff, 0, &rect);
				SDL_RenderPresent(renderer);
				rewrite = false;
			}
		}		
		{
			if(startOn) {
				phase += angularVelocity;
				rect.h = rect.w = 1;
				for(int i = 0; i < 256; i++) {
					rect.y = i;
					for(int j = 0; j < 256; j++) {
						rect.x = j;
						int distSquare = (i * 2 - 255) * (i * 2 - 255) + (j * 2 - 255) * (j * 2 - 255);
						if(maxSquare * 4 < distSquare) SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
						else {
							double ang = atan2(i * 2 - 255, j * 2 - 255) + phase;
							SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, std::min(255, (int)(128 * (1 + sin(ang)))), std::min(255, (int)(128 * (1 + sin(ang + M_PI * 2 / 3)))), std::min(255, (int)(128 * (1 + sin(ang + M_PI * 4 / 3))))));
						}
					}
				}
				SDL_DestroyTexture(texture);
				texture = SDL_CreateTextureFromSurface(renderer, surface);
			}
			SDL_RenderClear(renderer);
			my_RenderCopy(renderer, texture, ypos, xpos, size);
			rect.y = rect.x = 0;
			rect.w = rect.h = iconSize;
			SDL_RenderCopy(renderer, startOn? textureIconOn: textureIconOff, 0, &rect);
			SDL_RenderPresent(renderer);
			rewrite = false;
		}
		SDL_Delay(interval);
	}
	return 0;
}

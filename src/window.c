#include "../includes/window.h"

Window *CreateWindow(){
    Window *window = malloc(sizeof(window));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(-1);
    }

    window->sdl_window = SDL_CreateWindow("SERVEUR_SOCKET", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window->sdl_window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s)n", SDL_GetError());
        SDL_Quit();
        exit(-1);
    }
    window->renderer = SDL_CreateRenderer(window->sdl_window, -1, SDL_RENDERER_ACCELERATED);
    if (window->renderer == NULL)
    {
        printf("Renderer could not be created! SDL_Error: %s)n", SDL_GetError());
        SDL_DestroyWindow(window->sdl_window);
        SDL_Quit();
        exit(-1);
    }

    SDL_SetRenderDrawColor(window->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(window->renderer);
    SDL_RenderPresent(window->renderer);

    return window;

}

void DestroyWindow(Window *window){
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->sdl_window);
    free(window);
}


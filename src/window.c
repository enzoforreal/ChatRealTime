// fichier window.c
#include "../includes/window.h"
#include <SDL2/SDL_ttf.h>

Window *CreateWindow()
{
    Window *window = malloc(sizeof(window));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) || TTF_Init() != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        SDL_Quit();
        TTF_Quit();
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

    // chargement de la police de caractère TTF

    TTF_Font *font = TTF_OpenFont("./fonts/arial.ttf", 20);

    if (font == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(window->renderer);
        SDL_DestroyWindow(window->sdl_window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    window->font = font;

    SDL_SetRenderDrawColor(window->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(window->renderer);
    // Render a small colored rectangle to test if the window is being updated
    SDL_Rect rect = {50, 50, 50, 50};
    SDL_SetRenderDrawColor(window->renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(window->renderer, &rect);

    SDL_RenderPresent(window->renderer);

    return window;
}

void DestroyWindow(Window *window)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->sdl_window);
    free(window);
}

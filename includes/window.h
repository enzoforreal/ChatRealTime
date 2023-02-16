// fichier window.h
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
    SDL_Window *sdl_window;
    SDL_Renderer *renderer;
    TTF_Font *font;
} Window;

typedef struct
{
    int client_socket;
    Window *window;
    TTF_Font *font;
} ClientThreadArgs;

Window *CreateWindow();

void DestroyWindow(Window *window);
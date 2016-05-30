/*************************************************************************************/
/*************************************************************************************/
/**                                                                                 **/
/**  Tinyplayer - TibV2 example                                                     **/
/**  written by Tammo 'kb' Hinrichs 2000-2008                                       **/
/**  This file is in the public domain                                              **/
/**  "Patient Zero" is (C) Melwyn+LB 2005, do not redistribute                      **/
/**                                                                                 **/
/*************************************************************************************/
/*************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "v2mplayer.h"
#include "libv2.h"

static V2MPlayer player;

////////////////////////////////////////////////////////////////////////////////////////////////////
static void sdl_callback(void *userdata, Uint8 * stream, int len) {
    player.Render((float*) stream, len / 8);
}

static bool init_sdl() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("Couldn't initialize SDL: %s\n",SDL_GetError());
        SDL_Quit();
        return false;
    }

    SDL_AudioSpec desired, actual;
    desired.channels = 2;
    desired.freq = 44100;
    desired.samples = 4096;
    desired.format = AUDIO_F32;
    desired.callback = sdl_callback;
    if (SDL_OpenAudio(&desired, &actual)) {
        SDL_Log("Failed to open audio, %s\n", SDL_GetError());
        return false;
    }

    return true;
}

int main(int argc, const char** argv) {
    printf("Farbrausch Tiny Music Player v0.dontcare TWO\n");
    printf("Code and Synthesizer (C) 2000-2008 kb/Farbrausch\n");
    printf("SDL Port by github.com/jgilje\n\n");

    if (argc <= 1) {
        printf("usage: %s V2M_file\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Failed to open %s\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* theTune = (char*) calloc(1, size);

    size_t read = fread(theTune, 1, size, file);
    if (size != read) {
        fprintf(stderr, "Invalid read size\n");
        return 1;
    }

    player.Init();
    player.Open(theTune);

    printf("Now Playing: %s\n", argv[1]);

    if (! init_sdl()) {
        return 1;
    }

    player.Play();
    SDL_PauseAudio(0);

    printf("\n\npress Enter to quit\n");
    getc(stdin);

    SDL_PauseAudio(1);
    SDL_Quit();
    player.Close();
    return 0;
}

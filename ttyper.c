#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

#define WIDTH 1600
#define HEIGHT 1200

char* getCurTime() {
    time_t timer;
    time(&timer);
    char* timeStr = ctime(&timer);
    return timeStr;
}

typedef struct gameStat {
    Font MonoFont;
    char* Prompt;
    char* Str;
    float charWidth;
    int letterCount;
    Vector2 StrPosition;
}gameStat;


void ShowStr(const gameStat* gs,char* showStr) {
    int len = strlen(gs->Prompt);
    int col = 0, row = 0;
    int rowNum = WIDTH / gs->charWidth - 2;
    // Draw Prompt word
    for (int i = 0; i < len; i++) {
        Vector2 curPosition = {gs->StrPosition.x+gs->charWidth*col,gs->StrPosition.y+100.f*row};

        if (gs->Prompt[i] == '\n') {
            DrawTextEx(gs->MonoFont, " `E", curPosition, 32, 1, GRAY);
            col = 0;
            row++;
            continue;
        }

        char c[2] = {gs->Prompt[i], '\0'};
        if (gs->Prompt[i] == ' ') DrawTextEx(gs->MonoFont, "_", curPosition, 64, 1, GRAY);
        else DrawTextEx(gs->MonoFont, c, curPosition, 64, 1, GRAY);

        col++;
        if (col / rowNum) {
            row++;
            col = 0;
        }
    }

    col = 0;
    row = 0;
    for (int i = 0; i < gs->letterCount; i++) {
        Vector2 curPosition = {gs->StrPosition.x+gs->charWidth*col,gs->StrPosition.y+100.f*row};

        if (gs->Prompt[i] == '\n') {
            if (gs->Str[i] == '\n') DrawTextEx(gs->MonoFont, " `E", curPosition, 32, 1, PURPLE);
            else DrawTextEx(gs->MonoFont, " `E", curPosition, 32, 1, RED);
            col = 0;
            row++;
            continue;
        }

        char c[2] = {gs->Prompt[i], '\0'};

        // Draw covering str
        if (gs->Str[i] == gs->Prompt[i]) {
            if (gs->Prompt[i] == ' ') DrawTextEx(gs->MonoFont, "_", curPosition, 64, 1, WHITE);
            else DrawTextEx(gs->MonoFont, c, curPosition, 64, 1, WHITE);
        }
        // Draw wrong str
        else {
            if (gs->Prompt[i] == ' ') DrawTextEx(gs->MonoFont, "_", curPosition, 64, 1, RED);
            else DrawTextEx(gs->MonoFont, c, curPosition, 64, 1, RED);
        }

        col++;
        if (col / rowNum) {
            row++;
            col = 0;
        }
    }
}

int main(int argc, char *argv[])
{
    // char Prompt[] = "Hello World --xfw";
    // char* Prompt = "Hello World! My name is xfw,I like playing computer games.";
    char* Prompt = "A model`s\necosystem position\nis not defined only by its own releases, but by how much the community builds on top of it. As mentioned above, Qwen is one exception which is getting attention and adoption.";
    int MAX_INPUT_SIZE = (int)strlen(Prompt);

    char* Str = (char*)malloc(sizeof(char)*MAX_INPUT_SIZE+1);
    Str[MAX_INPUT_SIZE] = '\0';
    int letterCount = 0;

    InitWindow(WIDTH, HEIGHT, "ttyper -c -raylib");
    InitAudioDevice();

    // Load the background
    Texture background = LoadTexture("./resources/Hyper-Girl.png");

    // Load the fonts
    Font MonoFont = LoadFontEx("./resources/FiraCodeNerdFontMono-Bold.ttf",96,NULL,0);
    SetTextureFilter(MonoFont.texture, TEXTURE_FILTER_BILINEAR);
    // Get the char width
    float charWidth = MeasureTextEx(MonoFont, "M", 64, 1).x + 1; // + spacing

    // Load the keyboard sounds
    Sound KeySound = LoadSound("./resources/sound/keyboard.mp3");
    Sound KeySoundB = LoadSound("./resources/sound/backspace.mp3");
    Sound KeySoundS = LoadSound("./resources/sound/space.mp3");

    // Load the musics
    Music bgm = LoadMusicStream("./resources/sound/bgm.mp3");
    bgm.looping = true;
    SetMusicVolume(bgm, 0.04);
    PlayMusicStream(bgm);

    // Init line position
    Vector2 StrPosition = {charWidth,100.f}; // One key`s gap

    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        UpdateMusicStream(bgm);
        // Get the keyWord
        char key = GetCharPressed();

        // Adjust the keyWord
        if ((key >= 32) && (key <= 126) && (letterCount < MAX_INPUT_SIZE)) {
            Str[letterCount++] = key;
            Str[letterCount] = '\0';
            if (key == 32) PlaySound(KeySoundS);
            else PlaySound(KeySound);
        }
        key = GetCharPressed();

        // If backspace
         if (IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            Str[letterCount] = '\0';
            PlaySound(KeySoundB);
        }

        // If enter
        if (IsKeyPressed(KEY_ENTER)) {
            Str[letterCount++] = '\n';
            Str[letterCount] = '\0';
            PlaySound(KeySoundS);
        }

        // Get the str covering the Prompt
        char *showStr = (char*)malloc(sizeof(char)*MAX_INPUT_SIZE+1);
        strcpy(showStr,Prompt);
        showStr[letterCount] = '\0';
        gameStat gs = {MonoFont,Prompt,Str,charWidth,letterCount,StrPosition};

        BeginDrawing();
            // ClearBackground(WHITE);
            DrawTexture(background, 0, 0, WHITE);

            ShowStr(&gs, showStr);
            EndDrawing();
            free(showStr);
            showStr = NULL;
    }
    UnloadFont(MonoFont);
    UnloadSound(KeySound);
    UnloadSound(KeySoundB);
    UnloadSound(KeySoundS);
    UnloadMusicStream(bgm);
    UnloadTexture(background);
    CloseAudioDevice();
    CloseWindow();
    free(Str);
    Str = NULL;
    return EXIT_SUCCESS;
}

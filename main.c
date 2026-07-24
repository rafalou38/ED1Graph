#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <math.h>
#include <stdio.h>

#include "tinyexpr/tinyexpr.h"

const int grid_w = 50;
const int grid_h = 30;
const int cell_sz = 30;
const int text_sz = 50;

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Vector2 plot2screen(Vector2 v) {
  return (Vector2){(v.x + grid_w / 2.0) * cell_sz,
                   (grid_h / 2.0 - v.y) * cell_sz};
}

Vector2 screen2plot(Vector2 v) {
  return (Vector2){(v.x / cell_sz) - grid_w / 2.0,
                   grid_h / 2.0 - (v.y / cell_sz)};
}
char eq[255];
double x, y;
te_variable vars[] = {{"x", &x}, {"y", &y}};
int err;
te_expr *expr;

const int screenWidth = grid_w * cell_sz;
const int screenHeight = grid_h * cell_sz;
Font roboto;

bool editMode = false;

void draw() {
  BeginDrawing();

  ClearBackground(RAYWHITE);

  Color c = {0, 0, 0, 255};
  int thick = 1;

  for (int i = 1; i < grid_w; i++) {
    if (i % 5 == 0)
      c.a = 255;
    else
      c.a = 50;
    if (i == grid_w / 2)
      thick = 3;
    else
      thick = 1;

    DrawLineEx((Vector2){i * cell_sz, 0}, (Vector2){i * cell_sz, screenHeight},
               thick, c);
  }

  for (int i = 1; i < grid_h; i++) {
    Color c = {0, 0, 0, 255};
    if (i % 5 == 0)
      c.a = 255;
    else
      c.a = 50;

    if (i == grid_h / 2)
      thick = 3;
    else
      thick = 1;

    DrawLineEx((Vector2){0, i * cell_sz}, (Vector2){screenWidth, i * cell_sz},
               thick, c);
  }

  for (int i = 0; i < grid_w; i += 1) {
    for (int j = 0; j < grid_h; j += 1) {
      Vector2 center = {(i + 0.5) - grid_w / 2.0, grid_h / 2.0 - (j + 0.5)};

      x = center.x;
      y = center.y;
      // int d = f(center.x, center.y);
      int d = te_eval(expr);

      Vector2 delta = {1, d};
      delta = Vector2Normalize(delta);

      Vector2 start = Vector2Add(center, Vector2Scale(delta, -0.3));
      Vector2 end = Vector2Add(center, Vector2Scale(delta, 0.3));

      DrawLineEx(plot2screen(start), plot2screen(end), 1,
                 (Color){200, 50, 50, 255});

      DrawCircleV(plot2screen(end), 2, (Color){200, 50, 50, 255});

      // char pos[20];
      // sprintf(pos, "%.0f:%.0f", start.x, start.y);
      // DrawText(pos, center.x, center.y, 5, BLACK);
    }
  }

  Vector2 position = screen2plot(GetMousePosition());
  Vector2 prev = Vector2Zero();

  DrawCircleV(GetMousePosition(), cell_sz * 0.4, BLUE);

  double dt = 0.05;
  for (int i = 0; i < 1000; i++) {
    prev = position;

    x = position.x;
    y = position.y;

    position.x += dt;
    position.y += dt * te_eval(expr);

    DrawLineEx(plot2screen(prev), plot2screen(position), 4, BLUE);
  }

  float labelWidth = 100.0f;
  float height = text_sz * 2.0f;
  // Position at bottom of screen (screenHeight - height)
  float posY = (float)screenHeight;

  // Label Rect: Fixed width on the far left
  Rectangle labelBounds = {20, posY, labelWidth, height};

  // TextBox Rect: Offset X by labelWidth, take up remaining width
  Rectangle boxBounds = {labelWidth, posY, (float)screenWidth - labelWidth,
                         height};

  // -------------------------------------------------------------
  // DRAW CONTROLS
  // -------------------------------------------------------------
  GuiLabel(labelBounds, "y' =");

  bool keyboard_shown = false;
  if (GuiTextBox(boxBounds, eq, 100, editMode)) {
    editMode = !editMode;
    expr = te_compile(eq, vars, 2, &err);
    printf("done\n");
  }
  // GuiTextInputBox((Rectangle){0, screenHeight - 200, screenWidth, 200},
  // "y'=", "toto", eq, 1, "t",false, false);

  char fps_text[50];
  sprintf(fps_text, "%d", GetFPS());
  DrawText(fps_text, 5, 5, 40, BLACK);
  EndDrawing();
}

int main(void) {
  sprintf(eq, "0.1*(x+y)");

  expr = te_compile(eq, vars, 2, &err);

  InitWindow(screenWidth, screenHeight + text_sz * 2, "ED1Graph");

  // SetTargetFPS(60);

  roboto = LoadFontEx("roboto/Roboto-Bold.ttf", text_sz, NULL, 0);
  GuiSetFont(roboto);
  GuiSetStyle(DEFAULT, TEXT_SIZE, text_sz);

  // -------------------------------------------------------------
  // 1. SET PADDING & ALIGNMENT
  // -------------------------------------------------------------
  // Add inner left/right space (in pixels)
  GuiSetStyle(TEXTBOX, TEXT_PADDING, 8);
  // Left-align text inside the box (0 = Left, 1 = Center, 2 = Right)
  GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

  // -------------------------------------------------------------
  // 2. SET BLACK & WHITE COLOR PALETTE
  // -------------------------------------------------------------
  // Color hex values format: 0xRRGGBBAA

  // -- STATE: NORMAL --
  GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, 0xFFFFFFFF);   // White background
  GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, 0x000000FF); // Black border
  GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, 0x000000FF);   // Black text

  // -- STATE: FOCUSED (Hovered) --
  GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, 0xFFFFFFFF); // Light gray background
  GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, 0x000000FF); // Black border
  GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, 0x000000FF);   // Black text

  // -- STATE: PRESSED/ACTIVE (Editing mode) --
  GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED,
              0xFFFFFFFF); // Inverted: Black background
  GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, 0x000000FF); // Black border
  GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, 0x000000FF); // Inverted: White text

  #if defined(PLATFORM_WEB)
    // 0 = match browser frame rate (60fps), 1 = simulate infinite loop
    emscripten_set_main_loop(draw, 0, 1);
    #else
        SetTargetFPS(60);
        while (!WindowShouldClose()) {
            draw();
        }
    #endif
  CloseWindow();

  return 0;
}

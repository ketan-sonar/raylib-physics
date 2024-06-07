#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

typedef struct
{
    Vector2 pos;
    Vector2 vel;
    size_t bounces;
} Ball;

#define DA_INIT_CAP 16
typedef struct
{
    Ball *items;
    size_t count;
    size_t capacity;
} Balls;

#define da_append(da, item)                                                    \
    if ((da)->capacity == 0) {                                                 \
        (da)->items = malloc(sizeof(*(da)->items) * DA_INIT_CAP);              \
        (da)->capacity = DA_INIT_CAP;                                          \
    }                                                                          \
    if ((da)->count >= (da)->capacity) {                                       \
        (da)->capacity *= 2;                                                   \
        (da)->items =                                                          \
            realloc((da)->items, sizeof(*(da)->items) * (da)->capacity);       \
    }                                                                          \
    (da)->items[(da)->count++] = (item);

#define RADIUS 20

#define BG_COLOR CLITERAL(Color){ 0x18, 0x18, 0x18, 0xFF }

#define ELASTICITY_X 0.4
#define ELASTICITY_Y 0.6
#define BOUNCE 1.5
#define BALL_BOUNCE_LIMIT 15

size_t width = 800;
size_t height = 600;

Balls balls = {0};
Vector2 gravity = { .x = 0, .y = 3000 };
bool spawn_mode = false;
Vector2 spawn_pos = {0};
float spawn_start_time = 0.0;
bool first_render = true;

void spawn_ball()
{
    Vector2 pos = GetMousePosition();
    Vector2 vel = Vector2Multiply(Vector2Subtract(pos, spawn_pos), (Vector2){ .x = 3, .y = 3 });
    Ball ball = { .pos = spawn_pos, .vel = vel };
    da_append(&balls, ball);
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);    // Window configuration flags
    InitWindow(width, height, "Game!");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        width = GetScreenWidth();
        height = GetScreenHeight();

        BeginDrawing();
        ClearBackground(BG_COLOR);

        if (first_render) {
            char *text = "Click and Drag to Spawn Balls.";
            int font_size = 24;
            int text_size = MeasureText(text, font_size);
            int center_x = width / 2 - text_size / 2;
            int center_y = height / 2 - font_size / 2;
            DrawText(text, center_x, center_y, font_size, WHITE);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            first_render = false;
            spawn_mode = true;
            spawn_pos = GetMousePosition();
            spawn_start_time = GetTime();
        }

        if (spawn_mode) {
            DrawCircleV(spawn_pos, RADIUS, ColorAlpha(RED, 0.5));
            DrawLineV(spawn_pos, GetMousePosition(), WHITE);
            DrawRing(GetMousePosition(), RADIUS - 2.0, RADIUS, 0, 360, 100, WHITE);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                spawn_ball();
                spawn_mode = false;
            }
        }

        float dt = GetFrameTime();

        for (size_t i = 0; i < balls.count; ++i) {
            Ball *ball = &balls.items[i];
            Color color = RED;
            if (ball->bounces > BALL_BOUNCE_LIMIT) continue;
            else if (ball->bounces > 10) {
                color = ColorAlpha(RED, (BALL_BOUNCE_LIMIT - ball->bounces) / 5.0);
            }

            ball->vel = Vector2Add(ball->vel, Vector2Scale(gravity, dt));
            ball->pos = Vector2Add(ball->pos, Vector2Scale(ball->vel, dt));

            if (ball->pos.x - RADIUS < 0) {
                ball->pos.x = RADIUS;
                ball->vel.x *= -ELASTICITY_X * BOUNCE;
            } else if (ball->pos.x + RADIUS > width) {
                ball->pos.x = width - RADIUS;
                ball->vel.x *= -ELASTICITY_X * BOUNCE;
            }

            if (ball->pos.y - RADIUS < 0) {
                ball->pos.y = RADIUS;
                ball->vel.y *= -ELASTICITY_Y * BOUNCE;
                ball->bounces++;
            } else if (ball->pos.y + RADIUS > height) {
                ball->pos.y = height - RADIUS;
                ball->vel.y *= -ELASTICITY_Y * BOUNCE;
                ball->bounces++;
            }

            DrawCircleV(ball->pos, RADIUS, color);
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}

#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

typedef struct
{
    Vector2 pos;
    Vector2 vel;
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

#define WIDTH 800
#define HEIGHT 600

#define RADIUS 20

#define BG_COLOR CLITERAL(Color){ 0x18, 0x18, 0x18, 0xFF }

#define ELASTICITY_X 0.4
#define ELASTICITY_Y 0.6
#define BOUNCE 1.25

Balls balls = {0};
Vector2 gravity = { .x = 0, .y = 1000 };
bool spawn_mode = false;
Vector2 spawn_pos = {0};
float spawn_start_time = 0.0;

void spawn_ball()
{
    float delta = GetTime() - spawn_start_time;
    Vector2 pos = GetMousePosition();
    Vector2 vel = Vector2Scale(Vector2Subtract(pos, spawn_pos), 1.0 / delta);
    Ball ball = { .pos = spawn_pos, .vel = vel };
    da_append(&balls, ball);
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Game!");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            spawn_mode = true;
            spawn_pos = GetMousePosition();
            spawn_start_time = GetTime();
        }

        if (spawn_mode) {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                spawn_ball();
                spawn_mode = false;
            }
        }

        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BG_COLOR);

        for (size_t i = 0; i < balls.count; ++i) {
            Ball *ball = &balls.items[i];
            if (ball->pos.x - RADIUS < 0) {
                ball->pos.x = RADIUS;
                ball->vel.x *= -ELASTICITY_X * BOUNCE;
            } else if (ball->pos.x + RADIUS > WIDTH) {
                ball->pos.x = WIDTH - RADIUS;
                ball->vel.x *= -ELASTICITY_X * BOUNCE;
            }

            if (ball->pos.y - RADIUS < 0) {
                ball->pos.y = RADIUS;
                ball->vel.y *= -ELASTICITY_Y * BOUNCE;
            } else if (ball->pos.y + RADIUS > HEIGHT) {
                ball->pos.y = HEIGHT - RADIUS;
                ball->vel.y *= -ELASTICITY_Y * BOUNCE;
            }

            ball->vel = Vector2Add(ball->vel, Vector2Scale(gravity, dt));
            ball->pos = Vector2Add(ball->pos, Vector2Scale(ball->vel, dt));

            DrawCircleV(ball->pos, RADIUS, RED);
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}

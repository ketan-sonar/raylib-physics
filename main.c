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

#define BALL_RADIUS 20
#define BALL_MASS   20

#define BG_COLOR CLITERAL(Color){ 0x18, 0x18, 0x18, 0xFF }

#define ELASTICITY_X 0.4
#define ELASTICITY_Y 0.6
#define BOUNCE       1.5

#define BALL_BOUNCE_LIMIT 15

#define ONE_METER 1080

size_t width = 1080;
size_t height = 768;

Balls balls = {0};
Vector2 gravity = { .x = 0, .y = 9.81 * ONE_METER };
bool spawn_mode = false;
Vector2 spawn_pos = {0};
float spawn_start_time = 0.0;
bool first_render = true;

void spawn_ball(void)
{
    Vector2 pos = GetMousePosition();
    Vector2 vel = Vector2Scale(Vector2Subtract(pos, spawn_pos), -9.81);
    Ball ball = { .pos = spawn_pos, .vel = vel };
    da_append(&balls, ball);
}

void render_init_text(void)
{
    char *text = "Click and Drag to Spawn Balls.";
    int font_size = 24;
    int text_size = MeasureText(text, font_size);
    int center_x = width / 2 - text_size / 2;
    int center_y = height / 2 - font_size / 2;
    DrawText(text, center_x, center_y, font_size, WHITE);
}

void start_spawning(void)
{
    first_render = false;
    spawn_mode = true;
    spawn_pos = GetMousePosition();
    spawn_start_time = GetTime();
}

void render_spawning_ball(void)
{
    DrawCircleV(spawn_pos, BALL_RADIUS, ColorAlpha(RED, 0.5));
    DrawLineV(spawn_pos, GetMousePosition(), WHITE);
    DrawRing(GetMousePosition(), BALL_RADIUS - 2.0, BALL_RADIUS, 0, 360, 100, WHITE);
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        spawn_ball();
        spawn_mode = false;
    }
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Game!");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        width = GetScreenWidth();
        height = GetScreenHeight();

        BeginDrawing();
        ClearBackground(BG_COLOR);

        if (first_render) {
            render_init_text();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            start_spawning();
        }

        if (spawn_mode) {
            render_spawning_ball();
        }

        float dt = GetFrameTime();

        for (size_t i = 0; i < balls.count; ++i) {
            Ball *ball = &balls.items[i];
            Color color = RED;
            if (ball->bounces > BALL_BOUNCE_LIMIT) continue;
            else if (ball->bounces > 10)
                color = ColorAlpha(RED, (BALL_BOUNCE_LIMIT - ball->bounces) / 5.0);

            ball->vel = Vector2Add(ball->vel, Vector2Scale(gravity, dt));
            ball->pos = Vector2Add(ball->pos, Vector2Scale(ball->vel, dt));

            if (ball->pos.x - BALL_RADIUS < 0) {
                ball->pos.x = BALL_RADIUS;
                ball->vel.x *= -ELASTICITY_X * BOUNCE;
            } else if (ball->pos.x + BALL_RADIUS > width) {
                ball->pos.x = width - BALL_RADIUS;
                ball->vel.x *= -ELASTICITY_X * BOUNCE;
            }

            if (ball->pos.y - BALL_RADIUS < 0) {
                ball->pos.y = BALL_RADIUS;
                ball->vel.y *= -ELASTICITY_Y * BOUNCE;
                ball->bounces++;
            } else if (ball->pos.y + BALL_RADIUS > height) {
                ball->pos.y = height - BALL_RADIUS;
                ball->vel.y *= -ELASTICITY_Y * BOUNCE;
                ball->bounces++;
            }

            for (size_t j = i+1; j < balls.count; ++j) {
                Ball *other_ball = &balls.items[j];
                if (other_ball->bounces > BALL_BOUNCE_LIMIT) continue;
                if (CheckCollisionCircles(ball->pos, BALL_RADIUS, other_ball->pos, BALL_RADIUS)) {
                }
            }

            DrawCircleV(ball->pos, BALL_RADIUS, color);
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}

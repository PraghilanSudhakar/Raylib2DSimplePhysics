#include <raylib-cpp.hpp>

int main(void)
{
    // Screen Dimensions
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Enable multi-sampling anti-aliasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Raylib C++ Physics");

    // Define the ball class
    class Ball
    {
    public:
        Vector2 ballPosition = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
        Vector2 ballSpeed = {0.0f, 0.0f};
        int ballRadius = 20;
        Color ballColor = MAROON;
    };

    // Create a vector of balls
    std::vector<Ball> balls(15);

    // Initialize the balls
    for (int i = 0; i < balls.size(); i++)
    {
        balls[i].ballPosition = {GetScreenWidth() / 2.0f + i * 60 - 120, GetScreenHeight() / 2.0f + i * 10};
        balls[i].ballSpeed = {0.0f + i * 5, 0.0f + i * 2};
        balls[i].ballRadius = rand() % 21 + 10;
        balls[i].ballColor = {(unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255};
    }

    // Physics parameters
    float gravity = 0.47f;
    float friction = .80f;
    float restitution = 0.87f;
    float drag = 0.005f;

    // Game state variables
    bool pause = 0;
    int framesCounter = 0;

    // So it runs at 60 fps, (1 frame error range)
    SetTargetFPS(61);

    // Game Loop
    while (!WindowShouldClose())
    {

        // Pause/Unpause
        if (IsKeyPressed(KEY_SPACE))
        {
            pause = !pause;
        }

        // Main Game Logic
        if (!pause)
        {
            for (int i = 0; i < balls.size(); i++)
            {
                Ball &ball = balls[i];

                // Apply gravity
                ball.ballSpeed.y += gravity;

                // Apply drag
                ball.ballSpeed.x *= 1 - drag;
                ball.ballSpeed.y *= 1 - drag;

                // Update position
                ball.ballPosition.x += ball.ballSpeed.x;
                ball.ballPosition.y += ball.ballSpeed.y;

                // Check for collision with ground
                if (ball.ballPosition.y >= (GetScreenHeight() - ball.ballRadius))
                {
                    ball.ballPosition.y = GetScreenHeight() - ball.ballRadius;
                    ball.ballSpeed.y *= -restitution;
                    ball.ballSpeed.y *= friction;
                }

                // Check for collision with ceiling
                else if (ball.ballPosition.y <= ball.ballRadius)
                {
                    ball.ballPosition.y = ball.ballRadius;
                    ball.ballSpeed.y *= -restitution;
                    ball.ballSpeed.y *= friction;
                }

                // Check for collision with right wall
                if (ball.ballPosition.x >= (GetScreenWidth() - ball.ballRadius))
                {
                    ball.ballPosition.x = GetScreenWidth() - ball.ballRadius;
                    ball.ballSpeed.x *= -restitution;
                    ball.ballSpeed.x *= friction;
                }

                // Check for collision with left wall
                else if (ball.ballPosition.x <= ball.ballRadius)
                {
                    ball.ballPosition.x = ball.ballRadius;
                    ball.ballSpeed.x *= -restitution;
                    ball.ballSpeed.x *= friction;
                }

                // Check for collision between balls
                for (int j = i + 1; j < balls.size(); j++)
                {
                    Ball &ball2 = balls[j];

                    if (CheckCollisionCircles(ball.ballPosition, ball.ballRadius, ball2.ballPosition, ball2.ballRadius))
                    {
                        Vector2 collisionVector = Vector2Subtract(ball2.ballPosition, ball.ballPosition);
                        float distance = Vector2Length(collisionVector);
                        float overlap = (ball.ballRadius + ball2.ballRadius) - distance;
                        Vector2 collisionNormal = Vector2Normalize(collisionVector);

                        // Add a small amount of extra separation
                        float extraSeparation = 0.01f;
                        overlap += extraSeparation;

                        // Separate the balls
                        ball.ballPosition = Vector2Subtract(ball.ballPosition, Vector2Scale(collisionNormal, overlap / 2));
                        ball2.ballPosition = Vector2Add(ball2.ballPosition, Vector2Scale(collisionNormal, overlap / 2));

                        Vector2 relativeVelocity = Vector2Subtract(ball.ballSpeed, ball2.ballSpeed);
                        float dotProduct = Vector2DotProduct(relativeVelocity, collisionNormal);

                        // If the balls are moving towards each other --> o <-- , they are colliding
                        if (dotProduct > 0)
                        {
                            // Calculate the impulse
                            float impulse = -(1 + restitution) * dotProduct / (2);
                            Vector2 impulseVec = Vector2Scale(collisionNormal, impulse);

                            // Apply the impulse
                            ball.ballSpeed = Vector2Add(ball.ballSpeed, impulseVec);
                            ball2.ballSpeed = Vector2Subtract(ball2.ballSpeed, impulseVec);
                        }
                    }
                }
            }
        }
        else
        {
            framesCounter++;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw the balls and update their positions
        for (const Ball &ball : balls)
        {
            DrawCircleV(ball.ballPosition, (float)ball.ballRadius, ball.ballColor);
        }

        //   Container For Future? = DrawCircleSectorLines({screenWidth / 2, screenHeight / 2}, 225, -90, 90, 40, BLUE);
        if (pause && ((framesCounter / 30) % 2))
            DrawText("PAUSED", 350, 200, 30, GRAY);

        // FPS Counter
        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
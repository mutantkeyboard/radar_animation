#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdlib>
#include <cstdio>

int win_width = 800, win_height = 600; // Window size
float radar_radius = 200.0f; // Radius of the radar screen
float hand_length = 200.0f; // Length of the rotating hand
float hand_angle = 0.0f; // Current angle of the hand
float dot_radius = 10.0f;
float sweep_tolerance = 5.0f; // Degrees of tolerance for hand-dot overlap

// Sweep speed control
float sweep_speed = 1.0f;    // Degrees per frame
float min_speed = 0.1f;
float max_speed = 5.0f;

// Slider geometry (in window coordinates, bottom-left area)
float slider_x = -370.0f;    // Left edge of slider track
float slider_w = 150.0f;     // Width of slider track
float slider_y = -270.0f;    // Y position of slider
float slider_h = 8.0f;       // Height of track
float knob_radius = 8.0f;    // Radius of the draggable knob
bool dragging_slider = false;

const int MAX_TARGETS = 20;
struct Target {
    float x, y;
    float brightness;
    int lifetime;  // Frames remaining before target disappears
    bool active;
};
Target targets[MAX_TARGETS];

float randFloat() { return (float) rand() / RAND_MAX; }

void spawnTarget(Target &t)
{
    float angle = randFloat() * 2.0f * M_PI;
    float r = (radar_radius - dot_radius) * std::sqrt(randFloat());
    t.x = r * std::cos(angle);
    t.y = r * std::sin(angle);
    t.brightness = 0.0f;
    t.lifetime = 300 + rand() % 600; // Lives for 15-45 seconds (at 20fps)
    t.active = true;
}

void initTargets()
{
    for (int i = 0; i < MAX_TARGETS; i++)
        targets[i].active = false;
}

void drawTargets()
{
    // Randomly spawn new targets (roughly one every few seconds)
    if (rand() % 40 == 0)
    {
        for (int i = 0; i < MAX_TARGETS; i++)
        {
            if (!targets[i].active)
            {
                spawnTarget(targets[i]);
                break;
            }
        }
    }

    glPointSize(dot_radius);

    for (int i = 0; i < MAX_TARGETS; i++)
    {
        if (!targets[i].active) continue;

        // Count down lifetime and remove expired targets
        targets[i].lifetime--;
        if (targets[i].lifetime <= 0)
        {
            targets[i].active = false;
            continue;
        }

        // Check if the hand is sweeping over this target
        float dot_angle = std::atan2(targets[i].y, targets[i].x) * 180.0f / M_PI;
        if (dot_angle < 0.0f) dot_angle += 360.0f;

        float angle_diff = hand_angle - dot_angle;
        if (angle_diff > 180.0f) angle_diff -= 360.0f;
        if (angle_diff < -180.0f) angle_diff += 360.0f;

        if (std::fabs(angle_diff) < sweep_tolerance)
            targets[i].brightness = 1.0f;

        // Draw if visible
        if (targets[i].brightness > 0.01f)
        {
            glColor3f(0.0f, targets[i].brightness, 0.0f);
            glBegin(GL_POINTS);
            glVertex2f(targets[i].x, targets[i].y);
            glEnd();

            targets[i].brightness *= 0.97f;
        }
    }
}

void drawCrosshair()
{
    glColor3f(0.2f, 0.6f, 0.2f); // Dim green
    glBegin(GL_LINES);
    // Horizontal line
    glVertex2f(-radar_radius, 0.0f);
    glVertex2f(radar_radius, 0.0f);
    // Vertical line
    glVertex2f(0.0f, -radar_radius);
    glVertex2f(0.0f, radar_radius);
    glEnd();
}



void drawText(float x, float y, const char *text)
{
    glRasterPos2f(x, y);
    while (*text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *text++);
}

void drawSlider()
{
    // Normalized position of the knob (0..1)
    float t = (sweep_speed - min_speed) / (max_speed - min_speed);
    float knob_x = slider_x + t * slider_w;

    // Track background
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(slider_x, slider_y - slider_h / 2);
    glVertex2f(slider_x + slider_w, slider_y - slider_h / 2);
    glVertex2f(slider_x + slider_w, slider_y + slider_h / 2);
    glVertex2f(slider_x, slider_y + slider_h / 2);
    glEnd();

    // Filled portion
    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(slider_x, slider_y - slider_h / 2);
    glVertex2f(knob_x, slider_y - slider_h / 2);
    glVertex2f(knob_x, slider_y + slider_h / 2);
    glVertex2f(slider_x, slider_y + slider_h / 2);
    glEnd();

    // Knob
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 20; i++)
    {
        float a = i * 2.0f * M_PI / 20.0f;
        glVertex2f(knob_x + knob_radius * std::cos(a),
                   slider_y + knob_radius * std::sin(a));
    }
    glEnd();

    // Label
    glColor3f(0.7f, 0.7f, 0.7f);
    char label[32];
    snprintf(label, sizeof(label), "Speed: %.1fx", sweep_speed);
    drawText(slider_x, slider_y + 18.0f, label);
}

void drawRadar()
{
    // Set the background color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the outer radar circle
    glColor3f(0.5f, 1.0f, 0.5f); // Light green
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++)
    {
        float angle = i * M_PI / 180.0f;
        float x = radar_radius * std::cos(angle);
        float y = radar_radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    // Draw the middle radar circle
    glColor3f(0.3f, 0.8f, 0.3f); // Darker green
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++)
    {
        float angle = i * M_PI / 180.0f;
        float x = 0.75f * radar_radius * std::cos(angle);
        float y = 0.75f * radar_radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    // Draw the inner radar circle
    glColor3f(0.1f, 0.5f, 0.1f); // Even darker green
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++)
    {
        float angle = i * M_PI / 180.0f;
        float x = 0.5f * radar_radius * std::cos(angle);
        float y = 0.5f * radar_radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

       // Draw the inner radar circle
    glColor3f(0.1f, 0.5f, 0.1f); // Even darker green
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++)
    {
        float angle = i * M_PI / 180.0f;
        float x = 0.25f * radar_radius * std::cos(angle);
        float y = 0.25f * radar_radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    // Draw the crosshair
    drawCrosshair();

    // Draw targets (before hand so hand renders on top)
    drawTargets();

    // Draw the rotating hand
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    float hand_angle_rad = hand_angle * M_PI / 180.0f;
    float hand_x = hand_length * std::cos(hand_angle_rad);
    float hand_y = hand_length * std::sin(hand_angle_rad);
    glVertex2f(hand_x, hand_y);
    glEnd();

    // Draw the speed slider
    drawSlider();

    // Swap buffers to display the screen
    glutSwapBuffers();
}

void updateHand(int value)
{
    hand_angle -= sweep_speed;

    if (hand_angle < 0.0f)
        hand_angle += 360.0f;

    glutPostRedisplay();
    glutTimerFunc(50, updateHand, 0);
}

// Convert window pixel coords to our GL coordinate system
void screenToWorld(int sx, int sy, float &wx, float &wy)
{
    wx = sx - win_width / 2.0f;
    wy = (win_height / 2.0f) - sy;
}

void updateSliderFromMouse(int sx, int sy)
{
    float wx, wy;
    screenToWorld(sx, sy, wx, wy);
    float t = (wx - slider_x) / slider_w;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    sweep_speed = min_speed + t * (max_speed - min_speed);
}

void onMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            float wx, wy;
            screenToWorld(x, y, wx, wy);
            // Check if click is near the slider
            float t = (sweep_speed - min_speed) / (max_speed - min_speed);
            float knob_x = slider_x + t * slider_w;
            float dx = wx - knob_x;
            float dy = wy - slider_y;
            if (dx * dx + dy * dy <= (knob_radius + 5) * (knob_radius + 5))
            {
                dragging_slider = true;
                updateSliderFromMouse(x, y);
            }
            // Also allow clicking anywhere on the track
            else if (wx >= slider_x && wx <= slider_x + slider_w &&
                     wy >= slider_y - 15 && wy <= slider_y + 15)
            {
                dragging_slider = true;
                updateSliderFromMouse(x, y);
            }
        }
        else
        {
            dragging_slider = false;
        }
    }
}

void onMotion(int x, int y)
{
    if (dragging_slider)
        updateSliderFromMouse(x, y);
}

void setupOpenGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-win_width/2, win_width/2, -win_height/2, win_height/2);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("Radar");

    setupOpenGL();
    initTargets();

    glutDisplayFunc(drawRadar);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutTimerFunc(10, updateHand, 0);

    glutMainLoop();

    return 0;
}

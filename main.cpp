#include <GL/glut.h>
#include <cmath>

int win_width = 800, win_height = 600; // Window size
float radar_radius = 200.0f; // Radius of the radar screen
float hand_length = 200.0f; // Length of the rotating hand
float hand_angle = 0.0f; // Current angle of the hand
int dot_timer = 30;
float dot_x, dot_y;
float dot_radius = 10.0f;


void drawDot()
{
    // Decrement the dot timer
    dot_timer--;

    // Generate a new dot and reset the timer if the timer reaches zero
    if (dot_timer <= 0)
    {
        dot_timer = 300; // Set the timer to 30 seconds (300 frames)
        dot_x = ((float) rand() / RAND_MAX) * (2 * radar_radius - 2 * dot_radius) - radar_radius + dot_radius;
        dot_y = ((float) rand() / RAND_MAX) * (2 * radar_radius - 2 * dot_radius) - radar_radius + dot_radius;
    }

    // Draw a dot at the current location
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glPointSize(dot_radius);
    glBegin(GL_POINTS);
    glVertex2f(dot_x, dot_y);
    glEnd();
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

    // Draw the rotating hand
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    float hand_angle_rad = hand_angle * M_PI / 180.0f;
    float hand_x = hand_length * std::cos(hand_angle_rad);
    float hand_y = hand_length * std::sin(hand_angle_rad);
    glVertex2f(hand_x, hand_y);
    glEnd();

    drawDot();

    // Swap buffers to display the screen
    glutSwapBuffers();
}

void updateHand(int value)
{
    // Decrement the angle of the hand
    hand_angle -= 1.0f;

    // Wrap the angle around if it goes below 0 degrees
    if (hand_angle < 0.0f)
        hand_angle += 360.0f;

    // Redraw the screen
    glutPostRedisplay();

    // Call this function again in 10 milliseconds
    glutTimerFunc(50, updateHand, 0);
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

    glutDisplayFunc(drawRadar);
    glutTimerFunc(10, updateHand, 0);

    glutMainLoop();

    return 0;
}

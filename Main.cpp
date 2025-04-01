#include <GL/glut.h>
#include <cmath>
#include <vector>

float angleX = 20.0f, angleY = -30.0f; // Camera angles
float zoom = 2.5f; // Initial zoom level (distance from the scene)
float laserHeight = 0.5f;
bool laserActive = true;

struct Particle {
    float x, y, z;
    float velocity;
};

std::vector<Particle> particles;

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);  // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending mode
}

void drawCylinder(float radius, float height, int slices) {
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluCylinder(quadric, radius, radius, height, slices, 1);
    gluDeleteQuadric(quadric);
}

void drawLaser() {
    if (laserActive) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red laser beam
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.75f, 0.0f);
        glVertex3f(0.0f, -0.3f, 0.0f);
        glEnd();
    }
}

void drawChamber() {
    glColor4f(0.5f, 0.1f, 0.1f, 0.3f); // Transparent chamber (like glass)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);  // Position the cylinder horizontally
    glRotatef(90, 0, 1, 0); // Rotate the cylinder to be horizontal (rotate along Y-axis)
    drawCylinder(2.0f, 3.0f, 30);  // Transparent cylinder
    glPopMatrix();
}

void drawSubstrate() {
    glColor3f(0.7f, 0.7f, 0.7f); // Grey substrate
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Position the substrate in the middle horizontally
    glScalef(3.0f, 0.05f, 3.0f); // Scale to make the substrate flat
    glutSolidCube(0.5); // Draw substrate as a cube
    glPopMatrix();
}

void generateParticles() {
    if (laserActive) {
        for (int i = 0; i < 5; i++) {
            particles.push_back({ (rand() % 100 - 50) / 100.0f, 1.0f, (rand() % 100 - 50) / 100.0f, 0.01f });
        }
    }
}

void updateParticles() {
    for (auto& p : particles) {
        // Move particles downwards towards the substrate
        p.y -= p.velocity*12.0f;
        if (p.y < 0.05f) p.y =0.05f; // Stops at substrate
    }
}

void drawParticles() {
    glColor3f(0.8f, 0.8f, 0.0f); // Yellow particles
    for (auto& p : particles) {
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glutSolidSphere(0.02, 10, 10); // Drawing particle as a small sphere
        glPopMatrix();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Use the zoom variable to adjust the camera's position along the z-axis
    gluLookAt(zoom, 1.0f, zoom, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); // Adjusting for zoom
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    drawChamber();
    drawSubstrate();  // Substrate is placed in the middle of the chamber
    drawLaser();
    drawParticles();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    if (key == 'l') laserActive = !laserActive; // Toggle laser on/off
    if (key == '+' && zoom > 0.5f) zoom -= 0.1f; // Zoom in
    if (key == '-' && zoom < 10.0f) zoom += 0.1f; // Zoom out
    glutPostRedisplay();
}

void mouse(int x, int y) {
    angleX = y - 250;
    angleY = x - 250;
    glutPostRedisplay();
}

void update(int value) {
    generateParticles();  // Generate particles when laser is active
    updateParticles();    // Update particles' position (falling towards the substrate)
    glutPostRedisplay();
    glutTimerFunc(50, update, 0);  // Call update function periodically
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Laser Assited Chemical Vapor Deposition Simulation");
    initLighting();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouse);
    glutTimerFunc(50, update, 0);
    glutMainLoop();
    return 0;
}

#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "LoadImageCustom.h"

/*
    Nama : Pradena Tiara Maharani
    NIM  : 672022028
*/

GLuint textures[6];
float angle = 0.0f;

struct TextureSystem {
    const char* texturePaths[6];
    const char* alternativePaths[6];
} textureSystem;

void setupTexturePaths() {
    textureSystem.texturePaths[0] = "C:\\Users\\HP5CD\\Downloads\\Tugas4_Texturing_672022028\\Tugas4_Texturing_672022028\\1.bmp";
    textureSystem.texturePaths[1] = "C:\\Users\\HP5CD\\Downloads\\Tugas4_Texturing_672022028\\Tugas4_Texturing_672022028\\2.tga";
    textureSystem.texturePaths[2] = "C:\\Users\\HP5CD\\Downloads\\Tugas4_Texturing_672022028\\Tugas4_Texturing_672022028\\3.jpg";
    textureSystem.texturePaths[3] = "C:\\Users\\HP5CD\\Downloads\\Tugas4_Texturing_672022028\\Tugas4_Texturing_672022028\\4.gif";
    textureSystem.texturePaths[4] = "C:\\Users\\HP5CD\\Downloads\\Tugas4_Texturing_672022028\\Tugas4_Texturing_672022028\\5.png";
    textureSystem.texturePaths[5] = "C:\\Users\\HP5CD\\Downloads\\Tugas4_Texturing_672022028\\Tugas4_Texturing_672022028\\6.jpeg";

    textureSystem.alternativePaths[0] = "1.bmp";
    textureSystem.alternativePaths[1] = "2.tga";
    textureSystem.alternativePaths[2] = "3.jpg";
    textureSystem.alternativePaths[3] = "4.gif";
    textureSystem.alternativePaths[4] = "5.png";
    textureSystem.alternativePaths[5] = "6.jpeg";
}

void loadAllTextures() {
    for (int i = 0; i < 6; i++) {
        const char* path = textureSystem.texturePaths[i];
        const char* fallback = textureSystem.alternativePaths[i];

        if (strstr(path, ".bmp"))
            textures[i] = loadBMP_custom(path);
        else if (strstr(path, ".tga"))
            textures[i] = loadTGA_custom(path);
        else
            textures[i] = loadImage_stb(path);

        if (textures[i] == 0) {
            printf("Gagal load %s, coba alternatif %s...\n", path, fallback);

            if (strstr(fallback, ".bmp"))
                textures[i] = loadBMP_custom(fallback);
            else if (strstr(fallback, ".tga"))
                textures[i] = loadTGA_custom(fallback);
            else
                textures[i] = loadImage_stb(fallback);
        }

        if (textures[i] == 0)
            printf("gagal total load texture ke-%d\n", i);
        else
            printf("berhasil load texture ke-%d ID=%u\n", i, textures[i]);
    }
}

void drawTexturedFace(int idx, float size) {
    glBindTexture(GL_TEXTURE_2D, textures[idx]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-size, -size,  size);
    glTexCoord2f(1, 0); glVertex3f( size, -size,  size);
    glTexCoord2f(1, 1); glVertex3f( size,  size,  size);
    glTexCoord2f(0, 1); glVertex3f(-size,  size,  size);
    glEnd();
}

void drawCube() {
    float size = 1.0f;
    glPushMatrix(); drawTexturedFace(0, size); glPopMatrix();                      // DEPAN
    glPushMatrix(); glRotatef(180, 0, 1, 0); drawTexturedFace(1, size); glPopMatrix(); // BELAKANG
    glPushMatrix(); glRotatef(90, 0, 1, 0); drawTexturedFace(2, size); glPopMatrix();  // KANAN
    glPushMatrix(); glRotatef(-90, 0, 1, 0); drawTexturedFace(3, size); glPopMatrix(); // KIRI
    glPushMatrix(); glRotatef(-90, 1, 0, 0); drawTexturedFace(4, size); glPopMatrix(); // ATAS
    glPushMatrix(); glRotatef(90, 1, 0, 0); drawTexturedFace(5, size); glPopMatrix();  // BAWAH
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(angle, 1.0f, 1.0f, 0.0f);
    drawCube();
    glutSwapBuffers();
}

void timer(int value) {
    angle += 0.5f;
    if (angle > 360.0f) angle -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setupTexturePaths();
    loadAllTextures();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tugas 4 Texturing | Pradena Tiara Maharani - 672022028");

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}

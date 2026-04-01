#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

const float PI = 3.14159265358979323846f;
int lebarWindow = 640, tinggiWindow = 480;
bool putar = false;

int indeksWarna = 0;
const int jumlahWarna = 3;

struct Objek {
    char bentuk;      // A, B, C, D
    float x, y;       // Posisi
    float rotasi;     // Sudut rotasi
    float r, g, b;    // Warna
};

vector<Objek> daftarObjek(9);

// Fungsi mendapatkan warna berdasarkan indeks
void getWarna(int indeks, float &r, float &g, float &b) {
    if (indeks == 0) {
        r = 1.0f; g = 0.8f; b = 0.2f;      // kuning
    } else if (indeks == 1) {
        r = 0.4f; g = 0.7f; b = 1.0f;      // biru
    } else if (indeks == 2) {
        r = 0.96f; g = 0.58f; b = 0.76f;   // pink
    } else {
        r = 1.0f; g = 1.0f; b = 1.0f;      // fallback putih
    }
}

// Fungsi gambar bentuk
void gambarSegitiga(float size = 100.0f) {
    glBegin(GL_TRIANGLES);
    glVertex2f(0, size / 2);
    glVertex2f(-size / 2, -size / 2);
    glVertex2f(size / 2, -size / 2);
    glEnd();
}

void gambarLingkaran(float radius = 40.0f) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; ++i) {
        float angle = 2 * PI * i / 100;
        glVertex2f(cos(angle) * radius, sin(angle) * radius);
    }
    glEnd();
}

void gambarDiamond(float width = 80.0f, float height = 100.0f) {
    glBegin(GL_POLYGON);
    glVertex2f(0, height / 2);
    glVertex2f(width / 2, 0);
    glVertex2f(0, -height / 2);
    glVertex2f(-width / 2, 0);
    glEnd();
}

void gambarSegiLima(float radius = 50.0f) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i) {
        float angle = 2 * PI * i / 5;
        glVertex2f(cos(angle) * radius, sin(angle) * radius);
    }
    glEnd();
}

// Gambar semua objek
void gambarObjek() {
    for (auto& obj : daftarObjek) {
        glPushMatrix();
        glTranslatef(obj.x, obj.y, 0);
        glRotatef(obj.rotasi, 0, 0, 1);
        glColor3f(obj.r, obj.g, obj.b);

        switch (obj.bentuk) {
            case 'A':
                gambarSegitiga();
                break;
            case 'B':
                gambarDiamond();
                break;
            case 'C':
                gambarLingkaran();
                break;
            case 'D':
                gambarSegiLima();
                break;
        }
        glPopMatrix();
    }
}

// Input bentuk dari user
void inputAwal() {
    for (int i = 0; i < 9; ++i) {
        cout << "Geometry: ";
        cin >> daftarObjek[i].bentuk;
        daftarObjek[i].bentuk = toupper(daftarObjek[i].bentuk);

        // Posisi 3x3 grid
        int col = i % 3;
        int row = i / 3;
        daftarObjek[i].x = (col - 1) * 200;
        daftarObjek[i].y = (1 - row) * 150;

        // Warna awal: kuning (index 0)
        getWarna(0, daftarObjek[i].r, daftarObjek[i].g, daftarObjek[i].b);

        daftarObjek[i].rotasi = 0.0f;
    }
}

// Display
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    gambarObjek();
    glutSwapBuffers();
}

// Reshape
void reshape(int l, int t) {
    lebarWindow = l;
    tinggiWindow = t;
    glViewport(0, 0, l, t);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-l / 2, l / 2, -t / 2, t / 2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Klik mouse: ubah semua warna berdasarkan indeks warna
void mouseku(int tombolMouse, int keadaan, int x, int y) {
    if (tombolMouse == GLUT_LEFT_BUTTON && keadaan == GLUT_DOWN) {
        cout << "mouse clicked! Change color\n";

        // Ganti ke warna berikutnya
        indeksWarna = (indeksWarna + 1) % jumlahWarna;

        float rr, gg, bb;
        getWarna(indeksWarna, rr, gg, bb);

        for (auto& obj : daftarObjek) {
            obj.r = rr;
            obj.g = gg;
            obj.b = bb;
        }

        glutPostRedisplay();
    }
}

// Keyboard handler
void keyboard(unsigned char key, int x, int y) {
    key = toupper(key);
    if (key == 'T') {
        putar = !putar;
        cout << (putar ? "t key pressed! animation start!\n" : "t key pressed! animation stop!\n");
    }
    glutPostRedisplay();
}

// Timer untuk rotasi
void updateRotasi(int value) {
    if (putar) {
        for (auto& obj : daftarObjek) {
            obj.rotasi += 2.0f;
            if (obj.rotasi >= 360.0f) obj.rotasi -= 360.0f;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(30, updateRotasi, 0);
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));
    inputAwal();
    glutInit(&argc, argv);
    glutInitWindowSize(lebarWindow, tinggiWindow);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("TTS_672022028");
    glClearColor(0, 0, 0, 0); // Background hitam

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseku);
    glutTimerFunc(30, updateRotasi, 0);

    glutMainLoop();
    return 0;
}

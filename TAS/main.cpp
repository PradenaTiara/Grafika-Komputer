#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

// Variabel rotasi model
float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

// Struktur untuk menyimpan data vertex
struct Vertex {
    float pos[3];   // Posisi (x, y, z)
    float norm[3];  // Normal (nx, ny, nz)
    float uv[2];    // Koordinat UV (u, v)
};

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;

GLuint textureID = 0;

bool loadTGA(const char* filename, GLuint& texID) {
    FILE* file = fopen(filename, "rb"); // Membuka file dalam mode binary
    if (!file) {
        std::cerr << "Tidak bisa buka file TGA: " << filename << std::endl;
        return false;
    }

    unsigned char header[18]; // Header TGA berukuran 18 byte
    fread(header, 1, 18, file); // Membaca header
    int width = header[12] + (header[13] << 8);   // Lebar gambar
    int height = header[14] + (header[15] << 8);  // Tinggi gambar
    int bpp = header[16]; // Bits per pixel

    // Validasi format (harus 24-bit atau 32-bit)
    if (bpp != 24 && bpp != 32) {
        std::cerr << "TGA harus 24/32 bit: " << filename << std::endl;
        fclose(file);
        return false;
    }

    int bytesPerPixel = bpp / 8;
    int imageSize = width * height * bytesPerPixel;
    unsigned char* data = new unsigned char[imageSize];
    fread(data, 1, imageSize, file); // Membaca data gambar
    fclose(file); // Menutup file

    for (int i = 0; i < imageSize; i += bytesPerPixel) {
        std::swap(data[i], data[i + 2]); // Menukar R dan B
    }

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, (bpp == 24 ? GL_RGB : GL_RGBA), width, height, 0,
        (bpp == 24 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, data);

    delete[] data; // Hapus data dari RAM setelah upload
    return true;
}

void loadNFG(const char* filename) {
    vertices.clear();
    indices.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "File tidak ditemukan: " << filename << std::endl;
        return;
    }

    std::string line;
    int nVertices = 0, nIndices = 0;

    while (std::getline(file, line)) {
        // Parsing jumlah vertex
        if (line.find("NrVertices:") != std::string::npos) {
            nVertices = std::stoi(line.substr(line.find(":") + 1));

            for (int i = 0; i < nVertices; ++i) {
                std::getline(file, line);
                Vertex v;
                int index;

                // Membaca posisi dan normal
                sscanf(line.c_str(), "%d. pos:[%f, %f, %f]; norm:[%f, %f, %f];",
                    &index, &v.pos[0], &v.pos[1], &v.pos[2],
                    &v.norm[0], &v.norm[1], &v.norm[2]);

                // Membaca koordinat UV (jika ada)
                size_t uvPos = line.find("uv:[");
                if (uvPos != std::string::npos) {
                    sscanf(line.c_str() + uvPos, "uv:[%f, %f];", &v.uv[0], &v.uv[1]);
                }

                vertices.push_back(v);
            }
        }
        // Parsing jumlah index
        else if (line.find("NrIndices:") != std::string::npos) {
            nIndices = std::stoi(line.substr(line.find(":") + 1));

            for (int i = 0; i < nIndices / 3; ++i) {
                std::getline(file, line);
                unsigned int a, b, c;
                sscanf(line.c_str(), "%*d. %u, %u, %u", &a, &b, &c);
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
            }
        }
    }

    file.close();
    std::cout << "Loaded " << vertices.size() << " vertices, " << indices.size() << " indices\n";
}

void drawModel() {
    glEnable(GL_TEXTURE_2D); // Aktifkan tekstur
    glBindTexture(GL_TEXTURE_2D, textureID); // Gunakan tekstur yang telah dimuat

    glBegin(GL_TRIANGLES); // Menggambar dalam bentuk segitiga
    for (size_t i = 0; i < indices.size(); ++i) {
        const Vertex& v = vertices[indices[i]];
        glNormal3fv(v.norm);      // Set normal
        glTexCoord2fv(v.uv);      // Set koordinat UV
        glVertex3fv(v.pos);       // Set posisi vertex
    }
    glEnd();

    glDisable(GL_TEXTURE_2D); // Nonaktifkan tekstur setelah menggambar
}

void initGL() {
    glEnable(GL_DEPTH_TEST);   // Aktifkan z-buffer
    glEnable(GL_LIGHTING);     // Aktifkan pencahayaan
    glEnable(GL_LIGHT0);       // Gunakan cahaya default
    glEnable(GL_NORMALIZE);    // Normalisasi normal vektor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Warna latar belakang (hitam)

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 100.0); // Sudut pandang
    glMatrixMode(GL_MODELVIEW);

    // Set properti cahaya
    GLfloat light_pos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
    GLfloat light_amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_dif[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);

    // Memuat tekstur dari file TGA
    if (!loadTGA("D:\\Grafika Komputer\\a Persiapan_TAS\\asdos\\a\\resources\\Woman1.tga", textureID)) {
        std::cerr << "Gagal load woman1.tga\n";
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Bersihkan layar dan depth buffer
    glLoadIdentity();
    gluLookAt(0, 1, 3, 0, 1, 0, 0, 1, 0); // Kamera menghadap ke tengah model

    // Rotasi objek berdasarkan input keyboard
    glRotatef(rotX, 1, 0, 0);
    glRotatef(rotY, 0, 1, 0);
    glRotatef(rotZ, 0, 0, 1);

    drawModel(); // Gambar model
    glutSwapBuffers(); // Tampilkan hasil render
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': case 'A': rotY += 5.0f; break; // Rotasi kiri
        case 'd': case 'D': rotY -= 5.0f; break; // Rotasi kanan
        case 'w': case 'W': rotX += 5.0f; break; // Rotasi atas
        case 's': case 'S': rotX -= 5.0f; break; // Rotasi bawah
        case 'q': case 'Q': rotZ += 5.0f; break; // Rotasi searah jarum jam
        case 'e': case 'E': rotZ -= 5.0f; break; // Rotasi berlawanan jarum jam
        case 27: exit(0); break; // Tombol ESC untuk keluar
    }
    glutPostRedisplay(); // Minta display untuk digambar ulang
}

void timer(int value) {
    rotY += 1.0f; // Tambah rotasi sumbu Y
    if (rotY > 360.0f) rotY -= 360.0f;
    glutPostRedisplay(); // Gambar ulang
    glutTimerFunc(1000 / 60, timer, 0); // Panggil ulang timer tiap 1/60 detik
}

// Fungsi utama (entry point)
int main(int argc, char** argv) {
    glutInit(&argc, argv); // Inisialisasi GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Gunakan buffer ganda dan depth
    glutInitWindowSize(800, 800); // Ukuran jendela
    glutCreateWindow("NFG Model Viewer"); // Judul jendela

    initGL(); // Inisialisasi OpenGL
    loadNFG("D:\\Grafika Komputer\\a Persiapan_TAS\\asdos\\a\\resources\\Woman1.nfg"); // Load file model

    // Daftarkan callback
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop(); // Masuk ke loop utama GLUT
    return 0;
}

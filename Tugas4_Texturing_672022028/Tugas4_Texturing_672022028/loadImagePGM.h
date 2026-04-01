#pragma once
#include <stdio.h>
#include <string.h>
#include <GL/freeglut.h>

GLuint loadPGM(const char* filename) {
    FILE* file;
    fopen_s(&file, filename, "rb");
    if (!file) return 0;

    char format[3];
    int width, height, maxval;
    fscanf(file, "%2s\n%d %d\n%d\n", format, &width, &height, &maxval);
    if (strcmp(format, "P5") != 0 || maxval != 255) {
        fclose(file); return 0;
    }

    int size = width * height;
    unsigned char* gray = new unsigned char[size];
    fread(gray, 1, size, file);
    fclose(file);

    // Convert to RGB
    unsigned char* rgb = new unsigned char[size * 3];
    for (int i = 0; i < size; ++i) {
        rgb[i * 3 + 0] = gray[i];
        rgb[i * 3 + 1] = gray[i];
        rgb[i * 3 + 2] = gray[i];
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    delete[] gray;
    delete[] rgb;
    return texID;
}

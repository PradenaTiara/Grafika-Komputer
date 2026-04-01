#pragma once
#include <stdio.h>
#include <string.h>
#include <GL/freeglut.h>

GLuint loadPPM(const char* filename) {
    FILE* file;
    fopen_s(&file, filename, "rb");
    if (!file) return 0;

    char format[3];
    int width, height, maxval;
    fscanf(file, "%2s\n%d %d\n%d\n", format, &width, &height, &maxval);
    if (strcmp(format, "P6") != 0 || maxval != 255) {
        fclose(file); return 0;
    }

    int size = width * height * 3;
    unsigned char* data = new unsigned char[size];
    fread(data, 1, size, file);
    fclose(file);

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    delete[] data;
    return texID;
}

#include "fileops.h"
#include <iostream>
#include <QImage>
using namespace std;



bool LoadImages(QString InputFile, QString fileIM, QString fileMask,
    unsigned char*& ID, unsigned char*& IM, unsigned char*& M,
    int& width, int& height, int& mWidth, int& mHeight, int& Mwidth, int& Mheight) {

ID = loadPixels(InputFile, width, height);
IM = loadPixels(fileIM, mWidth, mHeight);
M = loadPixels(fileMask, Mwidth, Mheight);

if (!ID || !IM || !M) {
    cout << "Error to load images." << endl;
    return false;
}
cout << "Images loaded." << endl;
return true;
}



unsigned char* loadPixels(QString input, int &width, int &height) {
    QImage imagen(input);
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen desde " << input.toStdString() << endl;
        return nullptr;
    }
    imagen = imagen.convertToFormat(QImage::Format_RGB888);
    if (imagen.format() != QImage::Format_RGB888) {
        cout << "Error: No se pudo convertir la imagen al formato RGB888." << endl;
        return nullptr;
    }
    width = imagen.width();
    height = imagen.height();
    int dataSize = width * height * 3;
    if (dataSize <= 0) { return nullptr; }
    unsigned char* pixelData = new unsigned char[dataSize]; // Sin nothrow
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);
        unsigned char* dstLine = pixelData + y * width * 3;
        // memcpy(dstLine, srcLine, width * 3); // Reemplazar si <cstring> no está permitido
        for (int x = 0; x < width * 3; ++x) { // Bucle manual si no hay memcpy
             dstLine[x] = srcLine[x];
        }
    }
    cout << "Imagen cargada: " << input.toStdString() << " (" << width << "x" << height << ")" << endl;
    return pixelData;
}

bool exportImage(unsigned char* pixelData, int width, int height, QString outputFile) {
    if (!pixelData || width <= 0 || height <= 0) return false;
    QImage outputImage(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        uchar* dstLine = outputImage.scanLine(y);
        const unsigned char* srcLine = pixelData + y * width * 3;
        // memcpy(dstLine, srcLine, width * 3); // Reemplazar si <cstring> no está permitido
        for (int x = 0; x < width * 3; ++x) { // Bucle manual si no hay memcpy
             dstLine[x] = srcLine[x];
        }
    }
    if (!outputImage.save(outputFile, "BMP")) {
        cout << "Error al guardar: " << outputFile.toStdString() << endl;
        return false;
    }
    cout << "Imagen guardada: " << outputFile.toStdString() << endl;
    return true;
}

string FileM(int indice, int totalFiles) {
    // Verificamos si el índice está dentro del rango
    if (indice >= 0 && indice < totalFiles) {
        // Creamos el nombre del File en el formato correcto
        string nombreFile = "../data/M" + to_string(indice) + ".txt";
        return nombreFile;
    } else {
        cout << "Índice fuera de rango." << endl;
        return "";  // Retorna una cadena vacía si el índice no es válido
    }
}

bool saveFinalFile(unsigned char* finalFile, int width, int height, QString outputFile) {
    if (!exportImage(finalFile, width, height, outputFile)) {
        cout << "Error to save the final image." << endl;
        return false;
    }
    cout << "Image saved: " << outputFile.toStdString() << endl;
    return true;
}



bool compareImages(unsigned char *img1, unsigned char *img2, int width, int height) {
    if (!img1 || !img2 || width <= 0 || height <= 0) return false;
    int size = width * height * 3;
    for (int i = 0; i < size; ++i) {
        if (img1[i] != img2[i]) {
            return false;
        }
    }
    return true;
}


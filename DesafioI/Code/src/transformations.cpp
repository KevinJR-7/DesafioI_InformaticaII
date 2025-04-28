#include <iostream>
#include "transformations.h"
#include "masking.h"
#include "fileops.h"
using namespace std;
unsigned char* XOR(unsigned char* img1, unsigned char* img2, int size) {
    if (!img1 || !img2 || size <= 0) return nullptr;
    unsigned char* result = new unsigned char[size]; 
    for (int i = 0; i < size; ++i) {
        result[i] = img1[i] ^ img2[i];
    }
    return result;
}

unsigned char* RotateBits(unsigned char *img, int size, int bits, bool direction) {
    if (img == nullptr || size <= 0 || bits < 0 || bits > 7) return nullptr;
    unsigned char* rotatedImage = new unsigned char[size]; 
    int shift_left = direction ? bits : (8 - bits);
    int shift_right = direction ? (8 - bits) : bits;
    if (bits == 0 || bits == 8) { 
         for(int k = 0; k < size; ++k) { rotatedImage[k] = img[k]; } 
         return rotatedImage;
    }
    for (int i = 0; i < size; ++i) {
        rotatedImage[i] = (img[i] << shift_left) | (img[i] >> shift_right);
    }
    return rotatedImage;
}

int verifyTransformation(
    unsigned char* inputImage, unsigned char* IM, unsigned char* mask,
    unsigned int* RGB, int size, int n_pixels, int seed)
{
    unsigned char* trans = nullptr; // Puntero para el resultado temporal

    // Prueba 0: XOR
    /* cout << "  Probando XOR..." << endl; */ // Debug
    trans = XOR(inputImage, IM, size);
    if (trans) {
        if (verifyMask(trans, mask, RGB, n_pixels, seed)) {
            /* cout << "  Transformacion encontrada: XOR" << endl; */ // Debug
            delete[] trans; // Liberar memoria antes de retornar
            return 0;
        }
        delete[] trans; // Liberar si la verificación falló
        trans = nullptr;
    } else { /* cout << "    Fallo al generar XOR" << endl; */ } // Debug

    // Pruebas 1-7: RotateRight
    /* cout << "  Probando RotR..." << endl; */ // Debug
    for (int i = 1; i < 8; i++) {
        // cout << "    Probando RotR " << i << "..." << endl; // Debug
        trans = RotateBits(inputImage, size, i, false); // Aplicar a inputImage original, i bits, false=Right
        if (trans) {
            if (verifyMask(trans, mask, RGB, n_pixels, seed)) {
                /* cout << "  Transformacion encontrada: RotR " << i << endl; */ // Debug
                delete[] trans; // Free memory before returning
                return i;
            }
            delete[] trans; // Free if verification failed
            trans = nullptr;
        } else { /* cout << "    Fallo al generar RotR " << i << endl; */ } // Debug
    }

    cout << " Not found any transformation." << endl; // Debug
    return -1; // No se encontró ninguna transformación
}

unsigned char* processStage(unsigned char* currentImage, unsigned char* IM, unsigned char* M, 
    int stage, int Mtxt, int size) {
    // Generar nombre del archivo de máscara
    string Filename = FileM(stage, Mtxt);

    // Cargar datos de enmascaramiento
    int seedi = 0, n_pixelsi = 0;
    unsigned int* data_i = loadSeedMasking(Filename.c_str(), seedi, n_pixelsi);

    if (!data_i) {
        cout << "Error to load the mask file." << endl;
        return nullptr;
    }

    // Verificar transformación
    int transformacion = verifyTransformation(currentImage, IM, M, data_i, size, n_pixelsi, seedi);
    delete[] data_i;
    data_i = nullptr;

    cout << "-------------------------" << endl;
    cout << "Stage " << (Mtxt - stage) << " of " << Mtxt << endl;

    // Aplicar transformación inversa
    if (transformacion == 0) {
        cout << "Detected transformation: XOR." << endl;
        return XOR(currentImage, IM, size);
    }
    
    if (transformacion >= 1 && transformacion <= 7) {
        cout << "Detected transformation: RotateRight " << transformacion << "." << endl;
        return RotateBits(currentImage, size, 8 - transformacion, true);  // Rotate bits to the left (inverse)
    }
    
    cout << "Error: Transformación no reconocida." << endl;
    return nullptr;
}


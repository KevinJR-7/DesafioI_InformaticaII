#include "masking.h"
#include <fstream>
#include <iostream>

using namespace std;

unsigned int* loadSeedMasking(const char* Filename, int &seed, int &n_pixels) {
    ifstream File(Filename);
    if (!File.is_open()) {
        cout << "Error: Could not open the file " << endl;
        return nullptr;
    }

    File >> seed;
    int r, g, b;

    while (File >> r >> g >> b) {
        n_pixels++;
    }

    File.close();
    File.open(Filename);

    if (!File.is_open()) {
        cout << "Error al reabrir el File." << endl;
        return nullptr;
    }

    unsigned int* RGB = new unsigned int[n_pixels * 3];
    File >> seed;

    for (int i = 0; i < n_pixels * 3; i += 3) {
        File >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }

    File.close();

    return RGB;
}

bool verifyMask(unsigned char* transformedImage, unsigned char* mask, unsigned int* RGB, int &n_pixels, int &seed) {
    for (int k = 0; k < n_pixels * 3; k++) {
        int result = RGB[k] - mask[k];
        if (result != transformedImage[seed + k]) {
            return false;
        }
    }
    /* cout << "Verificacion de la mascara exitosa." << endl; */ // Debug
    return true;
}

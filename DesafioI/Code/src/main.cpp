
#include <iostream>
#include "fileops.h"
#include "transformations.h"


using namespace std;



int main() {
    QString FileEntrada = "../data/I_D.bmp";
    QString outputFile = "../data/I_Ofin.bmp";
    QString FileIM = "../data/I_M.bmp";
    QString FileMask = "../data/M.bmp";
    int Mtxt = 7;

    unsigned char *ID = nullptr, *IM = nullptr, *M = nullptr;
    unsigned char *currentImage = nullptr;
    unsigned char *previousImage = nullptr;
    unsigned char* IO_final = nullptr;
    int width = 0, height = 0, size = 0;
    int mWidth = 0, mHeight = 0, Mwidth = 0, Mheight = 0;

    if (!LoadImages(FileEntrada, FileIM, FileMask, ID, IM, M, width, height, mWidth, mHeight, Mwidth, Mheight)) {
        return -1;
    }

    size = width * height * 3;
    currentImage = ID;

    for (int i = Mtxt - 1; i >= 0; --i) {
        previousImage = processStage(currentImage, IM, M, i, Mtxt, size);

        if (!previousImage) {
            cout << "Error in processing stage " << i << endl;
            return -1;
        }

        if (currentImage != ID) {
            delete[] currentImage;
        }
        currentImage = previousImage;
    }

    IO_final = currentImage;

    if (!saveFinalFile(IO_final, width, height, outputFile)) {
        return -1;
    }

    // Liberar memoria
    delete[] ID;
    delete[] IM;
    delete[] M;
    delete[] currentImage;

    return 0;
}


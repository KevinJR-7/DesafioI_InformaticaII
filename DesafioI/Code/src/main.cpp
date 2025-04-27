#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QFile>

#include "fileops.h"
#include "masking.h"
#include "transformations.h"


using namespace std;

int main() {
    
    QString FileEntrada = "../data/I_D.bmp";
    QString outputFile = "../data/I_Ofin.bmp";
    QString FileIM = "../data/I_M.bmp";
    QString FileMask = "../data/M.bmp"; // File de máscara inicial
    
    unsigned int Mtxt = 7;  // Número de Files de máscara a procesar

    // --- Variables ---
    unsigned char *ID = nullptr, *IM = nullptr, *M = nullptr;
    unsigned char *currentImage = nullptr; // Imagen en la etapa actual del descifrado
    unsigned char *previousImage = nullptr; // Resultado de aplicar la inversa
    unsigned int *data_i = nullptr; // Datos del File Mi.txt
    int width = 0, height = 0, size = 0; // Tamaño ID
    int mWidth = 0, mHeight = 0; // Tamaño IM
    int Mwidth = 0, Mheight = 0; // Tamaño Máscara
    bool decryption_successful = false;

    
    // --- cargar imagenes base ---
    ID = loadPixels(FileEntrada, width, height);
    IM = loadPixels(FileIM, mWidth, mHeight);
    M = loadPixels(FileMask, Mwidth, Mheight); // Cargar la imagen de máscara inicial
    if (!ID || !IM || !M) {
        cout << "Error al cargar las imagenes." << endl;
        return -1;
    }
    size = width * height * 3; // Tamaño de la imagen en bytes
    cout << "imagenes cargadas" << endl;
    currentImage = ID; // Inicializar currentImage con ID
    for (int i = Mtxt - 1; i >= 0; --i) {
        // Generar el nombre del File de máscara correspondiente
        string nombreFile = FileM(i, Mtxt);
        int seedi = 0;
        int n_pixelsi = 0;
        data_i = loadSeedMasking(nombreFile.c_str(), seedi, n_pixelsi); // Cargar la semilla y los datos de enmascaramiento
        int transformacion = verifyTransformation(currentImage, IM, M, data_i, size, n_pixelsi, seedi); // Verificar la transformación
        delete [] data_i; // Liberar memoria de los datos de enmascaramiento
        data_i = nullptr;
        cout << seedi << " SEMilllllaaaaaaaaa" << endl;
        cout << n_pixelsi <<  "N pixelssssssssss 0" << endl;
        previousImage = nullptr;

        switch (transformacion) {
            case 0:
                previousImage = XOR(currentImage, IM, size); // Aplicar XOR
                cout << "Transformacion 0: XOR aplicado." << endl;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                previousImage = RotateBits(currentImage, size, 8 - transformacion, true); // Rotar bits a la izquierda
                cout << "Transformacion rotacion de" << transformacion << " bits a la derecha." << endl;
                break;
            default:
                cout << "Error: Transformacion no reconocida." << endl;
                return -1; // Salir si la transformación no es válida   
        }
        if (!previousImage) {
            cout << "Error: previousImage no está inicializado." << endl;
            return -1;
        }
        if (currentImage != ID) {
            delete[] currentImage;
        }
        currentImage = previousImage; // Actualizar currentImage con el resultado de la transformación
        cout << "Etapa " << i << " completada." << endl;
    }

    decryption_successful = true; // Marcar como exitoso
    cout << "Desencriptacion exitosa." << endl;
    cout << "Guardando imagen desencriptada..." << endl;
    if (!exportImage(currentImage, width, height, outputFile)) {
        cout << "Error al guardar la imagen desencriptada." << endl;
        decryption_successful = false; // Marcar como fallido
    } else {
        cout << "Imagen desencriptada guardada exitosamente." << endl;
    }
    return decryption_successful ? 0 : -1; // Retornar el resultado de la desencriptación

    //limpiar
    delete[] ID; // Liberar memoria de la imagen original
    delete[] IM; // Liberar memoria de la imagen de enmascaramiento
    delete[] M; // Liberar memoria de la imagen de máscara inicial
    delete[] currentImage; // Liberar memoria de la imagen actual
    delete[] previousImage; // Liberar memoria de la imagen anterior
}


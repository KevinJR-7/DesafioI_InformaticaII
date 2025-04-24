#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QFile>
// NO incluir <new> ni <cstring> si no están permitidos explícitamente más allá de Qt/iostream/fstream

using namespace std;

// --- Declaraciones de Funciones ---
unsigned char* loadPixels(QString input, int &width, int &height);
bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida);
unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels);
bool compareImages(unsigned char* img1, unsigned char* img2, int width, int height);
unsigned char* ShiftBits(unsigned char *img, int size, int bits, bool direction); // Desplazamiento de bits
unsigned char* RotateBits(unsigned char *img, int size, int bits, bool direction); // Rotación de bits
unsigned char* XOR(unsigned char* img1, unsigned char* img2, int size);
unsigned char* ShiftImagePixels(const unsigned char* img, int size, int seed); // Desplazamiento cíclico de píxeles

bool verifyMask (unsigned char* transformedImage, unsigned char* mask, unsigned int* RGB, int &n_pixels, int &seed);



int main() {
    // --- Rutas (Asegúrate que sean correctas para tu sistema) ---
    const char* basePath = "C:/Users/goken/OneDrive/Escritorio/2025-1/Informatica_II/DesafioI_InformaticaII/DesafioI/Caso_1/";
    char pathBuffer[512]; // Buffer para construir rutas

    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", basePath, "I_O.bmp");
    QString archivoIO_q = pathBuffer;
    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", basePath, "I_M.bmp");
    QString archivoIM_q = pathBuffer;
    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", basePath, "M.bmp");
    QString archivoM_q = pathBuffer;

    // Rutas archivos de texto
    const char* archivoM1_path = "C:/Users/goken/OneDrive/Escritorio/2025-1/Informatica_II/DesafioI_InformaticaII/DesafioI/Caso_1/M1.txt";
    const char* archivoM2_path = "C:/Users/goken/OneDrive/Escritorio/2025-1/Informatica_II/DesafioI_InformaticaII/DesafioI/Caso_1/M2.txt";

    // (Opcional) Rutas imágenes de referencia para comparar
    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", basePath, "P1.bmp");
    QString archivoP1Ref_q = pathBuffer;
    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", basePath, "P2.bmp");
    QString archivoP2Ref_q = pathBuffer;
    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", basePath, "I_D.bmp"); // P3 = ID
    QString archivoP3Ref_q = pathBuffer;


    // --- Variables para dimensiones ---
    int widthIO = 0, heightIO = 0;
    int widthIM = 0, heightIM = 0;
    int widthM = 0, heightM = 0;
    int sizeIO = 0; // Tamaño en bytes

    // --- Cargar Imágenes Iniciales ---
    unsigned char* IO = loadPixels(archivoIO_q, widthIO, heightIO);
    unsigned char* IM = loadPixels(archivoIM_q, widthIM, heightIM);
    unsigned char* M = loadPixels(archivoM_q, widthM, heightM);

    // Validar carga y dimensiones
    if (!IO || !IM || !M) {
        cout << "Error al cargar las imágenes iniciales (IO, IM o M)." << endl;
        // Liberar lo que se haya podido cargar
        delete[] IO; delete[] IM; delete[] M;
        return -1;
    }
    if (widthIO != widthIM || heightIO != heightIM) {
        cout << "Error: Las dimensiones de IO e IM no coinciden." << endl;
        delete[] IO; delete[] IM; delete[] M;
        return -1;
    }
    sizeIO = widthIO * heightIO * 3; // Calcular tamaño una vez

    cout << "--- Iniciando Proceso (Caso 1) ---" << endl;

    // --- Paso 1 (Adelante): P1 = IO XOR IM ---
    cout << "Calculando P1 = IO XOR IM..." << endl;
    unsigned char* P1 = XOR(IO, IM, sizeIO);
    if (!P1) { cout << "Error calculando P1." << endl; /* Limpiar y salir */ delete[] IO; delete[] IM; delete[] M; return -1; }
    cout << "P1 calculado." << endl;

    // --- Verificación de P1 usando M1.txt ---
    cout << "Verificando P1 con M1.txt..." << endl;
    int seed1 = 0, n_pixels1 = 0;
    unsigned int* data1 = loadSeedMasking(archivoM1_path, seed1, n_pixels1);
    bool paso1_verificado = false;
    if (data1) {
        // Llamar a la función de verificación
        paso1_verificado = verifyMask(P1, M, data1, n_pixels1, seed1);
        cout << "Resultado de la verificacion de P1 con M1.txt: " << (paso1_verificado ? "EXITOSA" : "FALLIDA") << endl;
        delete[] data1; // Liberar memoria de data1
    } else {
        cout << "Error: No se pudieron cargar los datos de M1.txt para la verificacion." << endl;
        // Considerar si continuar o no si la verificación es crucial
    }

    // (Opcional) Comparar P1 con imagen de referencia P1.bmp
    // unsigned char* P1Ref = loadPixels(archivoP1Ref_q, widthIO, heightIO);
    // if (P1Ref) {
    //     cout << "Comparacion de P1 calculado vs P1.bmp: " << (compareImages(P1, P1Ref, widthIO, heightIO) ? "IGUALES" : "DIFERENTES") << endl;
    //     delete[] P1Ref;
    // }

    // --- Paso 2 (Adelante): P2 = RotateRight(P1, 3) ---
    cout << "Calculando P2 = RotateRight(P1, 3)..." << endl;
    unsigned char* P2 = RotateBits(P1, sizeIO, 3, false); // false = derecha
    if (!P2) { cout << "Error calculando P2." << endl; /* Limpiar y salir */ delete[] IO; delete[] IM; delete[] M; delete[] P1; return -1; }
    cout << "P2 calculado." << endl;

    // --- Verificación de P2 usando M2.txt ---
    cout << "Verificando P2 con M2.txt..." << endl;
    int seed2 = 0, n_pixels2 = 0;
    unsigned int* data2 = loadSeedMasking(archivoM2_path, seed2, n_pixels2);
    bool paso2_verificado = false;
    if (data2) {
        paso2_verificado = verifyMask(P2, M, data2, n_pixels2, seed2);
        cout << "Resultado de la verificacion de P2 con M2.txt: " << (paso2_verificado ? "EXITOSA" : "FALLIDA") << endl;
        delete[] data2;
    } else {
        cout << "Error: No se pudieron cargar los datos de M2.txt para la verificacion." << endl;
    }

    // (Opcional) Comparar P2 con imagen de referencia P2.bmp
    // unsigned char* P2Ref = loadPixels(archivoP2Ref_q, widthIO, heightIO);
    // if (P2Ref) {
    //     cout << "Comparacion de P2 calculado vs P2.bmp: " << (compareImages(P2, P2Ref, widthIO, heightIO) ? "IGUALES" : "DIFERENTES") << endl;
    //     delete[] P2Ref;
    // }

    // --- Paso 3 (Adelante): P3 = P2 XOR IM ---
    cout << "Calculando P3 = P2 XOR IM..." << endl;
    unsigned char* P3 = XOR(P2, IM, sizeIO);
    if (!P3) { cout << "Error calculando P3." << endl; /* Limpiar y salir */ delete[] IO; delete[] IM; delete[] M; delete[] P1; delete[] P2; return -1; }
    cout << "P3 calculado (equivalente a I_D)." << endl;

    // (Opcional) Comparar P3 con imagen de referencia I_D.bmp
    unsigned char* P3Ref = loadPixels(archivoP3Ref_q, widthIO, heightIO);
     if (P3Ref) {
         cout << "Comparacion de P3 calculado vs I_D.bmp: " << (compareImages(P3, P3Ref, widthIO, heightIO) ? "IGUALES" : "DIFERENTES") << endl;
         delete[] P3Ref;
     }

    // --- Limpieza Final ---
    cout << "Limpiando memoria..." << endl;
    delete[] IO;
    delete[] IM;
    delete[] M;
    delete[] P1;
    delete[] P2;
    delete[] P3;
    // Asegurarse que P1Ref, P2Ref, P3Ref se borren si se cargaron

    cout << "Programa terminado." << endl;
    return 0;
}

// --- Implementaciones de Funciones ---



// ... (Implementaciones de loadPixels, exportImage, loadSeedMasking, XOR, RotateBits, compareImages, ShiftBits, ShiftImagePixels) ...
// Asegúrate de que todas las implementaciones estén aquí o en archivos .cpp separados si usas .h
// Copia aquí las implementaciones que ya tienes de esas funciones.

// Ejemplo de implementación de loadPixels (asegúrate que usa memcpy si <cstring> no está permitido)
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

// Ejemplo de implementación de exportImage (asegúrate que usa bucle si memcpy no está permitido)
bool exportImage(unsigned char* pixelData, int width, int height, QString archivoSalida) {
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
    if (!outputImage.save(archivoSalida, "BMP")) {
        cout << "Error al guardar: " << archivoSalida.toStdString() << endl;
        return false;
    }
    cout << "Imagen guardada: " << archivoSalida.toStdString() << endl;
    return true;
}

// Implementación de loadSeedMasking (ya la tenías)
unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels){
    /*
     * @brief Carga la semilla y los resultados del enmascaramiento desde un archivo de texto.
     *
     * Esta función abre un archivo de texto que contiene una semilla en la primera línea y,
     * a continuación, una lista de valores RGB resultantes del proceso de enmascaramiento.
     * Primero cuenta cuántos tripletes de píxeles hay, luego reserva memoria dinámica
     * y finalmente carga los valores en un arreglo de enteros.
     *
     * @param nombreArchivo Ruta del archivo de texto que contiene la semilla y los valores RGB.
     * @param seed Variable de referencia donde se almacenará el valor entero de la semilla.
     * @param n_pixels Variable de referencia donde se almacenará la cantidad de píxeles leídos
     *                 (equivalente al número de líneas después de la semilla).
     *
     * @return Puntero a un arreglo dinámico de enteros que contiene los valores RGB
     *         en orden secuencial (R, G, B, R, G, B, ...). Devuelve nullptr si ocurre un error al abrir el archivo.
     *
     * @note Es responsabilidad del usuario liberar la memoria reservada con delete[].
     */
    
        // Abrir el archivo que contiene la semilla y los valores RGB
        ifstream archivo(nombreArchivo);
        if (!archivo.is_open()) {
            // Verificar si el archivo pudo abrirse correctamente
            cout << "No se pudo abrir el archivo." << endl;
            return nullptr;
        }
    
        // Leer la semilla desde la primera línea del archivo
        archivo >> seed;
    
        int r, g, b;
    
        // Contar cuántos grupos de valores RGB hay en el archivo
        // Se asume que cada línea después de la semilla tiene tres valores (r, g, b)
        while (archivo >> r >> g >> b) {
            n_pixels++;  // Contamos la cantidad de píxeles
        }
    
        // Cerrar el archivo para volver a abrirlo desde el inicio
        archivo.close();
        archivo.open(nombreArchivo);
    
        // Verificar que se pudo reabrir el archivo correctamente
        if (!archivo.is_open()) {
            cout << "Error al reabrir el archivo." << endl;
            return nullptr;
        }
    
        // Reservar memoria dinámica para guardar todos los valores RGB
        // Cada píxel tiene 3 componentes: R, G y B
        unsigned int* RGB = new unsigned int[n_pixels * 3];
    
        // Leer nuevamente la semilla desde el archivo (se descarta su valor porque ya se cargó antes)
        archivo >> seed;
    
        // Leer y almacenar los valores RGB uno por uno en el arreglo dinámico
        for (int i = 0; i < n_pixels * 3; i += 3) {
            archivo >> r >> g >> b;
            RGB[i] = r;
            RGB[i + 1] = g;
            RGB[i + 2] = b;
        }
    
        // Cerrar el archivo después de terminar la lectura
        archivo.close();
    
        // Mostrar información de control en consola
        cout << "Semilla: " << seed << endl;
        cout << "Cantidad de píxeles leídos: " << n_pixels << endl;
    
        // Retornar el puntero al arreglo con los datos RGB
        return RGB;
    }
// Implementación de XOR (ya la tenías)
unsigned char* XOR(unsigned char* img1, unsigned char* img2, int size) {
    if (!img1 || !img2 || size <= 0) return nullptr;
    unsigned char* result = new unsigned char[size]; // Sin nothrow
    for (int i = 0; i < size; ++i) {
        result[i] = img1[i] ^ img2[i];
    }
    return result;
}

// Implementación de RotateBits (ya la tenías)
unsigned char* RotateBits(unsigned char *img, int size, int bits, bool direction) {
    if (img == nullptr || size <= 0 || bits < 0 || bits > 7) return nullptr;
    unsigned char* resultado = new unsigned char[size]; // Sin nothrow
    int shift_left = direction ? bits : (8 - bits);
    int shift_right = direction ? (8 - bits) : bits;
    if (bits == 0 || bits == 8) { // Optimización para no rotación
         for(int k = 0; k < size; ++k) { resultado[k] = img[k]; } // Copia manual
         return resultado;
    }
    for (int i = 0; i < size; ++i) {
        resultado[i] = (img[i] << shift_left) | (img[i] >> shift_right);
    }
    return resultado;
}

// Implementación de compareImages (ya la tenías)
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

// Implementación de ShiftBits (desplazamiento de bits, ya la tenías)
unsigned char* ShiftBits(unsigned char *img, int size, int bits, bool direction) {
    if (img == nullptr || size <= 0 || bits < 0 || bits > 7) return nullptr;
    unsigned char* resultado = new unsigned char[size]; // Sin nothrow
    if (bits == 0) { // Optimización para no desplazamiento
        for(int k = 0; k < size; ++k) { resultado[k] = img[k]; } // Copia manual
        return resultado;
    }
    for (int i = 0; i < size; ++i) {
        if (direction) { // Izquierda
            resultado[i] = img[i] << bits;
        } else { // Derecha
            resultado[i] = img[i] >> bits;
        }
    }
    return resultado;
}
bool verifyMask (unsigned char* transformedImage, unsigned char* mask, unsigned int* RGB, int &n_pixels, int &seed){

    for (int k = 0; k < n_pixels * 3; k++) {
    int result = 0;
        result = RGB[k] - mask[k]; //
        if (result != transformedImage[seed + k]) {
            cout << "Error en la verificación de la máscara en el píxel " << k << endl;
            return false; // La verificación falló	
        }

    }
    cout << "Verificación de la máscara exitosa." << endl;
    return true; // La verificación fue exitosa
}

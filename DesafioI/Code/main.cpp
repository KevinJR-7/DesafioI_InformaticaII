#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QFile>



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
int verifyTransformation(
    unsigned char* inputImage, unsigned char* IM, unsigned char* mask,
    unsigned int* RGB, int size, int n_pixels, int seed);
string archivoM(int indice, int totalArchivos); // Genera el nombre del archivo de máscara


int main() {
    
    QString archivoEntrada = "../data/I_D.bmp";
    QString archivoSalida = "../data/I_Ofin.bmp";
    QString archivoIM = "../data/I_M.bmp";
    QString archivoMask = "../data/M.bmp"; // Archivo de máscara inicial
    
    int Mtxt = 7;  // Número de archivos de máscara a procesar

    // --- Variables ---
    unsigned char *ID = nullptr, *IM = nullptr, *M = nullptr;
    unsigned char *currentImage = nullptr; // Imagen en la etapa actual del descifrado
    unsigned char *previousImage = nullptr; // Resultado de aplicar la inversa
    unsigned int *data_i = nullptr; // Datos del archivo Mi.txt
    int width = 0, height = 0, size = 0;
    int mWidth = 0, mHeight = 0;
    int Mwidth = 0, Mheight = 0;
    bool decryption_successful = false;
    unsigned char* IO_final = nullptr; // Puntero al resultado final

    
    // --- cargar imagenes base ---
    ID = loadPixels(archivoEntrada, width, height);
    IM = loadPixels(archivoIM, mWidth, mHeight);
    M = loadPixels(archivoMask, Mwidth, Mheight); // Cargar la imagen de máscara inicial
    if (!ID || !IM || !M) {
        cout << "Error al cargar las imagenes." << endl;
        return -1;
    }
    size = width * height * 3; // Tamaño de la imagen en bytes
    cout << "imagenes cargadas" << endl;
    currentImage = ID; // Inicializar currentImage con ID
    for (int i = Mtxt - 1; i >= 0; --i) {
        // Generar el nombre del archivo de máscara correspondiente
        string nombreArchivo = archivoM(i, Mtxt);
        int seedi = 0;
        int n_pixelsi = 0;
        data_i = loadSeedMasking(nombreArchivo.c_str(), seedi, n_pixelsi); // Cargar la semilla y los datos de enmascaramiento
        int transformacion = verifyTransformation(currentImage, IM, M, data_i, size, n_pixelsi, seedi); // Verificar la transformación
        delete [] data_i; // Liberar memoria de los datos de enmascaramiento
        cout << seedi << " SEMilllllaaaaaaaaa" << endl;
        cout << n_pixelsi <<  "N pixelssssssssss 0" << endl;
        data_i = nullptr; // Reiniciar puntero para evitar uso posterior
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
                previousImage = RotateBits(currentImage, size, 8 - transformacion, true); // Rotar bits a la derecha
                cout << "Transformacion rotacion de" << transformacion << " bits a la izquierda." << endl;
                break;
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
                previousImage = RotateBits(currentImage, size, transformacion - 10, false); // Rotar bits a la derecha
                cout << "Transformacion rotacion de" << transformacion - 10 << " bits a la derecha." << endl;
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
            delete[] currentImage; // Liberar memoria de la imagen anterior
        }
        currentImage = previousImage; // Actualizar currentImage con el resultado de XOR
        cout << "Etapa" << i << "completada." << endl;
        






    }
    IO_final = currentImage; // Guardar el resultado final
    decryption_successful = true; // Marcar como exitoso
    cout << "Desencriptacion exitosa." << endl;
    cout << "Guardando imagen desencriptada..." << endl;
    if (!exportImage(IO_final, width, height, archivoSalida)) {
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



int verifyTransformation(
    unsigned char* inputImage, unsigned char* IM, unsigned char* mask,
    unsigned int* RGB, int size, int n_pixels, int seed)
{
    cout << "Verificando transformacion para seed=" << seed << ", n_pixels=" << n_pixels << "..." << endl;
    unsigned char* trans = nullptr; // Puntero para el resultado temporal

    // Prueba 0: XOR
    cout << "  Probando XOR..." << endl;
    trans = XOR(inputImage, IM, size);
    if (trans) {
        if (verifyMask(trans, mask, RGB, n_pixels, seed)) {
            cout << "  Transformacion encontrada: XOR (Code 0)" << endl;
            delete[] trans; // Liberar memoria antes de retornar
            return 0;
        }
        delete[] trans; // Liberar si la verificación falló
        trans = nullptr;
    } else { cout << "    Fallo al generar XOR" << endl; }

    // Pruebas 1-7: RotateRight
    cout << "  Probando RotateRight..." << endl;
    for (int i = 1; i < 8; i++) {
        // cout << "    Probando RotR " << i << "..." << endl; // Debug
        trans = RotateBits(inputImage, size, i, false); // Aplicar a inputImage original, i bits, false=Right
        if (trans) {
            if (verifyMask(trans, mask, RGB, n_pixels, seed)) {
                cout << "  Transformacion encontrada: RotateRight " << i << " (Code " << i << ")" << endl;
                delete[] trans; // Liberar memoria antes de retornar
                return i;
            }
            delete[] trans; // Liberar si falló
            trans = nullptr;
        } else { /* cout << "    Fallo al generar RotR " << i << endl; */ } // Debug
    }

    // Pruebas 11-17: RotateLeft
    cout << "  Probando RotateLeft..." << endl;
    for (int i = 1; i < 8; i++) {
        // cout << "    Probando RotL " << i << "..." << endl; // Debug
        trans = RotateBits(inputImage, size, i, true); // Aplicar a inputImage original, i bits, true=Left
        if (trans) {
            if (verifyMask(trans, mask, RGB, n_pixels, seed)) {
                cout << "  Transformacion encontrada: RotateLeft " << i << " (Code " << 10 + i << ")" << endl;
                delete[] trans; // Liberar memoria antes de retornar
                return 10 + i;
            }
            delete[] trans; // Liberar si falló
            trans = nullptr;
        } else { /* cout << "    Fallo al generar RotL " << i << endl; */ } // Debug
    }

    // Pruebas 21-27: ShiftLeft
    cout << "  Probando ShiftLeft..." << endl;
    for (int i = 1; i < 8; i++) {
        // cout << "    Probando ShiftL " << i << "..." << endl; // Debug
        trans = ShiftBits(inputImage, size, i, true); // Aplicar a inputImage original, i bits, true=Left
        if (trans) {
            if (verifyMask(trans, mask, RGB, n_pixels, seed)) {
                cout << "  Transformacion encontrada: ShiftLeft " << i << " (Code " << 20 + i << ")" << endl;
                delete[] trans; // Liberar memoria antes de retornar
                return 20 + i;
            }
            delete[] trans; // Liberar si falló
            trans = nullptr;
        } else { /* cout << "    Fallo al generar ShiftL " << i << endl; */ } // Debug
    }

    // Pruebas 31-37: ShiftRight
    cout << "  Probando ShiftRight..." << endl;
    for (int i = 1; i < 8; i++) {
        // cout << "    Probando ShiftR " << i << "..." << endl; // Debug
        trans = ShiftBits(inputImage, size, i, false); // Aplicar a inputImage original, i bits, false=Right
        if (trans) {
            if (verifyMask(trans, mask, RGB, n_pixels, seed)) {
                cout << "  Transformacion encontrada: ShiftRight " << i << " (Code " << 30 + i << ")" << endl;
                delete[] trans; // Liberar memoria antes de retornar
                return 30 + i;
            }
            delete[] trans; // Liberar si falló
            trans = nullptr;
        } else { /* cout << "    Fallo al generar ShiftR " << i << endl; */ } // Debug
    }

    cout << "  No se encontro ninguna transformacion compatible." << endl;
    return -1; // No se encontró ninguna transformación
}


string archivoM(int indice, int totalArchivos) {
    // Verificamos si el índice está dentro del rango
    if (indice >= 0 && indice < totalArchivos) {
        // Creamos el nombre del archivo en el formato correcto
        string nombreArchivo = "../data/M" + to_string(indice) + ".txt";
        return nombreArchivo;
    } else {
        cout << "Índice fuera de rango." << endl;
        return "";  // Retorna una cadena vacía si el índice no es válido
    }
}

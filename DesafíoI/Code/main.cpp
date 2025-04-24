/*
 * Programa demostrativo de manipulaciónprocesamiento de imágenes BMP en C++ usando Qt.
 *
 * Descripción:
 * Este programa realiza las siguientes tareas:
 * 1. Carga una imagen BMP desde un archivo (formato RGB sin usar estructuras ni STL).
 * 2. Modifica los valores RGB de los píxeles asignando un degradado artificial basado en su posición.
 * 3. Exporta la imagen modificada a un nuevo archivo BMP.
 * 4. Carga un archivo de texto que contiene una semilla (offset) y los resultados del enmascaramiento
 *    aplicados a una versión transformada de la imagen, en forma de tripletas RGB.
 * 5. Muestra en consola los valores cargados desde el archivo de enmascaramiento.
 * 6. Gestiona la memoria dinámicamente, liberando los recursos utilizados.
 *
 * Entradas:
 * - Archivo de imagen BMP de entrada ("I_O.bmp").
 * - Archivo de salida para guardar la imagen modificada ("I_D.bmp").
 * - Archivo de texto ("M1.txt") que contiene:
 *     • Una línea con la semilla inicial (offset).
 *     • Varias líneas con tripletas RGB resultantes del enmascaramiento.
 *
 * Salidas:
 * - Imagen BMP modificada ("I_D.bmp").
 * - Datos RGB leídos desde el archivo de enmascaramiento impresos por consola.
 *
 * Requiere:
 * - Librerías Qt para manejo de imágenes (QImage, QString).
 * - No utiliza estructuras ni STL. Solo arreglos dinámicos y memoria básica de C++.
 *
 * Autores: Augusto Salazar Y Aníbal Guerra
 * Fecha: 06/04/2025
 * Asistencia de ChatGPT para mejorar la forma y presentación del código fuente
 */

#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QFile>

using namespace std;
unsigned char* loadPixels(QString input, int &width, int &height);
bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida);
unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels);
bool compareImages(unsigned char* img1, unsigned char* img2, int width, int height);
unsigned char* ShiftBits(unsigned char *img, int size, int bits, bool direction);
unsigned char* RotateBits(unsigned char *img, int size, int bits, bool direction);
unsigned char* XOR(unsigned char* img1, unsigned char* img2, int size);
bool verifyMasking(unsigned char* transformedImage, unsigned char* mask, int maskWidth, int maskHeight, int seed, const unsigned int* maskingData, int n_pixels);


int main() {
    // Rutas de las imágenes y archivos
    QString archivoIO = ":/imagenes/Imagenes/I_O.bmp";
    QString archivoIM = ":/imagenes/Imagenes/I_M.bmp";
    QString archivoM = ":/imagenes/Imagenes/M.bmp";
    QString archivoP1Ref = ":/imagenes/Imagenes/P1.bmp";
    QString archivoP2Ref = ":/imagenes/Imagenes/P2.bmp";
    QString archivoP3Ref = ":/imagenes/Imagenes/I_D.bmp";

    // Variables para dimensiones
    int widthIO = 0, heightIO = 0;
    int widthIM = 0, heightIM = 0;
    int widthM = 0, heightM = 0;

    // Cargar las imágenes IO, IM y la máscara M
    unsigned char* IO = loadPixels(archivoIO, widthIO, heightIO);
    unsigned char* IM = loadPixels(archivoIM, widthIM, heightIM);
    unsigned char* M = loadPixels(archivoM, widthM, heightM);

    if (!IO || !IM || !M) {
        cout << "Error al cargar las imágenes." << endl;
        return -1;
    }

    // Paso 1: XOR entre IO e IM -> P1
    unsigned char* P1 = XOR(IO, IM, widthIO * heightIO * 3);
    cout << "Paso 1 completado: P1 generado." << endl;

    // Paso 2: Rotar P1 3 bits a la derecha -> P2
    unsigned char* P2 = RotateBits(P1, widthIO * heightIO * 3, 3, false);
    cout << "Paso 2 completado: P2 generado." << endl;

    // Paso 3: XOR entre P2 e IM -> P3
    unsigned char* P3 = XOR(P2, IM, widthIO * heightIO * 3);
    cout << "Paso 3 completado: P3 generado." << endl;

    // Cargar las imágenes de referencia
    int widthP1Ref = 0, heightP1Ref = 0;
    int widthP2Ref = 0, heightP2Ref = 0;
    int widthP3Ref = 0, heightP3Ref = 0;

    unsigned char* P1Ref = loadPixels(archivoP1Ref, widthP1Ref, heightP1Ref);
    unsigned char* P2Ref = loadPixels(archivoP2Ref, widthP2Ref, heightP2Ref);
    unsigned char* P3Ref = loadPixels(archivoP3Ref, widthP3Ref, heightP3Ref);

    if (!P1Ref || !P2Ref || !P3Ref) {
        cout << "Error al cargar las imágenes de referencia." << endl;
        return -1;
    }

    // Comparar las imágenes generadas con las de referencia
    bool isP1Valid = compareImages(P1, P1Ref, widthIO, heightIO);
    bool isP2Valid = compareImages(P2, P2Ref, widthIO, heightIO);
    bool isP3Valid = compareImages(P3, P3Ref, widthIO, heightIO);

    cout << "Comparación de P1: " << (isP1Valid ? "Correcto" : "Incorrecto") << endl;
    cout << "Comparación de P2: " << (isP2Valid ? "Correcto" : "Incorrecto") << endl;
    cout << "Comparación de P3: " << (isP3Valid ? "Correcto" : "Incorrecto") << endl;

    // Liberar memoria
    delete[] IO;
    delete[] IM;
    delete[] M;
    delete[] P1;
    delete[] P2;
    delete[] P3;
    delete[] P1Ref;
    delete[] P2Ref;
    delete[] P3Ref;

    return 0;
}

/**
  * @brief loadPixels
  *
  * @param input
  * @param width
  * @param height
  * @return unsigned*
  */

unsigned char* loadPixels(QString input, int &width, int &height) {

    // Cargar la imagen BMP desde el archivo especificado
    QImage imagen(input);

    // Verifica si la imagen fue cargada correctamente
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen desde " << input.toStdString() << endl;
        return nullptr;
    }

    // Convierte la imagen al formato RGB888 (3 canales de 8 bits sin transparencia)
    imagen = imagen.convertToFormat(QImage::Format_RGB888);
    if (imagen.format() != QImage::Format_RGB888) {
        cout << "Error: No se pudo convertir la imagen al formato RGB888." << endl;
        return nullptr;
    }

    // Obtiene el ancho y el alto de la imagen cargada
    width = imagen.width();
    height = imagen.height();
    cout << "Dimensiones de la imagen: " << width << "x" << height << endl;

    // Calcula el tamaño total de datos (3 bytes por píxel: R, G, B)
    int dataSize = width * height * 3;
    if (dataSize <= 0) {
        cout << "Error: Tamaño de datos inválido: " << dataSize << endl;
        return nullptr;
    }

    // Reserva memoria dinámica para almacenar los valores RGB de cada píxel
    unsigned char* pixelData = new unsigned char[dataSize];

    // Copia cada línea de píxeles de la imagen Qt a nuestro arreglo lineal
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);              // Línea original de la imagen con posible padding
        unsigned char* dstLine = pixelData + y * width * 3;     // Línea destino en el arreglo lineal sin padding
        memcpy(dstLine, srcLine, width * 3);                    // Copia los píxeles RGB de esa línea (sin padding)
    }

    cout << "Imagen cargada correctamente desde: " << input.toStdString() << endl;
    return pixelData;
}

/**
  * @brief Exporta una imagen en formato BMP a partir de un arreglo de píxeles en formato RGB.
  *
  * Esta función crea una imagen de tipo QImage utilizando los datos contenidos en el arreglo dinámico
  * `pixelData`, que debe representar una imagen en formato RGB888 (3 bytes por píxel, sin padding).
  * A continuación, copia los datos línea por línea a la imagen de salida y guarda el archivo resultante
  * en formato BMP en la ruta especificada.
  *
  * @param pixelData Puntero a un arreglo de bytes que contiene los datos RGB de la imagen a exportar.
  *                  El tamaño debe ser igual a width * height * 3 bytes.
  * @param width Ancho de la imagen en píxeles.
  * @param height Alto de la imagen en píxeles.
  * @param archivoSalida Ruta y nombre del archivo de salida en el que se guardará la imagen BMP (QString).
  *
  * @return true si la imagen se guardó exitosamente; false si ocurrió un error durante el proceso.
  *
  * @note La función no libera la memoria del arreglo pixelData; esta responsabilidad recae en el usuario.
  */
bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida){


    // Crear una nueva imagen de salida con el mismo tamaño que la original
    // usando el formato RGB888 (3 bytes por píxel, sin canal alfa)
    QImage outputImage(width, height, QImage::Format_RGB888);

    // Copiar los datos de píxeles desde el buffer al objeto QImage
    for (int y = 0; y < height; ++y) {
        // outputImage.scanLine(y) devuelve un puntero a la línea y-ésima de píxeles en la imagen
        // pixelData + y * width * 3 apunta al inicio de la línea y-ésima en el buffer (sin padding)
        // width * 3 son los bytes a copiar (3 por píxel)
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    // Guardar la imagen en disco como archivo BMP
    if (!outputImage.save(archivoSalida, "BMP")) {
        // Si hubo un error al guardar, mostrar mensaje de error
        cout << "Error: No se pudo guardar la imagen BMP modificada.";
        return false; // Indica que la operación falló
    } else {
        // Si la imagen fue guardada correctamente, mostrar mensaje de éxito
        cout << "Imagen BMP modificada guardada como " << archivoSalida.toStdString() << endl;
        return true; // Indica éxito
    }

}

/**
  *  @brief Carga la semilla y los resultados del enmascaramiento desde un archivo de texto.
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
unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels){


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


//Operacion XOR entre dos imágenes
unsigned char* XOR(unsigned char* img1, unsigned char* img2, int size) {
    unsigned char* result = new unsigned char[size];
    for (int i = 0; i < size; ++i) {
        result[i] = img1[i] ^ img2[i];
    }

    return result;
}


//Rotar bits derecha o izquierda
unsigned char* RotateBits(unsigned char *img, int size, int bits, bool direction) {
    if (img == nullptr || size <= 0) return nullptr;

    unsigned char* resultado = new unsigned char[size];

    for (int i = 0; i < size; ++i) {
        if (direction) {
            resultado[i] = (img[i] << bits) | (img[i] >> (8 - bits));
        } else {
            resultado[i] = (img[i] >> bits) | (img[i] << (8 - bits));
        }
    }

    return resultado;
}

//desplazar bits
unsigned char* ShiftBits(unsigned char *img, int size, int bits, bool direction) {
    if (img == nullptr || size <= 0) return nullptr;

    unsigned char* resultado = new unsigned char[size];

    for (int i = 0; i < size; ++i) {
        if (direction) {
            resultado[i] = img[i] << bits;
        } else {
            resultado[i] = img[i] >> bits;
        }
    }

    return resultado;
}

//Comparar dos imágenes
bool compareImages(unsigned char *img1, unsigned char *img2, int width, int height) {
    int size = width * height * 3; // 3 bytes por píxel (RGB)
    for (int i = 0; i < size; ++i) {
        if (img1[i] != img2[i]) {
            return false; // Las imágenes son diferentes
        }
    }
    return true; // Las imágenes son iguales
}

bool verifyMasking(unsigned char* transformedImage, unsigned char* mask, int maskWidth, int maskHeight, int seed, const unsigned int* maskingData, int n_pixels) {
    // Validar que las dimensiones sean válidas
    if (maskWidth <= 0 || maskHeight <= 0 || transformedImage == nullptr || mask == nullptr || maskingData == nullptr) {
        cout << "Error: Dimensiones inválidas o datos nulos." << endl;
        return false;
    }

    // Iterar sobre los datos de enmascaramiento
    for (int k = 0; k < n_pixels * 3; k += 3) {
        // Calcular el índice del píxel en la imagen transformada usando el desplazamiento (seed)
        int pixelIndex = (k / 3 + seed); //% (maskWidth * maskHeight);

        // Convertir el índice lineal a coordenadas 2D en la imagen transformada
        //int pixelY = pixelIndex / maskWidth;
        //int pixelX = pixelIndex % maskWidth;

        // Ajustar las coordenadas para que se alineen con la máscara (repetir la máscara si es necesario)
        //int maskX = (k / 3) % maskWidth;
        //int maskY = (k / 3) / maskWidth;

        // Calcular el índice lineal correspondiente en la máscara
        //int maskIndex = (maskY * maskWidth + maskX) * 3;

        // Realizar la suma de los valores RGB
        int r = transformedImage[pixelIndex * 3] + mask[k];
        int g = transformedImage[pixelIndex * 3 + 1] + mask[k + 1];
        int b = transformedImage[pixelIndex * 3 + 2] + mask[k + 2];

        // Comparar con los datos de enmascaramiento
        if (r != maskingData[k] || g != maskingData[k + 1] || b != maskingData[k + 2]) {
            return false; // Los valores no coinciden
        }
    }

    return true; // Todos los valores coinciden
}













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
 
 int main()
 {
     // Definición de rutas de archivo de entrada (imagen original) y salida (imagen modificada)
     QString archivoEntrada = "C:/Users/goken/OneDrive/Escritorio/2025-1/Informatica II/DesafioI_InformaticaII/DesafíoI/Caso 2/I_D.bmp";
     QString archivoSalida = "C:/Users/goken/OneDrive/Escritorio/2025-1/Informatica II/DesafioI_InformaticaII/DesafíoI/Code/I_O.bmp";
 
     // Variables para almacenar las dimensiones de la imagen
     int height = 0;
     int width = 0;
 
     // Carga la imagen BMP en memoria dinámica y obtiene ancho y alto
     unsigned char *pixelData = loadPixels(archivoEntrada, width, height);
 
 
     // Exporta la imagen modificada a un nuevo archivo BMP
     bool exportI = exportImage(pixelData, width, height, archivoSalida);
 
     // Muestra si la exportación fue exitosa (true o false)
     cout << exportI << endl;
 
     // Libera la memoria usada para los píxeles
     delete[] pixelData;
     pixelData = nullptr;
 
     // Variables para almacenar la semilla y el número de píxeles leídos del archivo de enmascaramiento
     int seed = 0;
     int n_pixels = 0;
 
     // Carga los datos de enmascaramiento desde un archivo .txt (semilla + valores RGB)
     unsigned int *maskingData = loadSeedMasking("M1.txt", seed, n_pixels);
 
     // Muestra en consola los primeros valores RGB leídos desde el archivo de enmascaramiento
     for (int i = 0; i < n_pixels * 3; i += 3) {
         cout << "Pixel " << i / 3 << ": ("
              << maskingData[i] << ", "
              << maskingData[i + 1] << ", "
              << maskingData[i + 2] << ")" << endl;
     }
 
     // Libera la memoria usada para los datos de enmascaramiento
     if (maskingData != nullptr){
         delete[] maskingData;
         maskingData = nullptr;
     }
 
     return 0; // Fin del programa
 
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
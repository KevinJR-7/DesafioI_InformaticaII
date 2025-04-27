// FILEOPS_H
#ifndef FILEOPS_H
#define FILEOPS_H

#include <QString>

using namespace std;

/**
 * @brief Loads the base images required for the decryption process.
 * 
 * This function loads three images from the specified files:
 * - Distorted image (ID)
 * - Masking image (IM)
 * - Initial mask image (M)
 * 
 * The images are loaded into memory as arrays of bytes (unsigned char*).
 * Their dimensions (width and height) are also retrieved.
 * 
 * @param InputFile Path to the distorted image file (I_D.bmp).
 * @param fileIM Path to the masking image file (I_M.bmp).
 * @param fileMask Path to the initial mask image file (M.bmp).
 * @param ID Reference to the pointer where the loaded distorted image will be stored.
 * @param IM Reference to the pointer where the loaded masking image will be stored.
 * @param M Reference to the pointer where the loaded initial mask image will be stored.
 * @param width Reference where the width of the distorted image will be stored.
 * @param height Reference where the height of the distorted image will be stored.
 * @param mWidth Reference where the width of the masking image will be stored.
 * @param mHeight Reference where the height of the masking image will be stored.
 * @param Mwidth Reference where the width of the initial mask image will be stored.
 * @param Mheight Reference where the height of the initial mask image will be stored.
 * 
 * @return true if all images were successfully loaded, false otherwise.
 * 
 * @note The user is responsible for freeing the allocated memory for the images after use.
 */
bool LoadImages(QString InputFile, QString fileIM, QString fileMask,
    unsigned char*& ID, unsigned char*& IM, unsigned char*& M,
    int& width, int& height, int& mWidth, int& mHeight, int& Mwidth, int& Mheight);



/**
 * @brief Load a BMP image and convert it to an array of pixels in RGB888 format.
 * 
 * This function loads a BMP image from the specified path, converts it to RGB888 format,
 * and stores the pixel data in a dynamically allocated array. The width and height of the image
 * are also returned through reference parameters.
 * 
 * @param input Route of the image file to load. It should be a valid and accessible file.
 * @param width Reference to a variable that will store the width of the loaded image.
 * @param height Reference to a variable that will store the height of the loaded image.
 * @return A pointer to a dynamically allocated array of type `unsigned char` that contains the pixel data of the image in RGB888 format (3 bytes per pixel).
 * If the loading or conversion of the image fails, it returns `nullptr`.
 * 
 * @note The returned pixel array must be freed with `delete[]` after use to avoid memory leaks.
 * 
 * @warning If the image cannot be loaded or converted to RGB888 format, the function returns `nullptr` and prints an error message to the console.
 */
unsigned char* loadPixels(QString input, int &width, int &height);

/**
 * @brief  Saves pixel data to a BMP image file.
 * 
 * This function takes pixel data in RGB888 format and saves it to a BMP file at the specified path.
 * The pixel data should be stored in an array of type `unsigned char*`, with a size of `width * height * 3` 
 * (3 bytes per pixel for RGB888).
 * 
 * 
 * @param pixelData Pointer to an array containing the pixel data of the image in RGB888 format. The array must have at 
 *                  least `width * height * 3` bytes.
 * @param width Width of the image to be saved in pixels.
 * @param height Height of the image to be saved in pixels.
 * @param outputFile Route of the file where the resulting image will be saved. It must be a valid and accessible file for writing.
 * @return `true` if the image was successfully saved to the specified file, `false` if an error occurred during the save process.
 * 
 * @note The image is saved in BMP format, ensuring that the pixel data in RGB888 format is properly converted to the image format.
 * 
 * @warning If the pixel data or dimensions are not valid, or if an error occurs while saving the image, the function returns `false`. 
 */
bool exportImage(unsigned char* pixelData, int width, int height, QString outputFile);

/**
 * @brief Saves the final image to a specified file.
 * 
 * This function takes the final image data, width, height, and output file path,
 * and saves the image to the specified output file using the `exportImage` function.
 * If saving fails, an error message is displayed.
 * 
 * @param finalFile Pointer to the final image data (pixel data in memory).
 * @param width The width of the image.
 * @param height The height of the image.
 * @param outputFile The path where the image should be saved.
 * 
 * @return `true` if the image is saved successfully, `false` if an error occurred.
 * 
 * @note This function uses the `exportImage` function to handle the actual saving process.
 */
bool saveFinalFile(unsigned char* finalFile, int width, int height, QString outputFile);


/**
 * @brief Generates the filename for a mask file based on the given index.
 *
 * This function generates the file path for a mask file (e.g., `M0.txt`, `M1.txt`, etc.)
 * based on the provided index. It returns the path to the mask file in the format `../data/M{index}.txt`,
 * where `{index}` is the value of the input `index`.
 *
 * @param index The index of the mask file. It must be a non-negative integer less than `totalFiles`.
 * @param totalFiles The total number of available mask files. The `index` should be less than this value.
 * 
 * @return A string representing the file path of the mask file. If the index is out of range, an empty string
 *         is returned and an error message is printed.
 *
 * @note The returned file path is relative to the `../data/` directory and assumes the files are named
 *       sequentially as `M{index}.txt`.
 *
 * @warning If `index` is out of range (i.e., greater than or equal to `totalFiles` or negative), 
 *          an empty string is returned, and an error message is logged.
 */

string FileM(int indice, int totalFiles);

/**
 * @brief Compares two images pixel by pixel.
 *
 * This function checks if two images are identical by comparing each corresponding pixel.
 * It assumes that both images are in RGB888 format (3 bytes per pixel) and have the same dimensions.
 *
 * @param img1 Pointer to the pixel data of the first image.
 * @param img2 Pointer to the pixel data of the second image.
 * @param width Width of the images in pixels.
 * @param height Height of the images in pixels.
 * 
 * @return `true` if the images are identical, `false` otherwise.
 *
 * @note Both images must have the same dimensions and format. If any pointer is `nullptr` 
 *       or the width/height is non-positive, the function returns `false`.
 */
bool compareImages(unsigned char *img1, unsigned char *img2, int width, int height);


#endif // FILEOPS_H

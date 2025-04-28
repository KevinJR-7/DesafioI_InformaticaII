// masking.h
#ifndef MASKING_H
#define MASKING_H

#include <QString>
/**
 * @brief Loads the seed and masking results from a text file.
 * This function opens a text file that contains a seed on the first line and,
 * subsequently, a list of RGB values resulting from the masking process.
 * First, it counts how many pixel triplets there are, then reserves dynamic memory
 * and finally loads the values into an integer array.
 * 
 * @param Filename Path to the text file containing the seed and RGB values.
 * @param seed Reference variable where the integer value of the seed will be stored.
 * @param n_pixels Reference variable where the number of pixels read will be stored
 *                (equivalent to the number of lines after the seed).
 * 
 * @return Pointer to a dynamic array of integers containing the RGB values
 *         in sequential order (R, G, B, R, G, B, ...). Returns nullptr if an error occurs while opening the file.
 * 
 * @note It is the user's responsibility to free the memory reserved with delete[].
 */
unsigned int* loadSeedMasking(const char* Filename, int &seed, int &n_pixels);

/**
 * @brief Verifies the transformation of an image using a mask and compares it with a transformed image.
 *
 * This function checks if each pixel of the transformed image, after applying the mask and a seed offset, 
 * matches the expected result based on the given relationship S(k) = ID(k + s) + M(k) for 0 ≤ k < i × j × 3.
 * Knowing that i and j are the dimensions of the mask image.
 * It performs this check for each pixel in the image. If any discrepancy is found, it returns false.
 *
 * @param transformedImage A pointer to the transformed image pixels.
 * @param mask A pointer to the mask image pixels.
 * @param RGB An array holding the RGB values for comparison.
 * @param n_pixels An integer representing the number of pixels in the image (in 3-channel format).
 * @param seed An integer representing the seed value for pixel offset.
 * 
 * @return true if the transformation is valid and the mask matches the transformed image; false otherwise.
 */
bool verifyMask(unsigned char* transformedImage, unsigned char* mask, unsigned int* RGB, int &n_pixels, int &seed);

#endif // MASKING_H
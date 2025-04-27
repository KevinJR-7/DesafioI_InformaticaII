// TRANSFORMATIONS_H
#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

/**
 * @brief XORs two images pixel by pixel.
 * 
 * @param img1 First image.
 * @param img2 Second image.
 * @param size Size of the images in bytes.
 * @return XORed image. 
 * 
 * @note The function allocates memory for the XORed image. The caller is responsible for freeing it.
 * 
 */
unsigned char* XOR(unsigned char* img1, unsigned char* img2, int size);


/**
 * @brief Rotates the bits of an image to the left or right.
 * 
 * @param img Image to be rotated.
 * @param size Size of the image in bytes.
 * @param bits Bits to rotate (1-7).
 * @param direction True for left rotation, false for right rotation.
 * @return Rotated image. 
 * 
 * @note The function allocates memory for the rotated image. The caller is responsible for freeing it.
 */
unsigned char* RotateBits(unsigned char *img, int size, int bits, bool direction);

/**
 * @brief 
 * 
 */
int verifyTransformation(
    unsigned char* inputImage, unsigned char* IM, unsigned char* mask,
    unsigned int* RGB, int size, int n_pixels, int seed);
#endif // TRANSFORMATIONS_H
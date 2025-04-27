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
 * @brief Verifies the transformation applied to the input image.
 * 
 * This function tries different transformations on the input image and checks 
 * if any of them match the provided mask and reference data using a verification process.
 * It first checks for the XOR transformation and then checks for various right bit rotations.
 * 
 * @param inputImage Pointer to the input image (distorted image) in memory.
 * @param IM Pointer to the masking image in memory.
 * @param mask Pointer to the initial mask image in memory.
 * @param RGB Pointer to the reference data used for verifying the transformations (pixel data).
 * @param size The size of the images in bytes (width * height * 3).
 * @param n_pixels The number of pixels to verify from the mask.
 * @param seed The seed value used for the masking verification.
 * 
 * @return 
 * - `0` if the XOR transformation is detected.
 * - `1` to `7` if a right rotation of `i` bits is detected (where `i` is the rotation amount).
 * - `-1` if no transformation is found.
 * 
 * This function applies the transformations and checks if they match the mask using the verifyMask function.
 * If a valid transformation is found, it returns the corresponding transformation identifier.
 * If none are found, it returns `-1`.
 */

int verifyTransformation(
    unsigned char* inputImage, unsigned char* IM, unsigned char* mask,
    unsigned int* RGB, int size, int n_pixels, int seed);

/**
 * @brief Processes a specific stage of the decryption, applying the correct transformation.
 * 
 * This function handles a decryption stage by determining the appropriate transformation 
 * based on a mask file, and applies the inverse transformation (XOR or rotation).
 * 
 * @param currentImage Pointer to the current image data (pixel data).
 * @param IM Pointer to the mask image data (used for XOR and rotation operations).
 * @param M Pointer to the initial mask image data (used in mask verification).
 * @param stage The current stage of the decryption process (starting from 1).
 * @param Mtxt The total number of stages to be processed.
 * @param size The total size of the image (in bytes).
 * 
 * @return A pointer to the processed image data after applying the inverse transformation, 
 *         or `nullptr` if there was an error or an unrecognized transformation.
 * 
 * @note The function dynamically generates the mask file name, loads the corresponding 
 *       data, and applies either XOR or a rotation depending on the detected transformation.
 * 
 * @note The transformation verification process uses the `verifyTransformation` function 
 *       to check for the appropriate operation (XOR or rotation).
 * 
 * @see verifyTransformation()
 * @see XOR()
 * @see RotateBits()
 */
unsigned char* processStage(unsigned char* currentImage, unsigned char* IM, unsigned char* M, 
    int stage, int Mtxt, int size);

#endif // TRANSFORMATIONS_H
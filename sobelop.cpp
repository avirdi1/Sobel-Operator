#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// Both stb h files were not written by me. These 2 files are how I can use my own images.
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// These are the main sobel kernels
vector<vector<int>> sobelX = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
vector<vector<int>> sobelY = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

// Function to apply the Sobel operator to calculate gradient at a pixel
int applySobel(vector<vector<int>>& image, int x, int y, vector<vector<int>>& kernel) {
    int gradient = 0;
    int kernelSize = kernel.size();
    int halfK = kernelSize / 2;
    
    // Using -halfK is an easier way to make sure you iterate over the correct range
    for (int i = -halfK; i <= halfK; i++) {
        for (int j = -halfK; j <= halfK; j++) {
            int pixelX = x + j;
            int pixelY = y + i;
            int value = 0;
            
            // Check if we are in bounds and can procced with setting the value in bounds
            if (pixelX >= 0 && pixelX < image[0].size() && pixelY >= 0 && pixelY < image.size()) {
                value = image[pixelY][pixelX];
            }
            
            // The value for the gradient is set so it can be used to determine the magnitude later on
            gradient += kernel[i + halfK][j + halfK] * value;
        }
    }
    
    return gradient;
}

// Function to calculate gradient magnitude using Sobel operators
vector<vector<int>> sobelOperator(vector<vector<int>>& image) {
    // 2D vector initialized to size of image which has a magnitude for each pixel so you know 
    // if you should darken image edge or not.
    vector<vector<int>> gradientMagnitude(image.size(), vector<int>(image[0].size(), 0));
    
    /* We are now traversing over every pixel and applying the sobel operator to a vector of vectors 
     which will have gradientMagnitude[x][y] to tell if you need to darken the image. Typical order for
     image processing is iterating over rows,y, first before columns, x, within each row.
     The formula for gradient calculation is in report.
    */
    for (int y = 0; y < image.size(); y++) {
        for (int x = 0; x < image[y].size(); x++) {
            int gradientX = applySobel(image, x, y, sobelX);
            int gradientY = applySobel(image, x, y, sobelY);
            int magnitude = sqrt(gradientX * gradientX + gradientY * gradientY);
            gradientMagnitude[y][x] = magnitude;
        }
    }
    
    return gradientMagnitude;
}

// To test the code you have to go into your file explorer, find an image, copy the path, replace '\' as '/' and you should be set.
// EX: "C:\Users\anmol\OneDrive\Pictures\Camera Roll\door hinge.jpg" -> "C:/Users/anmol/OneDrive/Pictures/Camera Roll/door hinge.jpg"
int main() {
    // Load image using the stb_image file function
    const char* file = "C:/Users/anmol/OneDrive/Pictures/Screenshots/skelly.jpg";
    int width, height, channels;
    unsigned char* image_d = stbi_load(file, &width, &height, &channels, 0);
    
    // Simple check to see if the image was properly loaded so code doesn't break.
    if (!image_d) {
        cerr << "Failed to load image." << endl;
        return 1;
    }

    vector<vector<int>> image;

    // We are going to go over every pixel to convert it into grayscale
    for (int y = 0; y < height; y++) {
        vector<int> row;
        for (int x = 0; x < width; x++) {
            /* Since we are working with values of color RGB, we can use a formula like the 
             luminence conversion. 0.2126 = red, 0.7152 = green, 0.0722 = blue. And for each pixel we 
             access each channel, red, green, and blue to convert each value to gray.
             */
            int gray_value = (int)(0.2126 * image_d[(y * width + x) * channels] +
                                    0.7152 * image_d[(y * width + x) * channels + 1] +
                                    0.0722 * image_d[(y * width + x) * channels + 2]);
            row.push_back(gray_value);
        }
        image.push_back(row);
    }

    // We are going to apply the soble operator to the image to detect magnitudes of edges.
    vector<vector<int>> gradientMagnitude = sobelOperator(image);

    // Using the gradientMagnitudes found in the sobelOperator function, we are 
    // going to now darken the edges by using the magnitude found.
    vector<unsigned char> output_image(width * height * 3);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int magnitude = gradientMagnitude[y][x];
            // To acces the correct indices we have to multipy the pixel index by 3.
            // This is because we use the magnitue for all 3 channels, RGB.
            output_image[(y * width + x) * 3] = magnitude; // red channel
            output_image[(y * width + x) * 3 + 1] = magnitude; // green channel
            output_image[(y * width + x) * 3 + 2] = magnitude; // blue channel
        }
    }

    // Save the new image created by the sobel operator using the stbi_image_write file functions
    stbi_write_jpg("output.jpg", width, height, 3, output_image.data(), 100);

    // Free image data to avoid memory leaks
    stbi_image_free(image_d);

    return 0;
}

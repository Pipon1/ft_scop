#include "../include/image_loader.hpp"
#include <algorithm>
#include <cstring>

GLuint LoadBMP(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open BMP file: " << filename << std::endl;
        return 0;
    }

    // BMP Header structure (14 bytes for BMP header + 40 bytes for DIB header)
    unsigned char header[54];
    file.read(reinterpret_cast<char*>(header), 54);

    // Verify the BMP file by checking the "BM" identifier
    if (header[0] != 'B' || header[1] != 'M')
    {
        std::cerr << "Not a valid BMP file: " << filename << std::endl;
        return 0;
    }

    // Extract image dimensions from the BMP header
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int offset = *(int*)&header[10];
    int bitsPerPixel = *(short*)&header[28];

    // Ensure it's a 24-bit BMP file (RGB)
    if (bitsPerPixel != 24)
    {
        std::cerr << "Only 24-bit BMP files are supported: " << filename << std::endl;
        return 0;
    }

    // Calculate the size of the image data (with padding for 4-byte alignment)
    int rowPadded = (width * 3 + 3) & (~3);
    std::vector<unsigned char> data(rowPadded * height);

    // Read pixel data (BMP is stored bottom-to-top)
    file.seekg(offset, std::ios::beg);
    file.read(reinterpret_cast<char*>(data.data()), data.size());
    file.close();

    // Rearrange the data for OpenGL (BMP stores in BGR format, so we need RGB)
    std::vector<unsigned char> rgbData(width * height * 3);
    std::cout << height << std::endl;
    std::cout << width << std::endl;
    /*for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int bmpIndex = (height - 1 - y) * rowPadded + x * 3;
            int rgbIndex = (y * width + x) * 3;
            std::cout << bmpIndex << " vs " << rgbIndex << std::endl;
            rgbData[rgbIndex] = data[bmpIndex + 2];     // Red
            rgbData[rgbIndex + 1] = data[bmpIndex + 1]; // Green
            rgbData[rgbIndex + 2] = data[bmpIndex];     // Blue
        }
    }*/

    // Generate a texture ID and bind it
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Specify the texture with the RGB data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data.data());

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}
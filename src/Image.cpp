/// Image module implementation

#include <arpa/inet.h> // htons
#include <string>
#include <cmath>

#include "Utility.hpp"
#include "Image.hpp"

namespace kpeg
{
    Image::Image() :
        width{0},
        height{0},
        m_pixelPtr{nullptr}
    {
        logFile << "Created new Image object" << std::endl;
    }
    
    void Image::createImageFromMCUs(const std::vector<MCU>& MCUs)
    {
        logFile << "Creating Image from MCU vector..." << std::endl;
        
        int mcuNum = 0;
        
        int jpegWidth = width % 8 == 0 ? width : width + 8 - (width % 8);
        int jpegHeight = height % 8 == 0 ? height : height + 8 - (height % 8);
        
        // Create a pixel pointer of size (Image width) x (Image height)
        m_pixelPtr = std::make_shared<std::vector<std::vector<Pixel>>>(
            jpegHeight, std::vector<Pixel>(jpegWidth, Pixel()));
        
        // Populate the pixel pointer based on data from the specified MCUs
        for (int y = 0; y <= jpegHeight - 8; y += 8)
        {
            for (int x = 0; x <= jpegWidth - 8; x += 8)
            {
                auto pixelBlock = MCUs[mcuNum].getAllMatrices();
                
                for (int v = 0; v < 8; ++v)
                {
                    for (int u = 0; u < 8; ++u)
                    {
                        (*m_pixelPtr)[y + v][x + u].comp[0] = pixelBlock[0][v][u]; // R
                        (*m_pixelPtr)[y + v][x + u].comp[1] = pixelBlock[1][v][u]; // G
                        (*m_pixelPtr)[y + v][x + u].comp[2] = pixelBlock[2][v][u]; // B
                    }
                }
            
                mcuNum++;
            }
        }
        
        // Trim the image width to nearest multiple of 8
        if (width != jpegWidth)
        {
            for (auto&& row : *m_pixelPtr)
                for (int c = 0; c < 8 - width % 8; ++c)
                    row.pop_back();
        }
        
        // Trim the image height to nearest multiple of 8
        if (height != jpegHeight)
        {
            for (int c = 0; c < 8 - height % 8; ++c)
                m_pixelPtr->pop_back();
        }        

        logFile << "Finished created Image from MCU [OK]" << std::endl;
    }
    
    const bool Image::dumpRawData(const std::string& filename)
    {
        if (m_pixelPtr == nullptr)
        {
            logFile << "Unable to create dump file \'" + filename + "\', Invalid pixel pointer" << std::endl;
            return false;
        }
        
        std::ofstream dumpFile(filename, std::ios::out);
        
        if (!dumpFile.is_open() || !dumpFile.good())
        {
            logFile << "Unable to create dump file \'" + filename + "\'." << std::endl;
            return false;
        }
        
        dumpFile << "P6" << std::endl;
        dumpFile << "# PPM dump created using libKPEG: https://github.com/TheIllusionistMirage/libKPEG" << std::endl;
        dumpFile << width << " " << height << std::endl;
        dumpFile << 255 << std::endl;
        
        for (auto&& row : *m_pixelPtr)
        {
            for (auto&& pixel : row)
                dumpFile << (UInt8)pixel.comp[RGBComponents::RED]
                         << (UInt8)pixel.comp[RGBComponents::GREEN]
                         << (UInt8)pixel.comp[RGBComponents::BLUE];
        }
        
        logFile << "Raw image data dumped to file: \'" + filename + "\'." << std::endl;
        dumpFile.close();
        return true;
    }
}

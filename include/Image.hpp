/// Image module
///
/// Abstraction representing a raw uncompressed image

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>

#include "Types.hpp"
#include "MCU.hpp"

namespace kpeg
{    
    /// Image is an abstraction for a raw, uncompressed image
    ///
    /// A raw, uncompressed image is nothing but a 2D array of pixels
    class Image
    {
        public:
            
            /// Default constructor
            Image();
            
            /// Create an image from a list of MCUs
            ///
            /// @param MCUs list of minimum coded units that can be converted to an image
            void createImageFromMCUs(const std::vector<MCU>& MCUs);
            
            /// Write the raw, uncompressed image data to specified file on the disk.
            ///
            /// The data written is in PPM format
            ///
            /// @param filename the location in the disk to write the image data
            /// @return true if succeeds in writing, else false
            const bool dumpRawData(const std::string& filename);
            
        public:
            
            /// Width of the image
            std::size_t width;

            /// Height of the image
            std::size_t height;

        private:
            /// The 2D pixel array with discrete range
            PixelPtr m_pixelPtr;
    };
}

#endif // IMAGE_HPP

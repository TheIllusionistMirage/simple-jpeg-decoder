/// Types module
///
/// Contains definition of standard types

#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <array>
#include <utility>
#include <memory>

namespace kpeg
{
    /// Standard, cross-platform word sizes for channels
    ///
    /// The channels we deal with here are Red, Green and Blue channels
    /// in the RGB color model  and the Y, Cb and Cr channels in the
    /// Y-Cb-Cr color model.

    /// Standard unsigned integral types
    typedef unsigned char  UInt8;
    typedef unsigned short UInt16;
    
    /// Standard signed integral types
    typedef char  Int8;
    typedef short Int16;
    
    /// Enum for identifying channels in RGB color model
    enum RGBComponents
    {
        RED   ,
        GREEN ,
        BLUE
    };
    
    /// Pixel types
    ///
    /// These types are an abstraction of dealing with raw
    /// image data in terms of the individual pixel level
    ///
    /// The pixel types used here use three channels
    /// (or components)
    
    /// Pixel with integral (discrete) channel range
    struct Pixel
    {
        /// Default constructor
        ///
        /// By default a pixel is initialized to its lowest brightness value
        Pixel()
        {
            comp[0] = comp[1] = comp[2]  = 0;
        }
        
        /// Parameterized constructor
        ///
        /// @param comp1 - Intensity value of pixel component 1
        /// @param comp2 - Intensity value of pixel component 2
        /// @param comp3 - Intensity value of pixel component 3
        Pixel(const Int16 comp1, const Int16 comp2, const Int16 comp3)
        {
            comp[0] = comp1;
            comp[1] = comp2;
            comp[2] = comp3;
        }
        
        /// Store the intensity of the pixel
        Int16 comp[3];
    };
    
    /// Aliases for commonly used types

    /// A 2D array of pixels with integral (discrete) components
    typedef std::shared_ptr<std::vector<std::vector<Pixel>>>  PixelPtr;

    /// Huffman table
    typedef std::array<std::pair<int, std::vector<UInt8>>, 16> HuffmanTable;
    
    /// Identifiers used to access a Huffman table based on the class and ID
    /// E.g., To access the Huffman table for the DC coefficients of the
    /// CbCr component, we use `huff_table[HT_DC][HT_CbCr]`.
    const int HT_DC   = 0;
    const int HT_AC   = 1;
    const int HT_Y    = 0;
    const int HT_CbCr = 1;
}

#endif // TYPES_HPP

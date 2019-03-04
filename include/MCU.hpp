/// Minimum Coded Unit (MCU) module
///
/// Represents an abstraction for a MCU. An MCU is a block of 8x8 pixels.
/// This abstracts away the conversion of the decoded RLE-Huffman encoded pixel values
/// to corresponding matrices of 8x8 size. It actually contains three 8x8 matrices to
/// represent the lumninance (Y) and chrominance (Cb & Cr) components.
/// 
/// The MCU object expects as input the RLE-Huffman encoded vector (obtained after
/// the Huffman decoding is done).

#ifndef MCU_HPP
#define MCU_HPP

#include <array>
#include <vector>
#include <utility>

#include "Types.hpp"
#include "Transform.hpp"

namespace kpeg
{
    /// Alias for a 8x8 pixel block with integral values for its channels
    typedef std::array<std::array<std::array<int, 8>, 8>, 3> CompMatrices;
    
    /// Alias for a 8x8 matrix with integral elements
    typedef std::array< std::array< int, 8 >, 8 > Matrix8x8;
    
    class MCU
    {
        public:
            
            /// The total number of MCUs in the image
            static int m_MCUCount;
            
            /// The quantization table used for de-quantization
            static std::vector<std::vector<UInt16>> m_QTables;

        public:
            
            /// Default constructor
            MCU();
            
            /// Parameterized constructor
            ///
            /// Initialze the MCU with run-length encoding per
            /// channel and the quantization tables to use
            ///
            /// @param compRLE the run-length encoding for the MCU
            /// @param QTables the quantization tables to be used for encoding the MCU
            MCU(const std::array<std::vector<int>, 3>& compRLE,
                const std::vector<std::vector<UInt16>>& QTables);
            
            /// Create the MCU from the specified run-length encoding and quantization tables
            ///
            /// @param compRLE the run-length encoding for the MCU
            /// @param QTables the quantization tables to be used for encoding the MCU
            void constructMCU(const std::array<std::vector<int>, 3>& compRLE,
                              const std::vector<std::vector<UInt16>>& QTables);
            
            /// Get the pixel arrays for the pixels under this MCU.
            ///
            /// Since there are three channels per MCU, three pixel arrays will be returned.
            const CompMatrices& getAllMatrices() const;
        
        private:
            
            /// Inverse discrete cosine transform
            ///
            /// The 8x8 matrices for each component has to be converted
            /// back from frequency to spaital domain.
            void computeIDCT();
            
            /// Level shift the pixel data to center it within the pixel value range
            void performLevelShift();
            
            /// Convert the MCU's underlying pixels from the Y-Cb-Cr color model to RGB color model
            void convertYCbCrToRGB();
            
        private:
            
            /// The pixel arrays for the three channels in the MCU
            CompMatrices m_block;

            /// The order of the MCU in the image
            int m_order;
            
            /// The differences in the DC coefficients per channel
            static int m_DCDiff[3];
            
            // For storing the IDCT coefficients before level shifting
            std::array<std::array<std::array<float, 8>, 8>, 3> m_IDCTCoeffs;
    };
}

#endif // MCU_HPP

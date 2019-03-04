// The JFIF file markers

#ifndef MARKERS_HPP
#define MARKERS_HPP

#include <string>

#include "Image.hpp"

namespace kpeg
{
    const UInt16 JFIF_BYTE_0    = 0x00;
    
    /// JPEG-JFIF File Markers
    ///
    /// Refer to ITU-T.81 (09/92), page 32
    const UInt16 JFIF_BYTE_FF    = 0xFF; // All markers start with this as the MSB                  
    const UInt16 JFIF_SOF0       = 0xC0; // Start of Frame 0, Baseline DCT                           
    const UInt16 JFIF_SOF1       = 0xC1; // Start of Frame 1, Extended Sequential DCT               
    const UInt16 JFIF_SOF2       = 0xC2; // Start of Frame 2, Progressive DCT                       
    const UInt16 JFIF_SOF3       = 0xC3; // Start of Frame 3, Lossless (Sequential)                 
    const UInt16 JFIF_DHT        = 0xC4; // Define Huffman Table                                    
    const UInt16 JFIF_SOF5       = 0xC5; // Start of Frame 5, Differential Sequential DCT           
    const UInt16 JFIF_SOF6       = 0xC6; // Start of Frame 6, Differential Progressive DCT          
    const UInt16 JFIF_SOF7       = 0xC7; // Start of Frame 7, Differential Loessless (Sequential)   
    const UInt16 JFIF_SOF9       = 0xC9; // Extended Sequential DCT, Arithmetic Coding              
    const UInt16 JFIF_SOF10      = 0xCA; // Progressive DCT, Arithmetic Coding                      
    const UInt16 JFIF_SOF11      = 0xCB; // Lossless (Sequential), Arithmetic Coding                
    const UInt16 JFIF_SOF13      = 0xCD; // Differential Sequential DCT, Arithmetic Coding          
    const UInt16 JFIF_SOF14      = 0xCE; // Differential Progressive DCT, Arithmetic Coding         
    const UInt16 JFIF_SOF15      = 0xCF; // Differential Lossless (Sequential), Arithmetic Coding   
    const UInt16 JFIF_SOI        = 0xD8; // Start of Image                                          
    const UInt16 JFIF_EOI        = 0xD9; // End of Image                                            
    const UInt16 JFIF_SOS        = 0xDA; // Start of Scan                                           
    const UInt16 JFIF_DQT        = 0xDB; // Define Quantization Table
    const UInt16 JFIF_APP0       = 0xE0; // Application Segment 0, JPEG-JFIF Image
    const UInt16 JFIF_COM        = 0xFE; // Comment
}

#endif // MARKERS_HPP

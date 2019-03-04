/// A simple abstraction for the decoder of a JPEG decoder
///
/// Decoder module is the implementation of a 8-bit Sequential
/// Baseline DCT, grayscale/RGB encoder with no subsampling (4:4:4)

#ifndef DECODER_HPP
#define DECODER_HPP

#include <fstream>
#include <vector>
#include <utility>
#include <bitset>

#include "Types.hpp"
#include "Image.hpp"
#include "HuffmanTree.hpp"
#include "MCU.hpp"

namespace kpeg
{
    class Decoder
    {
        public:

            /// The result of a decode operation
            enum ResultCode
            {
                SUCCESS,
                TERMINATE,
                ERROR,
                DECODE_INCOMPLETE,
                DECODE_DONE
            };
            
        public:
            
            /// Default constructor
            Decoder();
            
            /// Initialize decoder with info about image file
            Decoder(const std::string& filename);
            
            /// Destructor
            ~Decoder();
            
            /// Open a JFIF image file for decoding
            bool open(const std::string& filename);
            
            /// Decode the image in the JFIF file
            ResultCode decodeImageFile();

            /// Write raw, uncompressed image data to disk in PPM format
            bool dumpRawData();

            /// Close the JFIF file
            void close();
                        
        private:

            /// Parse the info of the specified segment in the JFIF file
            ResultCode parseSegmentInfo(const UInt8 byte);
            
            /// Parse the JFIF segment at the very beginning of the JFIF file
            void parseAPP0Segment();

            /// Parse the comment in the JFIF file
            void parseCOMSegment();
            
            /// Parse the quantization tables specified in the JFIF file
            void parseDQTSegment();
            
            /// Parse the Start of File segment
            ResultCode parseSOF0Segment();
            
            /// Parse the Huffman tables specified in the JFIF file
            void parseDHTSegment();
            
            /// Parse the start of scan segment in the JFIF file
            void parseSOSSegment();
            
            /// Parse the actual compressed image data stored in the JFIF file
            void scanImageData();
            
            // Convert bytes of the form XXFF00YY to just XXFFYY
            void byteStuffScanData();
            
            /// Decode the RLE-Huffman encoded image pixel data
            ///
            /// This function goes bit by bit over the image scan data
            /// and decodes it using the provided DC and AC Huffman tables
            /// for luminance (Y) and chrominance (Cb & Cr)
            void decodeScanData();
            
        private:
            
            // void displayHuffmanCodes();
            
        private:
            
            std::string m_filename;
            
            std::ifstream m_imageFile;
            
            Image m_image;
            
            std::vector<std::vector<UInt16>> m_QTables;
            
            // For i=0..3:
            //    HT_i is array of size=16, where j-th element is < count-j-bits, symbol-list >
            //
            HuffmanTable m_huffmanTable[2][2];
            
            // std::vector< std::pair<int, int> > mDHTsScanned;
            
            HuffmanTree m_huffmanTree[2][2];
            
            // Image scan data
            std::string m_scanData;
            
            std::vector<MCU> m_MCU;
    };
}

#endif // DECODER_HPP

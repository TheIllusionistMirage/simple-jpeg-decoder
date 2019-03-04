/// Implementation of the decoder

#include <arpa/inet.h> // htons
#include <iomanip>
#include <sstream>

#include "Decoder.hpp"
#include "Markers.hpp"
#include "Utility.hpp"

namespace kpeg
{
    Decoder::Decoder()
    {
        logFile << "Created \'Decoder object\'." << std::endl;
    }
            
    Decoder::Decoder(const std::string& filename)
    {
        logFile << "Created \'Decoder object\'." << std::endl;
    }
    
    Decoder::~Decoder()
    {
        close();
        logFile << "Destroyed \'Decoder object\'." << std::endl;
    }
    
    bool Decoder::open(const std::string& filename)
    {
        m_imageFile.open(filename, std::ios::in | std::ios::binary);
        
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable to open image: \'" + filename + "\'" << std::endl;
            return false;
        }
        
        logFile << "Opened JPEG image: \'" + filename + "\'" << std::endl;
        
        m_filename = filename;
        
        return true;
    }
    
    void Decoder::close()
    {
        m_imageFile.close();
        logFile << "Closed image file: \'" + m_filename + "\'" << std::endl;
    }
    
    Decoder::ResultCode Decoder::parseSegmentInfo(const UInt8 byte)
    {
        if (byte == JFIF_BYTE_0 || byte == JFIF_BYTE_FF)
            return ERROR;
        
        switch(byte)
        {
            case JFIF_SOI  : logFile  << "Found segment, Start of Image (FFD8)" << std::endl; return ResultCode::SUCCESS;
            case JFIF_APP0 : logFile  << "Found segment, JPEG/JFIF Image Marker segment (APP0)" << std::endl; parseAPP0Segment(); return ResultCode::SUCCESS;
            case JFIF_COM  : logFile  << "Found segment, Comment(FFFE)" << std::endl; parseCOMSegment(); return ResultCode::SUCCESS;
            case JFIF_DQT  : logFile  << "Found segment, Define Quantization Table (FFDB)" << std::endl; parseDQTSegment(); return ResultCode::SUCCESS;
            case JFIF_SOF0 : logFile  << "Found segment, Start of Frame 0: Baseline DCT (FFC0)" << std::endl; return parseSOF0Segment();
            case JFIF_SOF1 : logFile << "Found segment, Start of Frame 1: Extended Sequential DCT (FFC1), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF2 : logFile << "Found segment, Start of Frame 2: Progressive DCT (FFC2), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF3 : logFile << "Found segment, Start of Frame 3: Lossless Sequential (FFC3), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF5 : logFile << "Found segment, Start of Frame 5: Differential Sequential DCT (FFC5), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF6 : logFile << "Found segment, Start of Frame 6: Differential Progressive DCT (FFC6), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF7 : logFile << "Found segment, Start of Frame 7: Differential lossless (Sequential) (FFC7), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF9 : logFile << "Found segment, Start of Frame 9: Extended Sequential DCT, Arithmetic Coding (FFC9), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF10: logFile << "Found segment, Start of Frame 10: Progressive DCT, Arithmetic Coding (FFCA), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF11: logFile << "Found segment, Start of Frame 11: Lossless (Sequential), Arithmetic Coding (FFCB), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF13: logFile << "Found segment, Start of Frame 13: Differentical Sequential DCT, Arithmetic Coding (FFCD), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF14: logFile << "Found segment, Start of Frame 14: Differentical Progressive DCT, Arithmetic Coding (FFCE), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_SOF15: logFile << "Found segment, Start of Frame 15: Differentical Lossless (Sequential), Arithmetic Coding (FFCF), Not supported" << std::endl; return ResultCode::TERMINATE;
            case JFIF_DHT  : logFile  << "Found segment, Define Huffman Table (FFC4)" << std::endl; parseDHTSegment(); return ResultCode::SUCCESS;
            case JFIF_SOS  : logFile  << "Found segment, Start of Scan (FFDA)" << std::endl; parseSOSSegment(); return ResultCode::SUCCESS;
        }
        
        return ResultCode::SUCCESS;
    }
    
    bool Decoder::dumpRawData()
    {
        std::size_t extPos = m_filename.find(".jpg");
        
        if (extPos == std::string::npos)
            extPos = m_filename.find(".jpeg");
        
        std::string targetFilename = m_filename.substr(0, extPos) + ".ppm";
        m_image.dumpRawData(targetFilename);
        
        return true;
    }
    
    Decoder::ResultCode Decoder::decodeImageFile()
    {
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return ResultCode::ERROR;
        }
        
        logFile << "Started decoding process..." << std::endl;
        
        UInt8 byte;
        ResultCode status = ResultCode::DECODE_DONE;
        
        while (m_imageFile >> std::noskipws >> byte)
        {
            if (byte == JFIF_BYTE_FF)
            {
                m_imageFile >> std::noskipws >> byte;
                
                ResultCode code = parseSegmentInfo(byte);
                
                if (code == ResultCode::SUCCESS)
                    continue;
                else if (code == ResultCode::TERMINATE)
                {
                    status = ResultCode::TERMINATE;
                    break;
                }
                else if (code == ResultCode::DECODE_INCOMPLETE)
                {
                    status = ResultCode::DECODE_INCOMPLETE;
                    break;
                }
            }
            else
            {
                logFile << "[ FATAL ] Invalid JFIF file! Terminating..." << std::endl;
                status = ResultCode::ERROR;
                break;
            }
        }
        
        if (status == ResultCode::DECODE_DONE)
        {
            decodeScanData();
            m_image.createImageFromMCUs(m_MCU);
            logFile << "Finished decoding process [OK]." << std::endl;
        }
        else if (status == ResultCode::TERMINATE)
        {
            logFile << "Terminated decoding process [NOT-OK]." << std::endl;
        }
        
        else if (status == ResultCode::DECODE_INCOMPLETE)
        {
            logFile << "Decoding process incomplete [NOT-OK]." << std::endl;
        }
        
        return status;
    }
    
    void Decoder::parseAPP0Segment()
    {
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return;
        }
        
        logFile << "Parsing JPEG/JFIF marker segment (APP-0)..." << std::endl;
        
        UInt16 lenByte = 0;
        UInt8 byte = 0;
        
        m_imageFile.read(reinterpret_cast<char *>(&lenByte), 2);
        lenByte = htons(lenByte);
        std::size_t curPos = m_imageFile.tellg();
        
        logFile << "JFIF Application marker segment length: " << lenByte << std::endl;
        
        // Skip the 'JFIF\0' bytes
        m_imageFile.seekg(5, std::ios_base::cur);
        
        UInt8 majVersionByte, minVersionByte;
        m_imageFile >> std::noskipws >> majVersionByte >> minVersionByte;
        
        logFile << "JFIF version: " << (int)majVersionByte << "." << (int)(minVersionByte >> 4) << (int)(minVersionByte & 0x0F) << std::endl;
        
        std::string majorVersion = std::to_string(majVersionByte);
        std::string minorVersion = std::to_string((int)(minVersionByte >> 4));
        minorVersion +=  std::to_string((int)(minVersionByte & 0x0F));
        
        UInt8 densityByte;
        m_imageFile >> std::noskipws >> densityByte;
        
        std::string densityUnit = "";
        switch(densityByte)
        {
            case 0x00: densityUnit = "Pixel Aspect Ratio"; break;
            case 0x01: densityUnit = "Pixels per inch (DPI)"; break;
            case 0x02: densityUnit = "Pixels per centimeter"; break;
        }
        
        logFile << "Image density unit: " << densityUnit << std::endl;
        
        UInt16 xDensity = 0, yDensity = 0;
        
        m_imageFile.read(reinterpret_cast<char *>(&xDensity), 2);
        m_imageFile.read(reinterpret_cast<char *>(&yDensity), 2);
        
        xDensity = htons(xDensity);
        yDensity = htons(yDensity);
        
        logFile << "Horizontal image density: " << xDensity << std::endl;
        logFile << "Vertical image density: " << yDensity << std::endl;
        
        // Ignore the image thumbnail data
        UInt8 xThumb = 0, yThumb = 0;
        m_imageFile >> std::noskipws >> xThumb >> yThumb;        
        m_imageFile.seekg(3 * xThumb * yThumb, std::ios_base::cur);
        
        logFile << "Finished parsing JPEG/JFIF marker segment (APP-0) [OK]" << std::endl;
    }
    
    void Decoder::parseDQTSegment()
    {
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return;
        }
        
        logFile << "Parsing quantization table segment..." << std::endl;
        
        UInt16 lenByte = 0;
        UInt8 PqTq;
        UInt8 Qi;
        
        m_imageFile.read(reinterpret_cast<char *>(&lenByte), 2);
        lenByte = htons(lenByte);
        logFile << "Quantization table segment length: " << (int)lenByte << std::endl;
        
        lenByte -= 2;
        
        for (int qt = 0; qt < int(lenByte) / 65; ++qt)
        {
            m_imageFile >> std::noskipws >> PqTq;
            
            int precision = PqTq >> 4; // Precision is always 8-bit for baseline DCT
            int QTtable = PqTq & 0x0F; // Quantization table number (0-3)
            
            logFile << "Quantization Table Number: " << QTtable << std::endl;
            logFile << "Quantization Table #" << QTtable << " precision: " << (precision == 0 ? "8-bit" : "16-bit") << std::endl;
            
            m_QTables.push_back({});
            
            // Populate quantization table #QTtable            
            for (auto i = 0; i < 64; ++i)
            {
                m_imageFile >> std::noskipws >> Qi;
                m_QTables[QTtable].push_back((UInt16)Qi);
            }
        }
        
        logFile << "Finished parsing quantization table segment [OK]" << std::endl;
    }
    
    Decoder::ResultCode Decoder::parseSOF0Segment()
    {
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return ResultCode::ERROR;
        }
        
        logFile << "Parsing SOF-0 segment..." << std::endl;
        
        UInt16 lenByte, imgHeight, imgWidth;
        UInt8 precision, compCount;
        
        m_imageFile.read(reinterpret_cast<char *>(&lenByte), 2);
        lenByte = htons(lenByte);
        
        logFile << "SOF-0 segment length: " << (int)lenByte << std::endl;
        
        m_imageFile >> std::noskipws >> precision;
        logFile << "SOF-0 segment data precision: " << (int)precision << std::endl;
        
        m_imageFile.read(reinterpret_cast<char *>(&imgHeight), 2);
        m_imageFile.read(reinterpret_cast<char *>(&imgWidth), 2);
        
        imgHeight = htons(imgHeight);
        imgWidth = htons(imgWidth);
        
        logFile << "Image height: " << (int)imgHeight << std::endl;
        logFile << "Image width: " << (int)imgWidth << std::endl;
        
        m_imageFile >> std::noskipws >> compCount;
        
        logFile << "No. of components: " << (int)compCount << std::endl;
        
        UInt8 compID = 0, sampFactor = 0, QTNo = 0;
        
        bool isNonSampled = true;
        
        for (auto i = 0; i < 3; ++i)
        {
            m_imageFile >> std::noskipws >> compID >> sampFactor >> QTNo;
            
            logFile << "Component ID: " << (int)compID << std::endl;
            logFile << "Sampling Factor, Horizontal: " << int(sampFactor >> 4) << ", Vertical: " << int(sampFactor & 0x0F) << std::endl;
            logFile << "Quantization table no.: " << (int)QTNo << std::endl;
            
            if ((sampFactor >> 4) != 1 || (sampFactor & 0x0F) != 1)
                isNonSampled = false;
        }
        
        if (!isNonSampled)
        {
            logFile << "Chroma subsampling not yet supported!" << std::endl;
            logFile << "Chroma subsampling is not 4:4:4, terminating..." << std::endl;
            return ResultCode::TERMINATE;
        }
        
        logFile << "Finished parsing SOF-0 segment [OK]" << std::endl;        
        m_image.width = imgWidth;
        m_image.height = imgHeight;
        
        return ResultCode::SUCCESS;
    }
    
    void Decoder::parseDHTSegment()
    {   
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return;
        }
        
        logFile << "Parsing Huffman table segment..." << std::endl;
        
        UInt16 len;
        m_imageFile.read(reinterpret_cast<char *>(&len), 2);
        len = htons(len);
        
        logFile << "Huffman table length: " << (int)len << std::endl;
        
        int segmentEnd = (int)m_imageFile.tellg() + len - 2;
        
        while (m_imageFile.tellg() < segmentEnd)
        {
            UInt8 htinfo;
            m_imageFile >> std::noskipws >> htinfo;
            
            int HTType = int((htinfo & 0x10) >> 4);
            int HTNumber = int(htinfo & 0x0F);
            
            logFile << "Huffman table type: " << HTType << std::endl;
            logFile << "Huffman table #: " << HTNumber << std::endl;
            
            int totalSymbolCount = 0;
            UInt8 symbolCount;
            
            for (auto i = 1; i <= 16; ++i)
            {
                m_imageFile >> std::noskipws >> symbolCount;
                m_huffmanTable[HTType][HTNumber][i-1].first = (int)symbolCount;
                totalSymbolCount += (int)symbolCount;
            }
            
            // Load the symbols
            int syms = 0;
            for (auto i = 0; syms < totalSymbolCount; )
            {
                // Read the next symbol, and add it to the
                // proper slot in the Huffman table.
                //
                // Depndending upon the symbol count, say n, for the current
                // symbol length, insert the next n symbols in the symbol
                // list to it's proper spot in the Huffman table. This means,
                // if symbol counts for symbols of lengths 1, 2 and 3 are 0,
                // 5 and 2 respectively, the symbol list will contain 7
                // symbols, out of which the first 5 are symbols with length
                // 2, and the remaining 2 are of length 3.
                UInt8 code;
                m_imageFile >> std::noskipws >> code;
                
                if (m_huffmanTable[HTType][HTNumber][i].first == 0)
                {
                    while (m_huffmanTable[HTType][HTNumber][++i].first == 0);
                }
                
                m_huffmanTable[HTType][HTNumber][i].second.push_back(code);
                syms++;
                
                if (m_huffmanTable[HTType][HTNumber][i].first == m_huffmanTable[HTType][HTNumber][i].second.size())
                    i++;
            }
            
            logFile << "Printing symbols for Huffman table (" << HTType << "," << HTNumber << ")..." << std::endl;
            
            int totalCodes = 0;
            for (auto i = 0; i < 16; ++i)
            {
                std::string codeStr = "";
                for (auto&& symbol : m_huffmanTable[HTType][HTNumber][i].second)
                {
                    std::stringstream ss;
                    ss << "0x" << std::hex << std::setfill('0') << std::setw(2) << std::setprecision(16) << (int)symbol;
                    codeStr += ss.str() + " ";
                    totalCodes++;
                }
                
                logFile << "Code length: " << i+1
                                        << ", Symbol count: " << m_huffmanTable[HTType][HTNumber][i].second.size()
                                        << ", Symbols: " << codeStr << std::endl;
            }
            
            logFile << "Total Huffman codes for Huffman table(Type:" << HTType << ",#:" << HTNumber << "): " << totalCodes << std::endl;
            
            m_huffmanTree[HTType][HTNumber].constructHuffmanTree(m_huffmanTable[HTType][HTNumber]);
            auto htree = m_huffmanTree[HTType][HTNumber].getTree();
            logFile << "Huffman codes:-" << std::endl;
            inOrder(htree);
        }
        
        logFile << "Finished parsing Huffman table segment [OK]" << std::endl;
    }
    
    void Decoder::parseSOSSegment()
    {
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return;
        }
        
        logFile << "Parsing SOS segment..." << std::endl;
        
        UInt16 len;
        
        m_imageFile.read(reinterpret_cast<char *>(&len), 2);
        len = htons(len);
        
        logFile << "SOS segment length: " << len << std::endl;
        
        UInt8 compCount; // Number of components
        UInt16 compInfo; // Component ID and Huffman table used
        
        m_imageFile >> std::noskipws >> compCount;
        
        if (compCount < 1 || compCount > 4)
        {
            logFile << "Invalid component count in image scan: " << (int)compCount << ", terminating decoding process..." << std::endl;
            return;
        }
        
        logFile << "Number of components in scan data: " << (int)compCount << std::endl;
        
        for (auto i = 0; i < compCount; ++i)
        {
            m_imageFile.read(reinterpret_cast<char *>(&compInfo), 2);
            compInfo = htons(compInfo);
            
            UInt8 cID = compInfo >> 8; // 1st byte denotes component ID 
            
            // 2nd byte denotes the Huffman table used:
            // Bits 7 to 4: DC Table #(0 to 3)
            // Bits 3 to 0: AC Table #(0 to 3)
            UInt8 DCTableNum = (compInfo & 0x00f0) >> 4;
            UInt8 ACTableNum = (compInfo & 0x000f);
            
            logFile << "Component ID: " << (int)cID << ", DC Table #: " << (int)DCTableNum << ", AC Table #: " << (int)ACTableNum << std::endl;
        }
        
        // Skip the next three bytes
        for (auto i = 0; i < 3; ++i)
        {
            UInt8 byte;
            m_imageFile >> std::noskipws >> byte;
        }
        
        logFile << "Finished parsing SOS segment [OK]" << std::endl;
        
        scanImageData();
    }
    
    void Decoder::scanImageData()
    {
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return;
        }
        
        logFile << "Scanning image data..." << std::endl;
        
        UInt8 byte;
        
        while (m_imageFile >> std::noskipws >> byte)
        {
            if (byte == JFIF_BYTE_FF)
            {
                UInt8 prevByte = byte;
                
                m_imageFile >> std::noskipws >> byte;
                
                if (byte == JFIF_EOI)
                {
                    logFile << "Found segment, End of Image (FFD9)" << std::endl;
                    return;
                }
                
                std::bitset<8> bits1(prevByte);
                logFile << "0x" << std::hex << std::setfill('0') << std::setw(2)
                                          << std::setprecision(8) << (int)prevByte
                                          << ", Bits: " << bits1 << std::endl;
                                          
                m_scanData.append(bits1.to_string());
            }
            
            std::bitset<8> bits(byte);
            logFile << "0x" << std::hex << std::setfill('0') << std::setw(2)
                                      << std::setprecision(8) << (int)byte
                                      << ", Bits: " << bits << std::endl;
            
            m_scanData.append(bits.to_string());
        }
        
        logFile << "Finished scanning image data [OK]" << std::endl;
    }
    
    void Decoder::parseCOMSegment()
    {
        if (!m_imageFile.is_open() || !m_imageFile.good())
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return;
        }
        
        logFile << "Parsing comment segment..." << std::endl;
        
        UInt16 lenByte = 0;
        UInt8 byte = 0;
        std::string comment;
        
        m_imageFile.read(reinterpret_cast<char *>(&lenByte), 2);
        lenByte = htons(lenByte);
        std::size_t curPos = m_imageFile.tellg();
        
        logFile << "Comment segment length: " << lenByte << std::endl;
        
        for (auto i = 0; i < lenByte - 2; ++i)
        {
            m_imageFile >> std::noskipws >> byte;
            
            if (byte == JFIF_BYTE_FF)
            {
                logFile << "Unexpected start of marker at offest: " << curPos + i << std::endl;
                logFile << "Comment segment content: " << comment << std::endl;
                return;
            }
            
            comment.push_back(static_cast<char>(byte));
        }
        
        logFile << "Comment segment content: " << comment << std::endl;
        logFile << "Finished parsing comment segment [OK]" << std::endl;
    }
    
    void Decoder::byteStuffScanData()
    {
        if (m_scanData.empty())
        {
            logFile << " [ FATAL ] Invalid image scan data" << std::endl;
            return;
        }
        
        logFile << "Byte stuffing image scan data..." << std::endl;
        
        for (unsigned i = 0; i <= m_scanData.size() - 8; i += 8)
        {
            std::string byte = m_scanData.substr(i, 8);
            
            if (byte == "11111111")
            {
                if (i + 8 < m_scanData.size() - 8)
                {
                    std::string nextByte = m_scanData.substr(i + 8, 8);
                    
                    if (nextByte == "00000000")
                    {
                        m_scanData.erase(i + 8, 8);
                    }
                    else continue;
                }
                else
                    continue;
            }
        }
        
        logFile << "Finished byte stuffing image scan data [OK]" << std::endl;
    }
    
    void Decoder::decodeScanData()
    {
        if (m_scanData.empty())
        {
            logFile << " [ FATAL ] Invalid image scan data" << std::endl;
            return;
        }
        
        byteStuffScanData();
        
        logFile << "Decoding image scan data..." << std::endl;
        
        const char* component[] = { "Y (Luminance)", "Cb (Chrominance)", "Cr (Chrominance)" };
        const char* type[] = { "DC", "AC" };        
        
        int MCUCount = (m_image.width * m_image.height) / 64;
        
        m_MCU.clear();
        logFile << "MCU count: " << MCUCount << std::endl;
        
        int k = 0; // The index of the next bit to be scanned
        
        for (auto i = 0; i < MCUCount; ++i)
        {
            logFile << "Decoding MCU-" << i + 1 << "..." << std::endl;
            
            // The run-length coding after decoding the Huffman data
            std::array<std::vector<int>, 3> RLE;            
            
            // For each component Y, Cb & Cr, decode 1 DC
            // coefficient and then decode 63 AC coefficients.
            //
            // NOTE:
            // Since a signnificant portion of a RLE for a
            // component contains a trail of 0s, AC coefficients
            // are decoded till, either an EOB (End of block) is
            // encountered or 63 AC coefficients have been decoded.
            
            for (auto compID = 0; compID < 3; ++compID)
            {
                std::string bitsScanned = ""; // Initially no bits are scanned
                
                // Firstly, decode the DC coefficient
                logFile << "Decoding MCU-" << i + 1 << ": " << component[compID] << "/" << type[HT_DC] << std::endl;
                
                int HuffTableID = compID == 0 ? 0 : 1;
                
                while (1)
                {       
                    bitsScanned += m_scanData[k];
                    auto value = m_huffmanTree[HT_DC][HuffTableID].contains(bitsScanned);
                    
                    if (!utils::isStringWhiteSpace(value))
                    {
                        if (value != "EOB")
                        {   
                            int zeroCount = UInt8(std::stoi(value)) >> 4 ;
                            int category = UInt8(std::stoi(value)) & 0x0F;
                            int DCCoeff = bitStringtoValue(m_scanData.substr(k + 1, category));
                            
                            k += category + 1;
                            bitsScanned = "";
                            
                            RLE[compID].push_back(zeroCount);
                            RLE[compID].push_back(DCCoeff);
                            
                            break;
                        }
                        
                        else
                        {
                            bitsScanned = "";
                            k++;
                            
                            RLE[compID].push_back(0);
                            RLE[compID].push_back(0);
                            
                            break;
                        }
                    }
                    else
                        k++;
                }
                
                // Then decode the AC coefficients
                logFile << "Decoding MCU-" << i + 1 << ": " << component[compID] << "/" << type[HT_AC] << std::endl;
                bitsScanned = "";
                int ACCodesCount = 0;
                                
                while (1)
                {   
                    // If 63 AC codes have been encountered, this block is done, move onto next block                    
                    if (ACCodesCount  == 63)
                    {
                        break;
                    }
                    
                    // Append the k-th bit to the bits scanned so far
                    bitsScanned += m_scanData[k];
                    auto value = m_huffmanTree[HT_AC][HuffTableID].contains(bitsScanned);
                    
                    if (!utils::isStringWhiteSpace(value))
                    {
                        if (value != "EOB")
                        {
                            int zeroCount = UInt8(std::stoi(value)) >> 4 ;
                            int category = UInt8(std::stoi(value)) & 0x0F;
                            int ACCoeff = bitStringtoValue(m_scanData.substr(k + 1, category));
                            
                            k += category + 1;
                            bitsScanned = "";
                            
                            RLE[compID].push_back(zeroCount);
                            RLE[compID].push_back(ACCoeff);
                            
                            ACCodesCount += zeroCount + 1;
                        }
                        
                        else
                        {
                            bitsScanned = "";
                            k++;
                            
                            RLE[compID].push_back(0);
                            RLE[compID].push_back(0);
                            
                            break;
                        }
                    }
                    
                    else
                        k++;
                }
                
                // If both the DC and AC coefficients are EOB, truncate to (0,0)
                if (RLE[compID].size() == 2)
                {
                    bool allZeros = true;
                    
                    for (auto&& rVal : RLE[compID])
                    {
                        if (rVal != 0)
                        {
                            allZeros = false;
                            break;
                        }
                    }
                    
                    // Remove the extra (0,0) pair
                    if (allZeros)
                    {
                        RLE[compID].pop_back();
                        RLE[compID].pop_back();
                    }
                }
            }
            
            // Construct the MCU block from the RLE &
            // quantization tables to a 8x8 matrix
            m_MCU.push_back(MCU(RLE, m_QTables));
            
            logFile << "Finished decoding MCU-" << i + 1 << " [OK]" << std::endl;
        }
        
        // The remaining bits, if any, in the scan data are discarded as
        // they're added byte align the scan data.
        
        logFile << "Finished decoding image scan data [OK]" << std::endl;
    }
}

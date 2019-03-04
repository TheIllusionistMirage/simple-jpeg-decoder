#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "Utility.hpp"
#include "MCU.hpp"

namespace kpeg
{
    int MCU::m_MCUCount = 0;
    std::vector<std::vector<UInt16>> MCU::m_QTables = {};
    int MCU::m_DCDiff[3] = { 0, 0, 0 };
    
    MCU::MCU()
    {   
    }
            
    MCU::MCU( const std::array<std::vector<int>, 3>& compRLE, const std::vector<std::vector<UInt16>>& QTables )
    {
        constructMCU( compRLE, QTables );
    }
    
    void MCU::constructMCU( const std::array<std::vector<int>, 3>& compRLE, const std::vector<std::vector<UInt16>>& QTables )
    {
        m_QTables = QTables;
        
        m_MCUCount++;
        m_order = m_MCUCount;
        
        logFile << "Constructing MCU: " << std::dec << m_order << "..." << std::endl;
        
        const char* component[] = { "Y (Luminance)", "Cb (Chrominance)", "Cr (Chrominance)" };
        const char* type[] = { "DC", "AC" };    
        
        for ( int compID = 0; compID < 3; compID++ )
        {
            // Initialize with all zeros
            std::array<int, 64> zzOrder;            
            std::fill( zzOrder.begin(), zzOrder.end(), 0 );
            int j = -1;
            
            for ( auto i = 0; i <= compRLE[compID].size() - 2; i += 2 )
            {
                if ( compRLE[compID][i] == 0 && compRLE[compID][i + 1] == 0 )
                    break;
                
                j += compRLE[compID][i] + 1; // Skip the number of positions containing zeros
                zzOrder[j] = compRLE[compID][i + 1];
            }
            
            // DC_i = DC_i-1 + DC-difference
            m_DCDiff[compID] += zzOrder[0];
            zzOrder[0] = m_DCDiff[compID];
            
            int QIndex = compID == 0 ? 0 : 1;
            for ( auto i = 0; i < 64; ++i ) // !!!!!! i = 1
                zzOrder[i] *= m_QTables[QIndex][i];
            
            // Zig-zag order to 2D matrix order
            for ( auto i = 0; i < 64; ++i )
            {
                auto coords = zzOrderToMatIndices( i );
                
                m_block[compID][ coords.first ][ coords.second ] = zzOrder[i];
            }
        }
        
        computeIDCT();
        performLevelShift();
        convertYCbCrToRGB();
        
        logFile << "Finished constructing MCU: " << m_order << "..." << std::endl;
    }
    
    const CompMatrices& MCU::getAllMatrices() const
    {
        return m_block;
    }
    
    void MCU::computeIDCT()
    {
        logFile << "Performing IDCT on MCU: " << m_order << "..." << std::endl;
        
        for ( int i = 0; i <3; ++i )
        {
            for ( int y = 0; y < 8; ++y )
            {
                for ( int x = 0; x < 8; ++x )
                {
                    float sum = 0.0;
                    
                    for ( int u = 0; u < 8; ++u )
                    {
                        for ( int v = 0; v < 8; ++v )
                        {
                            float Cu = u == 0 ? 1.0 / std::sqrt(2.0) : 1.0;
                            float Cv = v == 0 ? 1.0 / std::sqrt(2.0) : 1.0;
                            
                            sum += Cu * Cv * m_block[i][u][v] * std::cos( ( 2 * x + 1 ) * u * M_PI / 16.0 ) *
                                            std::cos( ( 2 * y + 1 ) * v * M_PI / 16.0 );
                        }
                    }
                    
                    m_IDCTCoeffs[i][x][y] = 0.25 * sum;
                }
            }
        }

        logFile << "IDCT of MCU: " << m_order << " complete [OK]" << std::endl;
    }
    
    void MCU::performLevelShift()
    {
        logFile << "Performing level shift on MCU: " << m_order << "..." << std::endl;
        
        for ( int i = 0; i <3; ++i )
        {
            for ( int y = 0; y < 8; ++y )
            {
                for ( int x = 0; x < 8; ++x )
                {
                    m_block[i][y][x] = std::roundl( m_IDCTCoeffs[i][y][x] ) + 128;
                }
            }
        }
        
        logFile << "Level shift on MCU: " << m_order << " complete [OK]" << std::endl;
    }
    
    void MCU::convertYCbCrToRGB()
    {
        logFile << "Converting from Y-Cb-Cr colorspace to R-G-B colorspace for MCU: " << m_order << "..." << std::endl;
        
        for ( int y = 0; y < 8; ++y )
        {
            for ( int x = 0; x < 8; ++x )
            {
                float Y = m_block[0][y][x];
                float Cb = m_block[1][y][x];
                float Cr = m_block[2][y][x];
                
                int R = (int)std::floor( Y + 1.402 * ( 1.0 * Cr - 128.0 ) );
                int G = (int)std::floor( Y - 0.344136 * ( 1.0 * Cb - 128.0 ) - 0.714136 * ( 1.0 * Cr - 128.0 ) );
                int B = (int)std::floor( Y + 1.772 * ( 1.0 * Cb - 128.0 ) );
                
                R = std::max( 0, std::min( R, 255 ) );
                G = std::max( 0, std::min( G, 255 ) );
                B = std::max( 0, std::min( B, 255 ) );
                
                m_block[0][y][x] = R;
                m_block[1][y][x] = G;
                m_block[2][y][x] = B;
            }
        }
        
        logFile << "Colorspace conversion for MCU: " << m_order << " done [OK]" << std::endl;
    }
}

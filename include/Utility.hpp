/// Common utilities module

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <cctype>
#include <fstream>

// Output log file, ugly solution global for logging
// into files! This approach is not recommended, I
// used here to focus on JPEG, not logging! :p
extern std::ofstream logFile;

namespace kpeg
{
    namespace utils
    {
        /// String helpers
        
        /// Check whether a character is valid in the context of JFIF file names
        ///
        /// @param ch character whose validity to test
        /// @return true if character is valid, else false
        inline const bool isValidChar(const char ch)
        {
            return isprint(ch) || ch != '/' || ch != '\\';
        }
        
        /// Check whether a file name is valid
        ///
        /// @param filename the file name whose validity to check
        /// @return true if the file name is valid, else false
        inline const bool isValidFilename(const std::string& filename)
        {
            // First make sure the filename contains only valid characters
            for (auto&& c : filename)
                if (!isValidChar(c))
                    return false;
            
            // Check if the correct extension is present in the file name
            std::size_t extPos = filename.find(".jpg");
            
            if (extPos == std::string::npos)
                extPos = filename.find(".jpeg");
            
            else if (extPos + 4 == filename.size())
                return true;
            
            if (extPos == std::string::npos)
                return false;
            
            return false;
        }
        
        /// Check whether a character is a whitespace
        ///
        /// @param ch the character to check for whitespace
        /// @return true if character is whitespace, else return false
        inline const bool isWhiteSpace(const char ch)
        {
            return iscntrl(ch) || isblank(ch) || isspace(ch);
        }
        
        /// Check whether a string consists entirely of whitespaces
        ///
        /// @param str string under test
        /// @return true if the specified string consists entirely of whitespaces, else false
        inline const bool isStringWhiteSpace( const std::string& str )
        {
            for ( auto&& c : str )
                if ( !isWhiteSpace(c) )
                    return false;
            return true;
        }
    }
}

#endif // UTILITY_HPP

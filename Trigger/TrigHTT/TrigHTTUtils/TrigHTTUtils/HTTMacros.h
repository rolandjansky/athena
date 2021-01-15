/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTMacros.h:
 *
 * Defines various macro functions, macro constants, and global variable constants
 * used throughout HTTSim.
 */

#ifndef HTTMACROS_H
#define HTTMACROS_H

#include <string>
#include <stdexcept>

#define MTX_TOLERANCE 3e-16

///////////////////////////////////////////////////////////////////////////////
// ASSERT MACROS
///////////////////////////////////////////////////////////////////////////////

#define HTT_ASSERT_THROW(condition)                             \
{                                                               \
    if (!(condition))                                           \
    {                                                           \
        throw std::runtime_error(  std::string(__FILE__)        \
                                 + std::string(":")             \
                                 + std::to_string(__LINE__)     \
        );                                                      \
    }                                                           \
}


// Throws an error if a != b. a and b must admit std::to_string().
#define HTT_EQUAL_THROW(a, b)                                   \
{                                                               \
    if ((a) != (b))                                             \
    {                                                           \
        throw std::runtime_error(  std::string(__FILE__)        \
                                 + std::string(":")             \
                                 + std::to_string(__LINE__)     \
                                 + std::string(":: ")           \
                                 + std::to_string(a)            \
                                 + std::string(" != ")          \
                                 + std::to_string(b)            \
        );                                                      \
    }                                                           \
}


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS / ERROR CODES
///////////////////////////////////////////////////////////////////////////////

#define SSID_WILDCARD -1
#define SSID_ERROR -2
#define MODULE_BADMODULE -1
#define SECTOR_NOTFOUND -1
#define FITTRACKS_OK 0
#define FITTRACKS_BAD -1


// Uniform error codes
enum ErrorStatus
{
    ES_OK = 0,
    ES_HELP,            // --help was called (for executables)
    ES_MISSREQARG,      // missing a required argument
    ES_BADARG,          // generic error in a supplied argument
    ES_FAIL,            // generic fail
    ES_NOTFOUND,        // for functions that do a search, target not found
    ES_EOF,             // end of file or other seek
    ES_EMPTY,           // an argument or other object was empty
    ES_CONSTRAINT       // a user-supplied constraint was violated
};

///////////////////////////////////////////////////////////////////////////////
// DECODE ETACODE
///////////////////////////////////////////////////////////////////////////////

// see TrigHTTObjects/HTTHit.h

#define SECTION_INT(a) (a%1000 < 20 ? 0 : (a%10))
#define ETA_CODE_INT(a) (a%1000)
#define PHI_MOD_INT(a) (a/1000)
#define ETA_MOD_INT(a) (a%1000 < 20 ? a%1000 : (a%1000)/20-1)
#define ASIDE_INT(a) (a%1000 < 20 ? 0 : ((ETA_CODE_INT(a))/10)%2 == 0) //POSEC
#define CSIDE_INT(a) (a%1000 < 20 ? 0 : ((ETA_CODE_INT(a))/10)%2)      //NEGEC

///////////////////////////////////////////////////////////////////////////////
// SCOPED CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// TODO 6/6/19 Most of these are probably outdated / unused. Prune as needed

namespace htt
{
    constexpr int NHITTYPES(2);     ///< Number of coordinate types: PIXEL, STRIP
    constexpr int NDETREGS(2); // Barrel/Endcap

    constexpr int MAXPLANES(20);
    constexpr int NOASSIGNED(-999);
    constexpr int BARREL(0);
    constexpr int ENDCAP(1);
    constexpr int POSEC(2);
    constexpr int NEGEC(-2);
    constexpr double PI(3.14159265398);
    constexpr double COTTMAX(10.0178749);
    constexpr double CURVMAX(0.00065);

    // --- These are for converting HTTRawHit to HTTHit --- //
    constexpr int row_coord_mask    = 0x00000FFF; ///< Bitmask marking the location of the row coordinate in m_hw_word for pixel(12 bits)
    constexpr int phi_width_mask    = 0x00007000; ///< Bitmask marking the location of the row width in m_hw_word for pixel(3 bits)
    constexpr int split_mask        = 0x00008000; ///< Bitmask marking the location of the split cluster flag in m_hw_word for pixel(1 bit)
    constexpr int column_coord_mask = 0x0FFF0000; ///< Bitmask marking the location of the column coordinate in m_hw_word for pixel(12 bits)
    constexpr int eta_width_mask    = 0x70000000; ///< Bitmask marking the location of the column width in m_hw_word for pixel(3 bits)

    constexpr int row_coord_bit     = 0; ///< Starting bit of row coordinate in m_hw_word for pixel
    constexpr int phi_width_bit     = 12; ///< Starting bit of row width in m_hw_word for pixel
    constexpr int split_bit         = 15; ///< Bit for the split cluster flag in m_hw_word for pixel
    constexpr int column_coord_bit  = 16; ///<  Starting bit of column coordinate in m_hw_word for pixel
    constexpr int eta_width_bit     = 28; ///< Starting bit of column width in m_hw_word for pixel

    constexpr int strip_coord_mask = 0x07FF; ///< Bitmask marking the location of the strip coordinate in m_hw_word for STRIP (3 bits)
    constexpr int strip_coord_bit  = 0; ///< Starting bit of strip coordinate in m_hw_word for STRIP
    constexpr int strip_width_mask = 0x7000; ///< Bitmask marking the location of the strip width in m_hw_word  for STRIP (3 bits)
    constexpr int strip_width_bit  = 12; ///< Starting bit of strip width in m_hw_word for STRIP

    // --- This is the current HTTCluster to HTTHit scaling factor --- //
    constexpr float scaleHitFactor = 2;

    /////////////////////////////////////////////////////////////////////////////

}



#endif

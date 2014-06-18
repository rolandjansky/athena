// $Id: crc64.cxx,v 1.2 2007-03-08 02:02:06 ssnyder Exp $
/**
 * @file  SGTools/crc64.cxx
 * @author scott snyder, originally from David T. Jones
 * @date Mar 2007
 * @brief A CRC-64 implementation.
 */
/*
 * Original comments:
 * Improved calculation of CRC-64 values for protein sequences
 * By David T. Jones (dtj@cs.ucl.ac.uk)  - September 28th 2002
 * 
 * Modified from code at URL:
 * ftp://ftp.ebi.ac.uk/pub/software/swissprot/Swissknife/old/SPcrc.tar.gz
 *
 * Changed to C++ and moved into a namespace.
 */


#include "SGTools/crc64.h"
#include <cstdio>
using namespace std;


// I don't have a citation for the source of this polynomial.
// Maybe should replace it with the ECMA DLT poly?
#define POLY64REV     0x95AC9329AC4BC9B5ULL
#define INITIALCRC    0xFFFFFFFFFFFFFFFFULL

// Original SWISSPROT/TrEMBL poly.  Shown to be weak.
//#define POLY64REV	0xd800000000000000ULL
//#define INITIALCRC	0x0000000000000000ULL


namespace {

bool crc_init = false;
uint64_t CRCTable[256];

// Initialize the CRC table.
void init_table()
{
  crc_init = true;
  for (int i = 0; i < 256; i++)
  {
    uint64_t part = i;
    for (int j = 0; j < 8; j++)
    {
      if (part & 1)
        part = (part >> 1) ^ POLY64REV;
      else
        part >>= 1;
    }
    CRCTable[i] = part;
  }
}

} // anonymous namespace


namespace SG {


/**
 * @brief Find the CRC-64 of a string.
 * @param str The string to hash.
 */
uint64_t crc64 (const std::string& str)
{
  if (!crc_init)
    init_table();

  uint64_t crc = INITIALCRC;
  const char* seq = str.data();
  const char* end = seq + str.size();
  while (seq < end)
    crc = CRCTable[(crc ^ *seq++) & 0xff] ^ (crc >> 8);
  return crc;
}


/**
 * @brief Extend a previously-calculated CRC to include an int.
 * @param crc The previously-calculated CRC.
 * @param x The integer to add.
 * @return The new CRC.
 */
uint64_t crc64addint (uint64_t crc, unsigned int x)
{
  if (!crc_init)
    init_table();

  while (x > 0) {
    crc = CRCTable[(crc ^ x) & 0xff] ^ (crc >> 8);
    x >>= 8;
  }
  return crc;
}


/**
 * @brief Format a CRC-64 as a string.
 * @param crc The CRC to format.
 */
std::string crc64format (uint64_t crc)
{
  char buf[64];
  sprintf (buf, "%08X%08X",
           (unsigned)((crc>>32)&0xffffffff), (unsigned)(crc&0xffffffff));
  return buf;
}


/**
 * @brief Return a CRC-64 digest of a string.
 * @param str The string to hash.
 * @return The CRC-64 digest of the string.
 *         This is the hash code, expressed as hex as a string.
 */
std::string crc64digest (const std::string& str)
{
  return crc64format (crc64 (str));
}


} // namespace SG

/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id: crc64.cxx,v 1.2 2007-03-08 02:02:06 ssnyder Exp $
/**
 * @file  SGTools/crc64.cxx
 * @author scott snyder
 * @date Mar 2007
 * @brief A CRC-64 implementation.
 *
 * A good reference for CRC calculations is <https://zlib.net/crc_v3.txt>.
 */


#include "SGTools/crc64.h"
#include <cstdio>
using namespace std;


// Polynomial taken from code from David T. Jones (dtj@cs.ucl.ac.uk).
// http://www0.cs.ucl.ac.uk/staff/D.Jones/crcnote.pdf
#define POLY64REV     0x95AC9329AC4BC9B5ULL
#define INITIALCRC    0xFFFFFFFFFFFFFFFFULL


namespace {

class CRCTable
{
public:
  CRCTable();
  uint64_t table[256];
};


// Initialize the CRC table.
CRCTable::CRCTable()
{
  for (int i = 0; i < 256; i++)
  {
    uint64_t r = i;
    for (int j = 0; j < 8; j++)
    {
      if (r & 1)
        r = (r >> 1) ^ POLY64REV;
      else
        r >>= 1;
    }
    table[i] = r;
  }
}

static const CRCTable table;

} // anonymous namespace


namespace SG {


/**
 * @brief Find the CRC-64 of a string.
 * @param str The string to hash.
 */
uint64_t crc64 (const std::string& str)
{
  uint64_t crc = INITIALCRC;
  const char* seq = str.data();
  const char* end = seq + str.size();
  while (seq < end)
    crc = table.table[(crc ^ *seq++) & 0xff] ^ (crc >> 8);
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
  while (x > 0) {
    crc = table.table[(crc ^ x) & 0xff] ^ (crc >> 8);
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

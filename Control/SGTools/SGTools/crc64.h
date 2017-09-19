// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file  SGTools/crc64.h
 * @author scott snyder
 * @date Mar 2007
 * @brief A CRC-64 implementation.
 */

#ifndef SGTOOLS_CRC64_H
#define SGTOOLS_CRC64_H


#include <string>
#include <stdint.h>


namespace SG {


/**
 * @brief Find the CRC-64 of a string.
 * @param str The string to hash.
 */
uint64_t crc64 (const std::string& str);


/**
 * @brief Extend a previously-calculated CRC to include an int.
 * @param crc The previously-calculated CRC.
 * @param x The integer to add.
 * @return The new CRC.
 */
uint64_t crc64addint (uint64_t crc, unsigned int x);


/**
 * @brief Format a CRC-64 as a string.
 * @param crc The CRC to format.
 */
std::string crc64format (uint64_t crc);


/**
 * @brief Return a CRC-64 digest of a string.
 * @param str The string to hash.
 * @return The CRC-64 digest of the string.
 *         This is the hash code, expressed as hex as a string.
 */
std::string crc64digest (const std::string& str);


} // namespace SG



#endif // not SGTOOLS_CRC64_H

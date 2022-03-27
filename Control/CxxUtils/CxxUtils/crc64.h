// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
// $Id$
/**
 * @file CxxUtils/crc64.h
 * @author scott snyder <snyder@bnl.gov>
 * @brief A crc-64 implementation, using pclmul where possible.
 */


#ifndef CXXUTILS_CRC64_H
#define CXXUTILS_CRC64_H


#include "CxxUtils/features.h"
#include <memory>
#include <cstdint>
#include <string>


// Use the vectorized version if we're compiling with gcc on x86_64.
// clang > 7 allows function multiversioning but some of the intrinsics we 
// use exist only for gcc
#if defined(__x86_64__) && HAVE_GCC_INTRINSICS
# define ATH_CRC64_VEC 1
#else
# define ATH_CRC64_VEC 0
#endif


// Forward declarations.
namespace CxxUtils {
/// Opaque object holding CRC tables and constants.
class CRCTable;
/// Delete a CRCTable object.
void deleteCRCTable (CxxUtils::CRCTable* table);
}


/// Set a delete policy for unique_ptr<CRCTable>.
/// This allows us to use an incomplete declaration here, and define
/// the class only in the cxx file.
namespace std {
template<>
struct default_delete<CxxUtils::CRCTable>
{
  void operator() (CxxUtils::CRCTable* table) const
  { deleteCRCTable (table); }
};
}


namespace CxxUtils {


class CRCTable;


/**
 * @brief Initialize CRC tables and constants.
 * @param p Polynomial for the CRC.
 *          A 1 in the 2^64 bit is implied.
 * @param initial Initial CRC value.
 */
std::unique_ptr<CRCTable> makeCRCTable (uint64_t p,
                                        uint64_t initial = 0xffffffffffffffff);


/**
 * @brief Find the CRC-64 of a string, using a byte-by-byte algorithm.
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 */
uint64_t crc64_bytewise (const CRCTable& table,
                         const char* data,
                         size_t data_len);


/**
 * @brief Find the CRC-64 of a string, using a byte-by-byte algorithm,
 *        with the default CRC.
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 */
uint64_t crc64_bytewise (const char* data,
                         size_t data_len);


/**
 * @brief Find the CRC-64 of a string, using a byte-by-byte algorithm,
 *        with the default CRC.
 * @param table Precomputed CRC tables and constants.
 * @param s String to hash.
 */
uint64_t crc64_bytewise (const std::string& s);


#if ATH_CRC64_VEC
/**
 * @brief Find the CRC-64 of a string,
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 *
 * This is the vectorized implementation, used on platforms with pclmul.
 */
__attribute__ ((target ("pclmul")))
uint64_t crc64 (const CRCTable& table,
                const char* data,
                size_t data_len);
#endif // ATH_CRC64_VEC


/**
 * @brief Find the CRC-64 of a string,
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 *
 * This is the default implementation, used on platforms without pclmul.
 */
#if ATH_CRC64_VEC
__attribute__ ((target ("default")))
#endif
uint64_t crc64 (const CRCTable& table,
                const char* data,
                size_t data_len);


/**
 * @brief Find the CRC-64 of a string, with the default CRC.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 */
uint64_t crc64 (const char* data,
                size_t data_len);


/**
 * @brief Find the CRC-64 of a string, using the default polynomial.
 * @param str The string to hash.
 */
uint64_t crc64 (const std::string& s);


/**
 * @brief Extend a previously-calculated CRC to include an int.
 * @param crc The previously-calculated CRC.
 * @param x The integer to add.
 * @return The new CRC.
 */
uint64_t crc64addint (uint64_t crc, uint64_t x);


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


} // namespace CxxUtils


#endif // not CXXUTILS_CRC64_H

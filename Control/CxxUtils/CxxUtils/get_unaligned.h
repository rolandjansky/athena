// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/get_unaligned.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Read little-endian values through possibly unaligned pointers.
 *
 * Doing, eg,
 *@code
 *   char* p = ...;
 *   int* ip = reinterpret_cast<int*>(p);
 *   int i = *ip++;
 @endif
 *
 * is undefined if @c p isn't aligned to an int boundary.  It further
 * will read the value using the host endianness, which is an issue
 * for the common use case of reading persistent data.
 *
 * The functions here allow reading little-endian data via possibly
 * unaligned pointers regardless of the host byte ordering and
 * without relying on undefined behavior.
 */


#ifndef CXXUTILS_GET_UNALIGNED_H
#define CXXUTILS_GET_UNALIGNED_H


#include "CxxUtils/restrict.h"
#include <cstring>
#include <stdint.h>


#ifdef __linux__
# include <endian.h>
#else
namespace CxxUtils {
  inline uint16_t htole16 (uint16_t x) { return x; }
  inline uint32_t htole32 (uint32_t x) { return x; }
  inline uint64_t htole64 (uint64_t x) { return x; }
}
#endif


namespace CxxUtils {


/**
 * @brief Read a 2-byte little-endian value from a possibly unaligned pointer.
 * @param p Pointer from which to read.  Advanced to the next value.
 *
 * Reads a little-endian value, regardless of the host byte ordering,
 * and advances the pointer.
 * Should not rely on undefined behavior, regardless of the alignment of @c p.
 *
 * If used in a loop, you'll get better code with a restricted pointer.
 */
inline
uint16_t get_unaligned16 (const uint8_t* ATH_RESTRICT &  p)
{
  uint16_t ret;
  memcpy (&ret, p, sizeof(ret));
  p += sizeof(ret);
  return htole16 (ret);
}


/**
 * @brief Read a 4-byte little-endian value from a possibly unaligned pointer.
 * @param p Pointer from which to read.  Advanced to the next value.
 *
 * Reads a little-endian value, regardless of the host byte ordering,
 * and advances the pointer.
 * Should not rely on undefined behavior, regardless of the alignment of @c p.
 *
 * If used in a loop, you'll get better code with a restricted pointer.
 */
inline
uint32_t get_unaligned32 (const uint8_t* ATH_RESTRICT &  p)
{
  uint32_t ret;
  memcpy (&ret, p, sizeof(ret));
  p += sizeof(ret);
  return htole32 (ret);
}


/**
 * @brief Read a 8-byte little-endian value from a possibly unaligned pointer.
 * @param p Pointer from which to read.  Advanced to the next value.
 *
 * Reads a little-endian value, regardless of the host byte ordering,
 * and advances the pointer.
 * Should not rely on undefined behavior, regardless of the alignment of @c p.
 *
 * If used in a loop, you'll get better code with a restricted pointer.
 */
inline
uint64_t get_unaligned64 (const uint8_t* ATH_RESTRICT &  p)
{
  uint64_t ret;
  memcpy (&ret, p, sizeof(ret));
  p += sizeof(ret);
  return htole64 (ret);
}


/**
 * @brief Read little-endian float value from a possibly unaligned pointer.
 * @param p Pointer from which to read.  Advanced to the next value.
 *
 * Reads a little-endian value, regardless of the host byte ordering,
 * and advances the pointer.
 * Should not rely on undefined behavior, regardless of the alignment of @c p.
 *
 * If used in a loop, you'll get better code with a restricted pointer.
 */
inline
float get_unaligned_float (const uint8_t* ATH_RESTRICT &  p)
{
  union {
    float f;
    uint32_t i;
  } cnv;
  cnv.i = get_unaligned32 (p);
  return cnv.f;
}


/**
 * @brief Read little-endian float value from a possibly unaligned pointer.
 * @param p Pointer from which to read.  Advanced to the next value.
 *
 * Reads a little-endian value, regardless of the host byte ordering,
 * and advances the pointer.
 * Should not rely on undefined behavior, regardless of the alignment of @c p.
 *
 * If used in a loop, you'll get better code with a restricted pointer.
 */
inline
double get_unaligned_double (const uint8_t* ATH_RESTRICT &  p)
{
  union {
    double f;
    uint64_t i;
  } cnv;
  cnv.i = get_unaligned64 (p);
  return cnv.f;
}


/// Define templated versions of the above functions.


template <class T>
T get_unaligned (const uint8_t* ATH_RESTRICT &  p);


template <>
inline
uint8_t get_unaligned<uint8_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return *p++;
}


template <>
inline
uint16_t get_unaligned<uint16_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned16 (p);
}


template <>
inline
uint32_t get_unaligned<uint32_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned32 (p);
}


template <>
inline
uint64_t get_unaligned<uint64_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned64 (p);
}


template <>
inline
float get_unaligned<float> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned_float (p);
}


template <>
inline
double get_unaligned<double> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned_double (p);
}


template <>
inline
int8_t get_unaligned<int8_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned<uint8_t> (p);
}


template <>
inline
int16_t get_unaligned<int16_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned<uint16_t> (p);
}


template <>
inline
int32_t get_unaligned<int32_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned<uint32_t> (p);
}


template <>
inline
int64_t get_unaligned<int64_t> (const uint8_t* ATH_RESTRICT &  p)
{
  return get_unaligned<uint64_t> (p);
}


} // namespace CxxUtils


#endif // not CXXUTILS_GET_UNALIGNED_H

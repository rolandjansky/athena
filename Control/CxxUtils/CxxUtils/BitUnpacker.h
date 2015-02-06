// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/BitUnpacker.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Helper to unpack a set of values bitwise from a stream.
 */


#ifndef CXXUTILS_BITUNPACKER_H
#define CXXUTILS_BITUNPACKER_H


#include <stdint.h>
#include <cassert>


namespace CxxUtils {


/**
 * @brief Helper to unpack a set of values bitwise from a stream.
 *
 * This helper can be used to unpack a set of values from a stream
 * and return them as a set of 32-bit words.  The number of bits taken by each
 * value is configurable.
 *
 * @c STREAM should support input (operator>>) with @c uint32_t
 * (satisfied by a ROOT TBuffer).
 */
template <class STREAM>
class BitUnpacker
{
public:
  /**
   * @brief Constructor.
   * @param nbits Number of bits per item to use in the packed representation.
   * @param stream Input stream object.
   */
  BitUnpacker (uint8_t nbits, STREAM& stream);


  /**
   * @brief Unpack one value from the stream.
   */
  uint32_t unpack();


private:
  /// Buffer for the current word being unpacked.
  uint32_t m_buf;

  /// Number of valid bits currently buffered.
  uint8_t m_nbuf;

  /// Number of bits to use for each item.
  uint8_t m_nbits;

  /// The input stream.
  STREAM& m_stream;

  /// Mask with the low @c m_nbits bits set.
  uint32_t m_mask;
};


/**
 * @brief Helper to unpack a set of values bitwise from a stream.
 *
 * This helper can be used to unpack a set of values from a stream
 * and return them as a set of 32-bit words.  This is a special case
 * for 8-bit values.
 *
 * @c STREAM should support input (operator>>) with @c uint32_t
 * (satisfied by a ROOT TBuffer).
 */
template <class STREAM>
class BitUnpacker8
{
public:
  /**
   * @brief Constructor.
   * @param stream Input stream object.
   */
  BitUnpacker8 (STREAM& stream);


  /**
   * @brief Constructor.
   * @param nbits Must be 8.
   * @param stream Input stream object.
   */
  BitUnpacker8 (uint8_t nbits, STREAM& stream);


  /**
   * @brief Unpack one value from the stream.
   */
  uint32_t unpack();


private:
  /// Buffer for the current word being unpacked.
  uint32_t m_buf;

  /// Number of valid bytes currently buffered.
  uint8_t m_nbuf;

  /// The input stream.
  STREAM& m_stream;
};


/**
 * @brief Helper to unpack a set of values bitwise from a stream.
 *
 * This helper can be used to unpack a set of values from a stream
 * and return them as a set of 32-bit words.  This is a special case
 * for 16-bit values.
 *
 * @c STREAM should support input (operator>>) with @c uint32_t
 * (satisfied by a ROOT TBuffer).
 */
template <class STREAM>
class BitUnpacker16
{
public:
  /**
   * @brief Constructor.
   * @param stream Input stream object.
   */
  BitUnpacker16 (STREAM& stream);


  /**
   * @brief Constructor.
   * @param nbits Must be 16.
   * @param stream Input stream object.
   */
  BitUnpacker16 (uint8_t nbits, STREAM& stream);


  /**
   * @brief Unpack one value from the stream.
   */
  uint32_t unpack();


private:
  /// Buffer for the current word being unpacked.
  uint32_t m_buf;

  /// Number of valid words currently buffered.
  uint8_t m_nbuf;

  /// The input stream.
  STREAM& m_stream;
};


} // namespace CxxUtils


#include "CxxUtils/BitUnpacker.icc"


#endif // not CXXUTILS_BITUNPACKER_H

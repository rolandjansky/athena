// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/BitPacker.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Helper to pack a set of values bitwise into a stream.
 */


#ifndef CXXUTILS_BITPACKER_H
#define CXXUTILS_BITPACKER_H


#include <stdint.h>
#include <cassert>


namespace CxxUtils {


/**
 * @brief Pack a set of values bitwise into a stream.
 *
 * This helper can be used to pack a set of values into 32-bit words
 * and write them to a stream.  The number of bits taken by each
 * value is configurable.
 *
 * @c STREAM should support output (operator<<) with @c uint32_t
 * (satisfied by a ROOT TBuffer).
 */
template <class STREAM>
class BitPacker
{
public:
  /**
   * @brief Constructor.
   * @param nbits Number of bits per item to use in the packed representation.
   * @param stream Output stream object.
   */
  BitPacker (uint8_t nbits, STREAM& stream);


  /**
   * @brief Destructor.
   *
   * This may flush buffered data to the output stream.
   */
  ~BitPacker();


  /**
   * @brief Pack one value to the stream.
   * @param x The value to pack.
   *          The upper bits should all be clear.
   */
  void pack (uint32_t x);


private:
  /// Buffer for the current words being packed.
  uint32_t m_buf;

  /// Number of valid bits currently buffered.
  uint8_t m_nbuf;

  /// Number of bits to use for each item.
  uint8_t m_nbits;

  /// The output stream.
  STREAM& m_stream;
};


/**
 * @brief Pack a set of values bitwise into a stream.
 *
 * This helper can be used to pack a set of values into 32-bit words
 * and write them to a stream.  This is a special case for 8-bit values.
 *
 * @c STREAM should support output (operator<<) with @c uint32_t
 * (satisfied by a ROOT TBuffer).
 */
template <class STREAM>
class BitPacker8
{
public:
  /**
   * @brief Constructor.
   * @param stream Output stream object.
   */
  BitPacker8 (STREAM& stream);


  /**
   * @brief Constructor.
   * @param nbits Must be 8.
   * @param stream Output stream object.
   */
  BitPacker8 (uint8_t nbits, STREAM& stream);


  /**
   * @brief Destructor.
   *
   * This may flush buffered data to the output stream.
   */
  ~BitPacker8();


  /**
   * @brief Pack one value to the stream.
   * @param x The value to pack.
   *          The upper bits should all be clear.
   */
  void pack (uint32_t x);


private:
  /// Buffer for the current words being packed.
  uint32_t m_buf;

  /// Number of valid bits currently buffered.
  uint8_t m_nbuf;

  /// The output stream.
  STREAM& m_stream;
};


/**
 * @brief Pack a set of values bitwise into a stream.
 *
 * This helper can be used to pack a set of values into 32-bit words
 * and write them to a stream.  This is a special case for 16-bit values.
 *
 * @c STREAM should support output (operator<<) with @c uint32_t
 * (satisfied by a ROOT TBuffer).
 */
template <class STREAM>
class BitPacker16
{
public:
  /**
   * @brief Constructor.
   * @param nbits Number of bits per item to use in the packed representation.
   * @param stream Output stream object.
   */
  BitPacker16 (STREAM& stream);


  /**
   * @brief Constructor.
   * @param nbits Must be 16.
   * @param stream Output stream object.
   */
  BitPacker16 (uint8_t nbits, STREAM& stream);


  /**
   * @brief Destructor.
   *
   * This may flush buffered data to the output stream.
   */
  ~BitPacker16();


  /**
   * @brief Pack one value to the stream.
   * @param x The value to pack.
   *          The upper bits should all be clear.
   */
  void pack (uint32_t x);


private:
  /// Buffer for the current words being packed.
  uint32_t m_buf;

  /// Number of valid words currently buffered.
  uint8_t m_nbuf;

  /// The output stream.
  STREAM& m_stream;
};


} // namespace CxxUtils


#include "CxxUtils/BitPacker.icc"


#endif // not CXXUTILS_BITPACKER_H


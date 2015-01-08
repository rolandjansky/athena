// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/PackedParameters.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Describe how the contents of a @c PackedContainer are to be saved.
 */


#ifndef ATHCONTAINERS_PACKEDPARAMETERS_H
#define ATHCONTAINERS_PACKEDPARAMETERS_H


#include "AthContainersInterfaces/AuxDataOption.h"
#include <limits>
#include <stdint.h>


namespace SG {


/** 
 * @brief Describe how the contents of a @c PackedContainer are to be saved.
 * 
 * When a @c PackedContainer is saved to a root file, the contents may
 * be stored with reduced precision.  How this packing is done is described
 * by the parameters held in this structure.  These include:
 *
 * - nbits: This is the overall number of bits used to hold one container
 *          element.  May be in the range 1-32, inclusive.
 * - isSigned: If true, then the elements are saved as signed numbers;
 *             otherwise as unsigned numbers.  If this is set, then one
 *             bit is used for the sign; nbits must therefore be at least 2.
 * - isFloat: If true, the elements are saved as floating-point numbers;
 *            otherwise as integers.  It may occasionally be useful to
 *            save floating-point values as integers.  The converse
 *            should also work, but is likely to be less useful.
 *
 * The remaining parameters are only used when elements are being
 * saved as floating-point.
 *
 * - nmantissa: The number of bits to be used for the mantissa part
 *              of the representation.  Does not include a sign bit.
 *              Any bits left out of nbits after removing nmantissa
 *              bits and (optionally) the sign bit are used as the exponent.
 *              If fewer than 2 bits are available for the exponent,
 *              then the number is saved using a fixed-point representation,
 *              with a range [0, 1).
 * - scale: If set, then numbers will be divided by this value before
 *          being saved.  This allows extending a fixed-point format
 *          to an arbitrary range.
 * - rounding: If true, numbers will be rounded to the nearest value
 *             when written.  Otherwise (the default), they will
 *             be truncated.
 *
 * User code will should usually not interact with this class directly.
 * Use the @c setOption interfaces instead.
 */
class PackedParameters
{
public:
  /**
   * @brief Default constructor.
   *
   * Set up to write unsigned integers with 32 bits.
   */
  PackedParameters();


  /**
   * @brief Constructor from nbits and flags.
   * @param nbits The number of bits for each element.
   * @param flags Extra flags describing the packing.
   *
   * Additionally nmantissa is set to correspond to a fixed-point
   * representation and the scale is cleared.
   *
   * This is meant to be used by the read converter.
   */
  PackedParameters (uint8_t nbits, uint8_t flags);


  /**
   * @brief Initialize with default packing parameters for type @c T.
   *
   * For integer types, the number of bits and signedness is set to match @c T.
   * For floating types, nbits is set to 32, nmantissa to 23, isSigned to true,
   * and the scale is cleared.
   */
  template <class T>
  PackedParameters(T);


  /**
   * @brief The number of bits used to store each element.
   */
  uint8_t nbits() const;

 
  /**
   * @brief The number of bits used for the mantissa portion of a float-point
   *        representation, excluding a sign bit (if any).
   *
   *        If there are at least two bits left over after accounting
   *        for the mantissa and sign bits, then numbers will be saved
   *        in a floating-point format; otherwise, fixed-point.
   */
  uint8_t nmantissa() const;


  /**
   * @brief Additional flags describing the packing.
   *
   * (This should really only be used by converters.)
   */
  uint8_t flags() const;


  /**
   * @brief Return the scale for floating-point numbers.
   *
   * If enabled, float-point numbers will be divided by this value
   * before being saved.  If not enabled, this may return 0.
   */
  float scale() const;


  /**
   * @brief Are elements being written as signed numbers?
   */
  bool isSigned() const;


  /**
   * @brief Are elements being written as floating-point numbers?
   */
  bool isFloat()  const;


  /**
   * @brief Should floats be rescaled before writing?
   */
  bool hasScale() const;


  /**
   * @brief Should floats be rounded during writing?
   */
  bool rounding() const;


  /**
   * @brief Set the number of bits to be used for each element.
   * @brief nbits The desired number of bits.
   *
   * @c nbits must be in the range 1-32 for unsigned numbers,
   * 2-32 for signed numbers.
   *
   * If necessary, @c nmantissa will be adjusted so that it still fits
   * within the requested number of bits.
   *
   * Returns true if successful, false otherwise.
   */
  bool setNbits (uint8_t nbits);


  /**
   * @brief Set the number of mantissa bits used in the packed representation.
   * @brief nmantissa The desired number of mantissa bits.
   *
   * This has an effect only when saving floating-point types.
   *
   * @c nmantissa must fit within the requested number of bits.
   * @c nmantissa does not include the sign bit.
   * If there are at least two bits left over, then numbers will be
   * saved in a floating-point format; otherwise, fixed point will be used.
   *
   * Returns true if successful, false otherwise.
   */
  bool setNmantissa (uint8_t nmantissa);


  /**
   * @brief Set the scale to use when packing floating-point data.
   * @param scale The new scale, or 0 to disable.
   *
   * This has an effect only when saving floating-point types.
   *
   * If set to something non-zero, then floating-point numbers
   * will be divided by this value before being written.
   *
   * Returns true to indicate success.
   */
  bool setScale (float scale);


  /**
   * @brief Set the signedness flag for the packed representation.
   * @param flag Should elements be saved as signed numbers?
   *
   * If the flag is false (unsigned), this always succeeds.
   * If the flag is true (signed), then this fails if @c nbits is 1.
   * @c nmantissa will be adjusted if there are now insufficient
   * bits for it.
   *
   * Returns true if successful, false otherwise.
   */
  bool setSigned (bool flag);


  /**
   * @brief Set the rounding mode.
   * @param flag Should numbers be rounded when being written?
   *
   * This has an effect only when saving floating-point types.
   *
   * Returns true to indicate success.
   */
  bool setRounding (bool flag);


  /**
   * @brief Set the floating-point flag.
   * @param flag Should numbers be written as floating-point?
   *
   * If true, numbers will be written in a floating/fixed point
   * representation; otherwise, they will be written as integers.
   *
   * Returns true to indicate success.
   */
  bool setFloat (bool flag);


  /**
   * @brief Test to see if @c option is a recognized packing option.
   * @param option The option to test.
   *
   * Returns @c true if the name of @c option is recognized as a valid
   * packing option; @c false otherwise.
   */
  static bool isValidOption (const AuxDataOption& option);


  /**
   * @brief Set a packing option.
   * @parameter option The option to set.
   *
   * Recognized options are `nbits', `nmantissa', `scale', `signed',
   * `rounding', and `float'.  See the setter functions above for details
   * on their semantics.
   *
   * Returns true on success; false otherwise.
   */
  bool setOption (const AuxDataOption& option);


  /// Define bit assignments for the flags field.
  /// Existing flag assignments should never be changed, as the persistent
  /// format may depend on them.
  enum {
    FLAG_IS_SIGNED = (1<<0),
    FLAG_IS_FLOAT  = (1<<1),
    FLAG_HAS_SCALE = (1<<2),
    FLAG_ROUNDING  = (1<<3),
  };


private:
  /// The number of bits to use for each element.
  uint8_t m_nbits;

  /// The number of bits for the mantissa of floating-point representations.
  /// Does not include the sign bit.
  uint8_t m_nmantissa;

  /// If nonzero, divide floating-point numbers by this before writing.
  float   m_scale;

  /// Additional flags.
  uint8_t m_flags;
};


} // namespace SG


#include "AthContainers/PackedParameters.icc"


#endif // not ATHCONTAINERS_PACKEDPARAMETERS_H


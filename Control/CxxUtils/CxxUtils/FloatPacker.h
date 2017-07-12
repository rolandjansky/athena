// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/FloatPacker.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009, from earlier code.
 * @brief Pack/unpack floating-point data from/to a given number of bits.
 */


#ifndef CXXUTILS_FLOATPACKER_H
#define CXXUTILS_FLOATPACKER_H


#include <string>
#include <stdint.h>



namespace CxxUtils {


/**
 * @brief Pack/unpack floating-point data from/to a given number of bits.
 *
 * The format is specified by the following parameters.
 *
 *   nbits - The total number of bits in the representation.
 *   scale - Scale factor to apply before storing.
 *   nmantissa - The number of bits to use for the mantissa and sign bit.
 *   is_signed - Flag to tell if we should use a sign bit.
 *   round - Flag to tell if we should round or truncate.
 *
 * From these we derive:
 *
 *   npack = nmantissa, if is_signed is false.
 *         = nmantissa-1 if is_signed is true.
 *   nexp = nbits - nmantissa
 *
 * The format consists of, in order from high bits to low bits:
 *   - A sign bit, if is_signed is true.
 *   - nexp bits of exponent information.
 *   - npack bits of mantissa.
 *
 * The number is stored in normalized form, with an exponent bias
 * of 2^(nexp-1).  But if the (biased) exponent is zero, then the
 * mantissa is stored in denormalized form.  If nexp==0, this
 * gives a fixed-point representation in the range [0,1).
 * 0 is represented by all bits 0; if we have a sign bit, we can also
 * represent -0 by all bits 0 except for the sign bit.
 */
class FloatPacker
{
public:
  /// Type into which we pack.
  typedef uint32_t Packdest;


  /**
   * @brief Constructor.
   * @param nbits The number of bits in the packed representation.
   * @param nmantissa The number of bits to use for the mantissa
   *                  and sign bit.
   * @param scale Divide the input number by this before packing.
   * @param is_signed If true, then one mantissa bit is used for a sign.
   * @param round If true, numbers will be rounded.
   *              Otherwise, they will be truncated.
   */
  FloatPacker (int nbits,
               int nmantissa,
               double scale = 1,
               bool is_signed = true,
               bool round = false);


  /**
   * @brief Pack a value.
   * @param src Value to pack.
   * @param err If non-null, then this string will be set to a description
   *            of any error that occurs.
   * @return The packed value.
   *
   * For now, we convert floats to doubles before packing.
   */
  Packdest pack (double src, std::string* err = nullptr) const;


  /**
   * @brief Unpack the value @c VAL.
   * @param val The packed data.  It should start with the low bit,
   *            and any extraneous bits should have been masked off.
   * @param err If non-null, then this string will be set to a description
   *            of any error that occurs.
   */
  double unpack (Packdest val, std::string* err = nullptr) const;


private:
  /// Number of bits in the mantissa + sign bit.
  int m_nmantissa;

  /// Scale factor for stored numbers.
  double m_scale;

  /// Inverse of scale.
  double m_invscale;

  /// Should we use a sign bit?
  bool m_is_signed;

  /// Should we round instead of truncating?
  bool m_round;

  /// Number of bits in mantissa (exclusive of any sign bit).
  int m_npack;

  /// Mask with that many low bits set.
  Packdest m_npack_ones;

  /// Mask containing the sign bit (or 0 if there's no sign bit).
  Packdest m_signmask;

  /// Number of exponent bits.
  int m_nexp;

  /// Mask with that many low bits set.
  Packdest m_nexp_ones;

  /// Minimum exponent value.
  int m_min_exp;

  /// Maximum exponent value.
  int m_max_exp;
};


} // namespace CxxUtils


#endif // not CXXUTILS_FLOATPACKER_H

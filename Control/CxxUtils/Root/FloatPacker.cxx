/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/src/FloatPacker.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009, from earlier code.
 * @brief Pack/unpack floating-point data from/to a given number of bits.
 */

#include "CxxUtils/FloatPacker.h"
#include "CxxUtils/ones.h"
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#ifndef __APPLE__
#include <ieee754.h>  // ??? Is this standardized?
#else
//ieee754.h doesn't exist on MacOSX
union ieee754_double
{
  long double d;
  struct {
    unsigned int negative:1;
    unsigned int exponent:11;
    /* Together these comprise the mantissa.  */
    unsigned int mantissa0:20;
    unsigned int mantissa1:32;
  } ieee;
  struct {
    unsigned int negative:1;
    unsigned int exponent:11;
    // cppcheck-suppress unusedStructMember
    unsigned int quiet_nan:1;
    /* Together these comprise the mantissa.  */
    unsigned int mantissa0:19;
    unsigned int mantissa1:32;
  } ieee_nan;
};

#define IEEE754_DOUBLE_BIAS  0x3ff /* Added to exponent.  */

#endif
#include <stdint.h>


namespace {



/**
 * @brief We sometimes need to be able to access a double
 *        as ints.
 */
union double_or_int {
  ieee754_double d;
  uint32_t i[2];
};


const int ieee754_double_bias = 0x3ff;
const int ieee754_double_exponent_bits = 11;
const int ieee754_double_mantissa0_bits = 20;
const int ieee754_double_mantissa1_bits = 32;


/// Abbreviation.
typedef CxxUtils::FloatPacker::Packdest Packdest;

/// Size of @c Packdest in bits.
const int packdest_bits = std::numeric_limits<Packdest>::digits;


// Handy constant: A Packdest with 1 in the high bit.
const Packdest high_packdest_bit = (1U << (packdest_bits - 1));

const Packdest ieee754_double_exponent_mask =
  (1U << ieee754_double_exponent_bits) - 1;


/**
 * @brief  Return the largest (signed) integer that can be represented
 *         With @c nbits bits.
 *         Boundary case: nbits=0 should return -1.
 * @param nbits Number of bits.
 */
inline
int max_int (int nbits)
{
  return ((1U << nbits) >> 1) - 1;
}


/**
 * @brief  Return the smallest (signed) integer that can be represented
 *         With @c nbits bits.
 *         Boundary case: nbits=0 should return -1.
 * @param nbits Number of bits.
 */
inline
int min_int (int nbits)
{
  return static_cast<int>(~0U << nbits) >> 1;
}


/**
 * @brief Renormalize a denormal number.
 * @param exponent[inout] The exponent of the number.
 *                        Should initially be the denormal flag value.
 *                        Will be modified in place.
 * @param mantissa[inout] The mantissa of the number.
 *                        Will be modified in place.
 */
void renormalize_denormal (int& exponent,
                           Packdest& mantissa)
{
  if (mantissa == 0)
    exponent -= packdest_bits; // Lost precision here.
  else {
    while ((mantissa & high_packdest_bit) == 0) {
      --exponent;
      mantissa <<= 1;
    }
    mantissa <<= 1;
  }
}


/**
 * @brief If the number given by @a exponent and @a mantissa is too small
 *           to be represented by a normalized number in a representation
 *           where @a min_exp is the exponent value flagging denormals, convert
 *           it to a denormal representation.
 *
 * @param min_exp The denormal marker exponent value.
 * @param round_bits Number of bits to consider for rounding.
 * @param exponent[inout] The exponent of the number.
 *                        Will be modified in place.
 * param mantissa[inout] The mantissa of the number.
 *                       Will be modified in place.
 */
void underflow_to_denormal (int min_exp,
                            int round_bits,
                            int& exponent,
                            Packdest& mantissa)
{
  if (exponent <= min_exp) {
    Packdest mantissa_in = mantissa;

    // Denormalize the mantissa.
    mantissa = (mantissa >> 1) | high_packdest_bit;

    // Now shift it right.
    int shift = min_exp - exponent;
    if (shift < packdest_bits)
      mantissa >>= shift;
    else
      mantissa = 0; // underflow to 0.

    // Flag it as denormal.
    exponent = min_exp;

    // Handle rounding, if desired.
    if (round_bits) {
      //
      //                                   +- packdest_bits - round_bits - 1
      //                  |- round_bits  -|v
      //    mantissa:     .................X....
      //                       v- shift+1 -^
      //    mantissa_in:  .....X................
      //                       ^
      //                       +- packdest_bits - round_bits + shift
      //
      int orig_pos = packdest_bits - round_bits + shift;
      if (orig_pos < packdest_bits &&
          ((static_cast<Packdest> (1) << orig_pos) & mantissa_in) != 0)
      {
        Packdest lsb = (static_cast<Packdest> (1) <<
                        (packdest_bits - round_bits));
        Packdest lsbmask = ~ (lsb - 1);

        // ??? If we overflow here, it means we have to go back
        //     to a normalized representation.  Just punt for now.
        if ((mantissa & lsbmask) != lsbmask)
          mantissa += lsb;
      }
    }
  }
}


} // unnamed namespace


namespace CxxUtils {


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
FloatPacker::FloatPacker (int nbits,
                          int nmantissa,
                          double scale /*= 1*/,
                          bool is_signed /*= true*/,
                          bool round /*= false*/)
  : m_nmantissa (nmantissa),
    m_scale (scale),
    m_is_signed (is_signed),
    m_round (round)
{
  // scale==0 means not to scale.
  // Use that instead of 1 since it's faster to test for 0.
  if (scale == 1)
    scale = 0;

  if (scale == 0)
    m_invscale = 0;
  else
    m_invscale = 1. / m_scale;

  // Set up other cached values.
  m_npack = m_nmantissa;
  if (m_is_signed)
    --m_npack;

  m_npack_ones = ones<Packdest> (m_npack);

  // Sign bit mask.
  if (m_is_signed)
    m_signmask = 1U << (nbits - 1);
  else
    m_signmask = 0;

  // Number of exponent bits.
  m_nexp = nbits - m_nmantissa;
  m_nexp_ones = ones<Packdest> (m_nexp);

  // Minimum exponent value.
  m_min_exp = min_int (m_nexp);

  // Maximum exponent value.
  m_max_exp = max_int (m_nexp);

  if (m_npack < 1 || m_npack > nbits)
    throw std::runtime_error ("Bad number of mantissa bits.");
}


/**
 * @brief Pack a value.
 * @param src Value to pack.
 * @param err If non-null, then this string will be set to a description
 *            of any error that occurs.
 * @return The packed value.
 *
 * For now, we convert floats to doubles before packing.
 */
FloatPacker::Packdest
FloatPacker::pack (double src, std::string* err /*= nullptr*/) const
{
  double_or_int d;
  d.d.d = src;

  // Fast-path for zero.  (Purely an optimization.)
  // Note: can't use a double compare here.  On some architectures (eg, MIPS)
  // a denormal will compare equal to zero.
  if (d.i[0] == 0 && d.i[1] == 0)
    return 0;

  // Check for NaN and infinity.
  if (d.d.ieee.exponent == ieee754_double_exponent_mask) {
    if (err) {
      std::ostringstream os;
      os << "Bad float number: " << src << " (" << std::setbase(16) << d.i[0]
         << " " << d.i[1] << ")";
      *err = os.str();
    }
    d.d.d = 0;
  }

  if (m_invscale)
    d.d.d *= m_invscale;

  bool was_negative = false;
  if (d.d.ieee.negative != 0) {
    if (m_is_signed) {
      was_negative = true;
      d.d.d = -d.d.d;
    }
    else {
      // Don't complain on -0.
      if (d.d.d < 0 && err) {
        std::ostringstream os;
        os << "Float overflow during packing: " << src;
        *err = os.str();
      }
      d.d.d = 0;
    }
  }

  // Check for zero again.
  // (Also need to preserve the sign; the scale division may
  // have underflowed.)
  if (d.i[0] == 0 && d.i[1] == 0) {
    if (was_negative)
      return m_signmask;
    else
      return 0;
  }

  // Get packdest_bits bits of mantissa.

  Packdest mantissa =
    (d.d.ieee.mantissa0 << (packdest_bits -
                          ieee754_double_mantissa0_bits)) |
    (d.d.ieee.mantissa1 >>
     (ieee754_double_mantissa1_bits -
      (packdest_bits - ieee754_double_mantissa0_bits)));

  // Get the unbiased exponent.
  int exponent =
    static_cast<int> (d.d.ieee.exponent) - ieee754_double_bias;

  // Do rounding, if requested.
  if (m_round) {
    Packdest lsbmask = (1 << (packdest_bits - m_npack));
    int roundbit;
    Packdest roundmask;
    if (lsbmask > 1) {
      roundbit = (mantissa & (lsbmask >> 1));
      roundmask = ~ static_cast<Packdest> (roundbit - 1);
    }
    else {
      roundbit = (d.d.ieee.mantissa1 &
                  ((1 << ((ieee754_double_mantissa1_bits -
                           (packdest_bits -
                            ieee754_double_mantissa0_bits)) - 1))));
      roundmask = ~ static_cast<Packdest> (0);
    }

    if (roundbit != 0) {
      // Handle the case where it would overflow.
      if ((mantissa & roundmask) == roundmask) {
        mantissa >>= 1;
        mantissa |= roundmask;
        exponent += 1;
      }

      mantissa += lsbmask;
    }
  }

  // If the number is too large, bitch, and reset to the largest number.
  if (exponent > m_max_exp) {
    if (err) {
      std::ostringstream os;
      os << "Float overflow during packing: " << src;
      *err = os.str();
    }
    exponent = m_max_exp;
    mantissa = static_cast<Packdest> (~0);
  }

  // Handle denormals.  (We've already handled the zero case.)
  if (exponent == - ieee754_double_bias)
    renormalize_denormal (exponent, mantissa);

  // If the number is too small, denormalize, or underflow to 0.
  underflow_to_denormal (m_min_exp, m_round ? m_npack: 0, exponent, mantissa);

  // Pack in the mantissa bits.
  Packdest dest = mantissa >> (packdest_bits - m_npack);

  // The exponent, if desired.
  if (m_nexp > 0)
    dest |= ((exponent - m_min_exp) << m_npack);

  // And the optional sign bit.
  if (was_negative)
    dest |= m_signmask;

  return dest;
}


/**
 * @brief Unpack the value @c VAL.
 * @param val The packed data.  It should start with the low bit,
 *            and any extraneous bits should have been masked off.
 * @param err If non-null, then this string will be set to a description
 *            of any error that occurs.
 */
double FloatPacker::unpack (Packdest val, std::string* err /*= nullptr*/) const
{
  // Fast-path for 0.
  if (val == 0)
    return 0;

  // Break apart the packed value.
  bool was_negative = false;
  if ((val & m_signmask) != 0)
    was_negative = true;

  double d;

  // Fast path for fixed-point representations.
  if (m_nexp == 0) {
    Packdest mantissa = (val & m_npack_ones);
    d = mantissa / ((double)m_npack_ones + 1);
    if (was_negative)
      d *= -1;
  }
  else {
    // Get the mantissa.
    Packdest mantissa = (val & m_npack_ones) << (packdest_bits - m_npack);

    // General case.
    // Get the exponent.
    int exponent = ((val >> m_npack) & m_nexp_ones);
    exponent += m_min_exp; // unbias.

    ieee754_double dd;

    // Handle denormals.
    if (exponent == m_min_exp) {
      // Maybe it was -0?
      if (mantissa == 0) {
        dd.d = 0;
        if (was_negative)
          dd.ieee.negative = 1;
        return dd.d;
      }

      renormalize_denormal (exponent, mantissa);
    }

    // Complain about overflow.
    if (exponent >= max_int (ieee754_double_exponent_bits)) {
      if (err) {
        std::ostringstream os;
        os << "Overflow while unpacking float; exponent: " << exponent;
        *err = os.str();
      }
      exponent = max_int (ieee754_double_exponent_bits) + 1;
      mantissa = 0; // Infinity.
    }

    // Underflow into denormal.
    underflow_to_denormal ( - ieee754_double_bias, 0,
                            exponent, mantissa);

    // Pack into a double.
    dd.ieee.negative = was_negative ? 1 : 0;
    dd.ieee.exponent = exponent + ieee754_double_bias;
    dd.ieee.mantissa0 =
      (mantissa >> (packdest_bits - ieee754_double_mantissa0_bits));
    dd.ieee.mantissa1 =
      (mantissa << (ieee754_double_mantissa0_bits -
                    (packdest_bits - ieee754_double_mantissa1_bits)));
    d = dd.d;
  }

  // Set the result.
  if (m_scale)
    d *= m_scale;
  return d;
}


} // namespace CxxUtils

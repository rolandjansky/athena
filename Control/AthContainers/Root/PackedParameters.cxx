/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/PackedParameters.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Describe how the contents of a @c PackedContainer are to be saved.
 */


#include "AthContainers/PackedParameters.h"


namespace SG {


/**
 * @brief Default constructor.
 *
 * Set up to write unsigned integers with 32 bits.
 */
PackedParameters::PackedParameters()
  : m_nbits(8 * sizeof(uint32_t)),
    m_nmantissa(23),
    m_scale(0),
    m_flags(0)
{
}


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
PackedParameters::PackedParameters (uint8_t nbits, uint8_t flags)
  : m_nbits(nbits),
    m_flags(flags)
{
  m_nmantissa = nbits;
  if (isSigned() && m_nmantissa > 0) --m_nmantissa;
  if (hasScale())
    m_scale = 1;
  else
    m_scale = 0;
}


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
bool PackedParameters::setNbits (uint8_t nbits)
{
  if (nbits <= 0 || nbits > 32) return false;
  uint8_t navail = nbits;
  if (isSigned()) --navail;
  if (navail == 0) return false;
  m_nbits = nbits;
  if (m_nmantissa > navail)
    m_nmantissa = navail;
  return true;
}


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
bool PackedParameters::setNmantissa (uint8_t nmantissa)
{
  uint8_t navail = m_nbits;
  if (isSigned()) --navail;
  if (nmantissa <= 0 || nmantissa > navail) return false;
  m_nmantissa = nmantissa;
  return true;
}


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
bool PackedParameters::setScale (float scale)
{
  m_scale = scale;
  if (m_scale == 0 || m_scale == 1)
    m_flags &= ~FLAG_HAS_SCALE;
  else
    m_flags |= FLAG_HAS_SCALE;
  return true;
}


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
bool PackedParameters::setSigned (bool flag)
{
  if (!flag) {
    m_flags &= ~FLAG_IS_SIGNED;
    return true;
  }

  if (m_nbits == 1) return false;
  if (m_nmantissa + 1 > m_nbits)
    m_nmantissa = m_nbits - 1;
  m_flags |= FLAG_IS_SIGNED;
  return true;
}


/**
 * @brief Set the rounding mode.
 * @param flag Should numbers be rounded when being written?
 *
 * This has an effect only when saving floating-point types.
 *
 * Returns true to indicate success.
 */
bool PackedParameters::setRounding (bool flag)
{
  if (flag)
    m_flags |= FLAG_ROUNDING;
  else
    m_flags &= ~FLAG_ROUNDING;
  return true;
}


/**
 * @brief Set the floating-point flag.
 * @param flag Should numbers be written as floating-point?
 *
 * If true, numbers will be written in a floating/fixed point
 * representation; otherwise, they will be written as integers.
 *
 * Returns true to indicate success.
 */
bool PackedParameters::setFloat (bool flag)
{
  if (flag)
    m_flags |= FLAG_IS_FLOAT;
  else
    m_flags &= ~FLAG_IS_FLOAT;
  return true;
}


/**
 * @brief Test to see if @c option is a recognized packing option.
 * @param option The option to test.
 *
 * Returns @c true if the name of @c option is recognized as a valid
 * packing option; @c false otherwise.
 */
bool PackedParameters::isValidOption (const AuxDataOption& option)
{
  if (option.name() == "nbits" ||
      option.name() == "nmantissa" ||
      option.name() == "scale" ||
      option.name() == "signed" ||
      option.name() == "rounding" ||
      option.name() == "float")
  {
    return true;
  }
  return false;
}


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
bool PackedParameters::setOption (const AuxDataOption& option)
{
  if (option.name() == "nbits")
    return setNbits (option.intVal());

  else if (option.name() == "nmantissa")
    return setNmantissa (option.intVal());

  else if (option.name() == "scale")
    return setScale (option.floatVal());

  else if (option.name() == "signed")
    return setSigned (option.intVal() != 0);
  
  else if (option.name() == "rounding")
    return setRounding (option.intVal() != 0);
  
  else if (option.name() == "float")
    return setFloat (option.intVal() != 0);
  
  return false;
}


} // namespace SG

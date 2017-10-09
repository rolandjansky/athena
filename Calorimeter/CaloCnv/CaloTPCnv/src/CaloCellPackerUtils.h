// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellPackerUtils.h,v 1.1 2007-11-08 18:14:20 ssnyder Exp $

/**
 * @file CaloCellPackerUtils.h
 * @author scott snyder
 * @date Nov 2007
 * @brief Utilities for compacting calorimeter cells.
 */


#ifndef CALOCELLPACKERUTILS_H
#define CALOCELLPACKERUTILS_H


#include "GaudiKernel/Kernel.h"  // For LIKELY


namespace CaloCellPackerUtils {


/**
 * @brief Helper for packing into/out of a bit field.
 *
 * The bitfield is identified by a bitmask, which should
 * contain a contiguous string of 1's.  The @c in function
 * will then take a value and shift and mask it appropriately
 * for the bitfield.  The @c out function will extract a value
 * from the bitfield.
 */
class Bitfield
{
public:
  /**
   * @brief Constructor.
   * @param The bit mask.  Should contain a single contiguous string of 1's.
   */
  Bitfield (unsigned int mask = 1);


  /**
   * @brief Shift and mask a value into the bitfield.
   * @param x The input value.
   * @return The value shifted and masked to go into the bitfield.
   */
  unsigned int in (unsigned int x) const;


  /**
   * @brief Extract a value from the bitfield.
   * @param x The input bitfield.
   * @return The value extracted from the bitfield.
   */
  unsigned int out (unsigned int x) const;


protected:
  /// Bitmask.  Lower bit should be 1.
  unsigned int m_mask;

  /// Shift count.
  unsigned int m_shift;
};


/**
 * @brief Helper for packing a float into/out of a bit field.
 *
 * This works like the bitfield class above, except that the
 * @c in/@c out methods take floating-point values.
 * The field takes a min/max range of values; the input value is converted
 * to a fraction within the range, and that fraction is stored.
 */
class Floatfield
  : public Bitfield
{
public:
  /**
   * @brief Constructor.
   * @param The bit mask.  Should contain a single contiguous string of 1's.
   * @param xmin The smallest storable value.
   * @param xmax The largest storable value.
   */
  Floatfield (unsigned int mask=1, double xmin=1, double xmax=0);


  /**
   * @brief Shift and mask a value into the bitfield.
   * @param x The input value.
   * @return The converted value shifted and masked to go into the bitfield.
   */
  unsigned int in (double x) const;


  /**
   * @brief Extract a value from the bitfield.
   * @param x The input bitfield.
   * @param underflag[out] Set to 1 if the value was the lowest possible.
   * @return The value extracted from the bitfield.
   */
  double out (unsigned int x, int& underflow) const;


private:
  /// Smallest representable value.
  double m_xmin;

  /// Largest representable value.
  double m_xmax;

  /// Cached factor for int->float conversions.
  double m_fact;

  /// Cached factor for float->int conversions.
  double m_ifact;
};


/**
 * @brief Helper for packing a float into/out of a bit field, with
 *        a minimum of 0.
 *
 * This is just like @c Floatfield, specialized for the case where @c xmin==0.
 */
class Floatfield2
  : public Bitfield
{
public:
  /**
   * @brief Constructor.
   * @param The bit mask.  Should contain a single contiguous string of 1's.
   * @param xmax The largest storable value.
   */
  Floatfield2 (unsigned int mask=1, double xmax=1);


  /**
   * @brief Shift and mask a value into the bitfield.
   * @param x The input value.
   * @return The converted value shifted and masked to go into the bitfield.
   */
  unsigned int in (double x) const;


  /**
   * @brief Extract a value from the bitfield.
   * @param x The input bitfield.
   * @return The value extracted from the bitfield.
   */
  double out (unsigned int x) const;


private:
  /// Largest representable value.
  double m_xmax;

  /// Cached factor for int->float conversions.
  double m_fact;

  /// Cached factor for float->int conversions.
  double m_ifact;
};


} // namespace CaloCellPackerUtils


#include "CaloCellPackerUtils.icc"


#endif // not CALOCELLPACKERUTILS_H

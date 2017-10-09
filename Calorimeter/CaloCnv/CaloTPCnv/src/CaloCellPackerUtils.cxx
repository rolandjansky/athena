/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellPackerUtils.cxx,v 1.1 2007-11-08 18:14:22 ssnyder Exp $
/**
 * @file CaloTools/src/CaloCellPackerUtils.cxx
 * @author scott snyder
 * @date Nov 2007
 * @brief Utilities for compacting calorimeter cells.
 */


#include "CaloCellPackerUtils.h"
#include <cassert>


namespace CaloCellPackerUtils {


/**
 * @brief Constructor.
 * @param The bit mask.  Should contain a single contiguous string of 1's.
 */
Bitfield::Bitfield (unsigned int mask /*= 1*/)
{
  // Shift the mask until the rightmost bit is non-zero.
  assert (mask != 0);
  m_shift = 0;
  while ((mask & 1) == 0) {
    mask >>= 1;
    ++m_shift;
  }
  m_mask = mask;
}


/**
 * @brief Constructor.
 * @param The bit mask.  Should contain a single contiguous string of 1's.
 * @param xmin The smallest storable value.
 * @param xmax The largest storable value.
 */
Floatfield::Floatfield (unsigned int mask/*=1*/,
                        double xmin/*=1*/,
                        double xmax/*=0*/)
  : Bitfield (mask),
    m_xmin (xmin),
    m_xmax (xmax),
    m_fact ((xmax-xmin) / (m_mask + 1)),
    m_ifact (1/m_fact)
{
}


/**
 * @brief Constructor.
 * @param The bit mask.  Should contain a single contiguous string of 1's.
 * @param xmax The largest storable value.
 */
Floatfield2::Floatfield2 (unsigned int mask/*=1*/,
                          double xmax/*=1*/)
  : Bitfield (mask),
    m_xmax (xmax),
    m_fact (xmax / (m_mask + 1)),
    m_ifact (1/m_fact)
{
}


} // namespace CaloCellPackerUtils

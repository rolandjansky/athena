/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileLaser.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief TileLaser out-of-line implementation.
 */


#include "TileEvent/TileLaser.h"
#include <sstream>


TileLaser::operator std::string() const
{
  std::ostringstream text;
  text << "TileLaser: ";
  text << "counter: " << m_counter << " ";
  text << "diodeCurrOrd: " << m_diodeCurrOrd << " ";
  text << "diodeCurrMeas: " << m_diodeCurrMeas << " ";
  text << "filtNumber: " << m_filtNumber << " ";
  text << "slamaDelay: " << m_slamaDelay << " ";
  return text.str();
}

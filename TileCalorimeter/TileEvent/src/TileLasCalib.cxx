/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileLasCalib.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief TileLasCalib out-of-line implementation.
 */


#include "TileEvent/TileLasCalib.h"
#include <sstream>


TileLasCalib::operator std::string() const
{
  std::ostringstream text;
  text << "TileLasCalib: ";
  text << "type: " << m_type << " ";
  text << "nevts: " << m_nevts << " ";
  text << "LG mean/sig: " << m_mean_LG << " " << m_sigma_LG << " (" << m_isSet_LG << ") ";
  text << "HG mean/sig: " << m_mean_HG << " " << m_sigma_HG << " (" << m_isSet_HG << ") ";
  return text.str();
}

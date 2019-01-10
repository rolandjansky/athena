/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileLaserPmt.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief TileLaserPmt out-of-line implementation.
 */


#include "TileEvent/TileLaserPmt.h"
#include <sstream>


TileLaserPmt::operator std::string() const
{
  std::ostringstream text;
  text << "TileLaserPmt: ";
  text << "PMADC: " << m_PMADC << " ";
  text << "TDC: " << m_TDC << " ";
  text << "ped: " << m_pmPedestal << " ";
  text << "sigPed: " << m_pmSigmaPedestal;
  return text.str();
}

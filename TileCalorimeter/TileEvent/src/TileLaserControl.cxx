/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileLaserControl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief TileLaserControl out-of-line implementation.
 */


#include "TileEvent/TileLaserControl.h"
#include <sstream>


TileLaserControl::operator std::string() const
{
  std::ostringstream text;
  text << "TileLaserControl: ";
  text << "pumpDiodeTemp: " << m_pumpDiodeTemp << " ";
  text << "diodeBoxTemp: " << m_diodeBoxTemp << " ";
  text << "timeLastMeasP: " << m_timeLastMeasP << " ";
  text << "gasFlux: " << m_gasFlux << " ";
  text << "timeLastMeasF: " << m_timeLastMeasF << " ";
  text << "humidity: " << m_humidity << " ";
  text << "timeLastMeasH: " << m_timeLastMeasH << " ";
  text << "lastPedMeas: " << m_lastPedMeas << " ";
  text << "lastAlphaMeas: " << m_lastAlphaMeas;
  return text.str();
}

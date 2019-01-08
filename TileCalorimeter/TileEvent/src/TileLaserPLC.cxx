/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileLaserPLC.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief TileLaserPLC out-of-line implementation.
 */


#include "TileEvent/TileLaserPLC.h"
#include <sstream>


TileLaserPLC::operator std::string() const
{
  std::ostringstream text;
  text << "TileLaserPLC: ";
  text << "alphaPos: " << m_alphaPos << " ";
  text << "LVdiodes: " << m_LVdiodes << " ";
  text << "HVpmts: " << m_HVpmts << " ";
  text << "shutter: " << m_shutter << " ";
  text << "interlock: " << m_interlock << " ";
  text << "alarm: " << m_alarm;
  return text.str();
}

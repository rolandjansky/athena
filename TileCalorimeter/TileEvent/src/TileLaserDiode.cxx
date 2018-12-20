/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileLaserDiode.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief TileLaserDiode out-of-line implementation.
 */


#include "TileEvent/TileLaserDiode.h"
#include <sstream>


TileLaserDiode::operator std::string() const
{
  std::ostringstream text;
  text << "TileLaserDiode: ";
  text << "ADC: " << m_diodeADC << " ";
  text << "ped: " << m_diodePedestal << " ";
  text << "sigPed: " << m_diodeSigmaPedestal << " ";
  text << "alpha: " << m_alpha << " ";
  text << "sigAlpha: " << m_sigmaAlpha << " ";
  text << "pedAlpha: " << m_pedestalAlpha << " ";
  text << "sigPedAlpha: " << m_sigmaPedAlpha;
  return text.str();
}

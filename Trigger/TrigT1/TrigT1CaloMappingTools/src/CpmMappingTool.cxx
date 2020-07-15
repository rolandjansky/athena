/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "CpmMappingTool.h"

namespace LVL1 {

// Static constants

const int    CpmMappingTool::s_crates;
const int    CpmMappingTool::s_modules;
const int    CpmMappingTool::s_channels;

const int    CpmMappingTool::s_etaBinsPerRow;

const double CpmMappingTool::s_etaGran = 0.1;
const double CpmMappingTool::s_phiGran = M_PI/32.;

// Initialise the mappings

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CpmMappingTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CpmMappingTool::finalize()
{

  return StatusCode::SUCCESS;
}
 
// Return eta, phi and layer mapping for given crate/module/channel

bool CpmMappingTool::mapping(const int crate, const int module,
               const int channel, double& eta, double& phi, int& layer) const
{
  if (crate < 0 || crate >= s_crates || module < 1 || module > s_modules ||
      channel < 0 || channel >= s_channels) return false;

  // Channels numbered thus:
  //
  //  :    :    :    :    :
  //  |  9 | 11 | 13 | 15 |
  //  |  8 | 10 | 12 | 14 | phi
  //  |  1 |  3 |  5 |  7 |
  //  |  0 |  2 |  4 |  6 |
  //  +----+----+----+----+
  //           eta

  const int etaBin = (channel / 2) % s_etaBinsPerRow;
  const int phiBin = ((channel / 2) / s_etaBinsPerRow) * 2
                                    + channel % 2 - 2;  // allow for overlap

  // End modules only have one column
  if ((module == 1 && etaBin != s_etaBinsPerRow - 1) ||
      (module == s_modules && etaBin != 0)) return false;

  const double twoPi   = 2.*M_PI;
  const double phiBase = M_PI/2. * double(crate);
  phi = phiBase + s_phiGran * (double(phiBin) + 0.5);
  if      (phi < 0.)     phi += twoPi;
  else if (phi >= twoPi) phi -= twoPi;

  const double etaBase = s_etaGran * s_etaBinsPerRow *
                                               (module - 1 - s_modules/2);
  eta = etaBase + s_etaGran * (double(etaBin) + 0.5);
 
  // Set layer to 1 for overlap channel, 0 for core
  layer = (phiBin < 0 || phiBin > 15) ? 1 : 0;
  return true;
}

// Return crate, module and channel mapping for given eta/phi/layer

bool CpmMappingTool::mapping(const double /*eta*/, const double /*phi*/,
                    const int /*layer*/, int& crate, int& module, int& channel) const
{
  // Not implemented
  crate   = 0;
  module  = 0;
  channel = 0;
  return false;
}

} // end namespace

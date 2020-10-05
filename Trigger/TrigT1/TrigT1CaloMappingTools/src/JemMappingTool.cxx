/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "JemMappingTool.h"

namespace LVL1 {

// Static constants

const int    JemMappingTool::s_crates;
const int    JemMappingTool::s_modules;
const int    JemMappingTool::s_channels;

const int    JemMappingTool::s_modulesPerQuadrant;
const int    JemMappingTool::s_extremeNegModule;
const int    JemMappingTool::s_extremePosModule;
const int    JemMappingTool::s_etaBinsPerRow;

const double JemMappingTool::s_etaGran = 0.2;
const double JemMappingTool::s_phiGran = M_PI/16.;


// Initialise the mappings

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode JemMappingTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  // Initialise extreme eta data vectors
  const double etasNeg[s_etaBinsPerRow] = {-4.05, -3.05, -2.8, -2.55 };
  const double granNeg[s_etaBinsPerRow] = { 1.7,   0.3,   0.2,  0.3  };
  const double etasPos[s_etaBinsPerRow] = { 2.55,  2.8,   3.05, 4.05 };
  const double granPos[s_etaBinsPerRow] = { 0.3,   0.2,   0.3,  1.7  };
  m_etasNegative.assign(etasNeg, etasNeg + s_etaBinsPerRow);
  m_granNegative.assign(granNeg, granNeg + s_etaBinsPerRow);
  m_etasPositive.assign(etasPos, etasPos + s_etaBinsPerRow);
  m_granPositive.assign(granPos, granPos + s_etaBinsPerRow);

  // Initialise extreme eta channel remapping vectors
  const int negChans[s_channels] = { 1,-1, 2, 3, -1,-1, 6, 7,  0,36,10,11,
                                     5, 9,14,15, 17,13,18,19,  4,12,22,23,
			            28,20,26,27, 21,25,30,31, 33,29,34,35,
			            37,41,38,39, -1,-1,42,43 };
  const int posChans[s_channels] = { 0, 1, 2,-1,  4, 5,-1,-1,  8, 9, 3,39,
                                    12,13, 6,10, 16,17,18,14, 20,21, 7,15,
			            24,25,31,23, 28,29,22,26, 32,33,34,30,
			            36,37,38,42, 40,41,-1,-1 };
  m_negChans.assign(negChans, negChans + s_channels);
  m_posChans.assign(posChans, posChans + s_channels);

  return StatusCode::SUCCESS;
}

StatusCode JemMappingTool::finalize()
{

  return StatusCode::SUCCESS;
}
 
// Return eta, phi and layer mapping for given crate/module/channel

bool JemMappingTool::mapping(const int crate, const int module,
               const int channel, double& eta, double& phi, int& layer) const
{
  if (crate < 0 || crate >= s_crates || module < 0 || module >= s_modules ||
      channel < 0 || channel >= s_channels) return false;

  // Channel remapping needed at extreme eta

  int chan = channel;
  const int quadMod = module % s_modulesPerQuadrant;
  if      (quadMod == s_extremeNegModule) chan = m_negChans[channel];
  else if (quadMod == s_extremePosModule) chan = m_posChans[channel];
  if (chan < 0) return false;
  const int etaBin = chan % s_etaBinsPerRow;
  const int phiBin = chan / s_etaBinsPerRow - 1;  // allow for overlap

  // Phi granularity doubles at FCAL

  const double twoPi   = 2.*M_PI;
  const double phiBase = M_PI/2. * double(crate)
                         + M_PI  * double(module/s_modulesPerQuadrant);
  phi = phiBase + s_phiGran * (double(phiBin) + 0.5);
  if (((quadMod == s_extremeNegModule) && (etaBin == 0)) ||
      ((quadMod == s_extremePosModule) && (etaBin == s_etaBinsPerRow - 1))) {
    if (chan < s_etaBinsPerRow) phi -= s_phiGran / 2.;
    else                        phi += s_phiGran / 2.;
  }
  if      (phi < 0.)     phi += twoPi;
  else if (phi >= twoPi) phi -= twoPi;

  // Eta granularity varies at endcap/FCAL

  if      (quadMod == s_extremeNegModule) eta = m_etasNegative[etaBin];
  else if (quadMod == s_extremePosModule) eta = m_etasPositive[etaBin];
  else {
    const double etaBase = s_etaGran * s_etaBinsPerRow
                                     * (quadMod - s_modulesPerQuadrant/2);
    eta = etaBase + s_etaGran * (double(etaBin) + 0.5);
  }

  // Set layer to 1 for overlap channel, 0 for core
  layer = (phiBin < 0 || phiBin > 7) ? 1 : 0;

  return true;
}

// Return crate, module and channel mapping for given eta/phi/layer

bool JemMappingTool::mapping(const double /*eta*/, const double /*phi*/,
                    const int /*layer*/, int& crate, int& module, int& channel) const
{
  // Not implemented
  crate   = 0;
  module  = 0;
  channel = 0;
  return false;
}

} // end namespace

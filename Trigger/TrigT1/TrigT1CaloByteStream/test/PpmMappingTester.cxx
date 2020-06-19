/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"

#include "PpmMappingTester.h"

namespace LVL1BS {

PpmMappingTester::PpmMappingTester(const std::string& name,
                                   ISvcLocator* pSvcLocator)
 : AthAlgorithm(name, pSvcLocator),
   m_tool1("LVL1::PpmMappingTool/PpmMappingTool"),
   m_tool2("LVL1::PpmCoolMappingTool/PpmCoolMappingTool")
{

  declareProperty("PpmMappingTool1", m_tool1);
  declareProperty("PpmMappingTool2", m_tool2);

  declareProperty("Timing", m_timing = -1);

  // Initialise m_etaBins
  double base = -4.9;
  const double width = 0.425;
  const double offset1 = width/2.;
  for (int i = 0; i < 4; ++i) {
    m_etaBins.push_back(base + double(i)*width + offset1);
  }
  m_etaBins.push_back(-3.15);
  m_etaBins.push_back(-3.0);
  m_etaBins.push_back(-2.8);
  m_etaBins.push_back(-2.6);
  const double offset2 = 0.05;
  for (int i = -25; i < 25; ++i) {
    m_etaBins.push_back(double(i)/10. + offset2);
  }
  m_etaBins.push_back(2.6);
  m_etaBins.push_back(2.8);
  m_etaBins.push_back(3.0);
  m_etaBins.push_back(3.15);
  base = 3.2;
  for (int i = 0; i < 4; ++i) {
    m_etaBins.push_back(base + double(i)*width + offset1);
  }
}

PpmMappingTester::~PpmMappingTester()
{
}

// Initialisation

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode PpmMappingTester::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endmsg;

  // Retrieve mapping tools

  StatusCode sc = m_tool1.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_tool1 << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_tool1 << endmsg;

  sc = m_tool2.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_tool2 << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_tool2 << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode PpmMappingTester::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode PpmMappingTester::execute()
{
  // Timing tests

  std::vector<unsigned int> chanIds;
  std::vector<double> etas;
  std::vector<double> phis;
  std::vector<int>    layers;
  if      (m_timing == 0) etaPhiToChannel(m_tool1, chanIds);
  else if (m_timing == 1) etaPhiToChannel(m_tool1, chanIds);
  else if (m_timing == 2) etaPhiToChannel(m_tool2, chanIds);
  else if (m_timing == 3) channelToEtaPhi(m_tool1, etas, phis, layers);
  else if (m_timing == 4) channelToEtaPhi(m_tool2, etas, phis, layers);
  else {

  // Comparison tests
  
    etaPhiToChannel(m_tool1, chanIds);
    std::vector<unsigned int> chanIds2;
    etaPhiToChannel(m_tool2, chanIds2);
    unsigned int size1 = chanIds.size();
    unsigned int size2 = chanIds2.size();
    msg(MSG::INFO) << "Vector sizes: " << size1 << ", " << size2 << endmsg;
    if (size1 == size2) {
      for (unsigned int i = 0; i < size1; ++i) {
        if (chanIds[i] != chanIds2[i]) {
	  msg(MSG::INFO) << "ChanID mismatch: " << chanIds[i] << ", "
	                 << chanIds2[i] << endmsg;
        }
      }
    }
    channelToEtaPhi(m_tool1, etas, phis, layers);
    std::vector<double> etas2;
    std::vector<double> phis2;
    std::vector<int>    layers2;
    channelToEtaPhi(m_tool2, etas2, phis2, layers2);
    size1 = etas.size();
    size2 = etas2.size();
    msg(MSG::INFO) << "Vector sizes: " << size1 << ", " << size2 << endmsg;
    if (size1 == size2) {
      for (unsigned int i = 0; i < size1; ++i) {
        if (notEqual(etas[i], etas2[i]) ||
	    notEqual(phis[i], phis2[i]) ||
	    notEqual(layers[i], layers2[i])) {
	  msg(MSG::INFO) << "ChanID mismatch: eta/phi/layer "
	                 << etas[i] << "/" << phis[i] << "/"
			 << layers[i] << "/" 
	                 << etas2[i] << "/" << phis2[i] << "/"
			 << layers2[i]      << endmsg;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

// Return a list of TT channel IDs for all possible eta/phi/layers

void PpmMappingTester::etaPhiToChannel(
                       ToolHandle<LVL1::IL1CaloMappingTool>& tool,
				  std::vector<unsigned int>& chanIds)
{
  const double etaMin = -4.9;
  const double etaMax = 4.9;
  const double phiMin = 0.;
  const double phiMax = 2.*M_PI;
  std::vector<double>::const_iterator etaPos  = m_etaBins.begin();
  std::vector<double>::const_iterator etaPosE = m_etaBins.end();
  for (; etaPos != etaPosE; ++etaPos) {
    const double eta = *etaPos;
    if (eta < etaMin) continue;
    if (eta > etaMax) break;
    const double absEta = (eta < 0.) ? -eta : eta;
    const int phiBins = (absEta > 3.2) ? 16 : (absEta > 2.5) ? 32 : 64;
    const double phiGran = 2.*M_PI/phiBins;
    for (int bin = 0; bin < phiBins; ++bin) {
      const double phi = phiGran*(double(bin) + 0.5);
      if (phi < phiMin) continue;
      if (phi > phiMax) break;
      int crate, module, channel;
      if (m_timing) {
        if (tool->mapping(eta, phi, 0, crate, module, channel)) {
          const unsigned int id = (crate * 16 + module) * 64 + channel;
          chanIds.push_back(id);
        }
      } else chanIds.push_back(0);
      if (m_timing) {
        if (tool->mapping(eta, phi, 1, crate, module, channel)) {
          const unsigned int id = (crate * 16 + module) * 64 + channel;
	  chanIds.push_back(id);
        }
      } else chanIds.push_back(0);
    }
  }
}

void PpmMappingTester::channelToEtaPhi(
                       ToolHandle<LVL1::IL1CaloMappingTool>& tool,
                       std::vector<double>& etas, std::vector<double>& phis,
                       std::vector<int>& layers)
{
  const int ncrates = 8;
  const int nmodules = 16;
  const int nchannels = 64;
  for (int crate = 0; crate < ncrates; ++crate) {
    for (int module = 0; module < nmodules; ++module) {
      for (int channel = 0; channel < nchannels; ++channel) {
	double eta = 0.;
	double phi = 0.;
	int layer = 0;
        if (m_timing) {
	  if (tool->mapping(crate, module, channel, eta, phi, layer)) {
	    etas.push_back(eta);
	    phis.push_back(phi);
	    layers.push_back(layer);
          }
        } else {
	  etas.push_back(0);
	  phis.push_back(0);
	  layers.push_back(0);
        }
      }
    }
  }
}

bool PpmMappingTester::notEqual(double item1, double item2)
{
  const double tolerance = 0.00001;
  return std::abs(item1 - item2) > tolerance;
}

} // end namespace

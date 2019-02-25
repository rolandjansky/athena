/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <set>

#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include "L1CaloSrcIdMap.h"

#include "TriggerTowerSelectionTool.h"

namespace LVL1BS {

TriggerTowerSelectionTool::TriggerTowerSelectionTool(const std::string& type,
                                         const std::string& name,
					 const IInterface*  parent)
 : AthAlgTool(type, name, parent),
   m_mappingTool("LVL1::PpmMappingTool/PpmMappingTool"),
   m_srcIdMap(0)
{
  declareInterface<ITriggerTowerSelectionTool>(this);

  declareProperty("PpmMappingTool", m_mappingTool);

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

TriggerTowerSelectionTool::~TriggerTowerSelectionTool()
{
}

// Initialisation


StatusCode TriggerTowerSelectionTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  // Retrieve mapping tool

  StatusCode sc = m_mappingTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_mappingTool << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_mappingTool << endmsg;

  m_srcIdMap = new L1CaloSrcIdMap();

  return StatusCode::SUCCESS;
}

StatusCode TriggerTowerSelectionTool::finalize()
{
  delete m_srcIdMap;

  return StatusCode::SUCCESS;
}

// Return a list of TT channel IDs for given eta/phi range

void TriggerTowerSelectionTool::channelIDs(const double etaMin,
                                           const double etaMax,
                                           const double phiMin,
					   const double phiMax,
					   std::vector<unsigned int>& chanIds)
{
  std::set<unsigned int> chanSet;
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
      m_mappingTool->mapping(eta, phi, 0, crate, module, channel);
      unsigned int id = (crate * 16 + module) * 64 + channel;
      chanSet.insert(id);
      m_mappingTool->mapping(eta, phi, 1, crate, module, channel);
      id = (crate * 16 + module) * 64 + channel;
      chanSet.insert(id);
    }
  }
  std::set<unsigned int>::const_iterator setPos  = chanSet.begin();
  std::set<unsigned int>::const_iterator setPosE = chanSet.end();
  for (; setPos != setPosE; ++setPos) chanIds.push_back(*setPos);
}

// Return a list of ROB IDs for given list of TT channel IDs

void TriggerTowerSelectionTool::robIDs(const std::vector<unsigned int>& chanIds,
                                             std::vector<uint32_t>& robs)
{
  const int channels = 64;
  const int modules  = 16;
  const int slinks   = 4;
  const int daqOrRoi = 0;
  std::set<uint32_t> robSet;
  std::vector<unsigned int>::const_iterator pos  = chanIds.begin();
  std::vector<unsigned int>::const_iterator posE = chanIds.end();
  for (; pos != posE; ++pos) {
    const int chanId = *pos;
    const int crate  = chanId / (channels * modules);
    const int module = (chanId / channels) % modules;
    const int slink  = module / slinks;
    const uint32_t rodId = m_srcIdMap->getRodID(crate, slink, daqOrRoi,
                                                eformat::TDAQ_CALO_PREPROC);
    const uint32_t robId = m_srcIdMap->getRobID(rodId);
    robSet.insert(robId);
  }
  std::set<uint32_t>::const_iterator setPos  = robSet.begin();
  std::set<uint32_t>::const_iterator setPosE = robSet.end();
  for (; setPos != setPosE; ++setPos) robs.push_back(*setPos);
}

} // end namespace

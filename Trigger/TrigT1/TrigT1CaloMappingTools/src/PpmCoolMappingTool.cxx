/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "PpmCoolMappingTool.h"

namespace LVL1 {

const int PpmCoolMappingTool::s_maxTableEntries;


// Initialise the mappings

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode PpmCoolMappingTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  // Retrieve the CaloTriggerTowerService tool
  StatusCode sc = m_ttSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_ttSvc << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_ttSvc << endmsg;

  // Retrieve the CaloIdManager from the detector store
  const CaloIdManager* caloMgr = 0;
  sc = detStore()->retrieve(caloMgr);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve CaloIdManager from DetectorStore"
                    << endmsg;
    return sc;
  }

  // Use the CaloIdManager to get a pointer to an instance
  // of the CaloLVL1_ID helper
  m_lvl1Helper = caloMgr->getLVL1_ID();
  if (!m_lvl1Helper) {
    msg(MSG::ERROR) << "Could not access CaloLVL1_ID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  // Use the CaloIdManager to get a pointer to an instance
  // of the TTOnlineID helper
  m_l1ttonlineHelper = caloMgr->getTTOnlineID();
  if (!m_l1ttonlineHelper ) {
    msg(MSG::ERROR) << "Could not access TTOnlineID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  // Incident Service:
  IIncidentSvc* incSvc = 0;
  sc = service("IncidentSvc", incSvc);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve pointer to IncidentSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  //start listening to "BeginRun"
  if (incSvc) incSvc->addListener(this, "BeginRun");


  return StatusCode::SUCCESS;
}

StatusCode PpmCoolMappingTool::finalize()
{

  return StatusCode::SUCCESS;
}

// Reset mapping table at start of run 

void PpmCoolMappingTool::handle(const Incident& inc)
{
  // FIXME: not thread safe --- won't work if there's more than one run
  // in the input.
  if (inc.type()=="BeginRun") {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Resetting mapping table at start of run" << endmsg;
    }

    bool verbose = msgLvl(MSG::VERBOSE);
    m_idTable.resize (s_maxTableEntries);
    for (int index = 0; index < s_maxTableEntries; ++index) {
      const int channel = index & 0x3f;
      const int module = (index >> 6) & 0x0f;
      const int crate = index >> 10;
      const int slot = module + 5;
      const int pin  = channel % 16;
      const int asic = channel / 16;

      Identifier ttId(0);
      Identifier invalidId(0);
      try {
        if (verbose) {
          msg(MSG::VERBOSE) << "crate/module/channel " << crate << "/"
                            << module << "/" << channel
                            << "  maps to crate/slot/pin/asic " << crate << "/"
                            << slot << "/" << pin << "/" << asic << endmsg;
        }
        const HWIdentifier id = m_l1ttonlineHelper->channelId(crate, slot, pin,
                                                              asic);
        if (verbose) {
          msg(MSG::VERBOSE) << "hardware_id: " << id << endmsg;
        }
        ttId = m_ttSvc->cnvToIdentifier(id, true);
        if (verbose) {
          msg(MSG::VERBOSE) << "tower_id: " << ttId << endmsg;
        }
      }
      catch (const CaloID_Exception&) { ttId = invalidId; }
      if (ttId == invalidId) {
        m_idTable[index] = 0;
      }
      else {
        const int side   = (m_lvl1Helper->pos_neg_z(ttId) == 1) ? 1 : 2;
        const int sample = m_lvl1Helper->sampling(ttId);
        const int region = m_lvl1Helper->region(ttId);
        const int ieta   = m_lvl1Helper->eta(ttId);
        const int iphi   = m_lvl1Helper->phi(ttId);
        m_idTable[index] = (side<<14)+(sample<<13)+(region<<11)+(ieta<<6)+iphi;
      }
    }
  }
  return;
} 
 
// Return eta, phi and layer mapping for given crate/module/channel

bool PpmCoolMappingTool::mapping(const int crate, const int module,
         const int channel, double& eta, double& phi, int& layer) const
{
  if (crate < 0 || crate >= 8 || module < 0 || module >= 16 ||
      channel < 0 || channel >= 64) return false;

  int index = (crate<<10) + (module<<6) + channel;
  if (index >= s_maxTableEntries) return false;
  
  bool verbose = msgLvl(MSG::VERBOSE);

  if (index >= static_cast<int>(m_idTable.size()) || m_idTable[index] == 0) {
    return false;
  }
  const unsigned int entry = m_idTable[index];
  const int side   = ((entry>>14) == 1) ? 1 : -1;
  const int region = (entry>>11)&0x3;
  const int ieta   = (entry>>6)&0x1f;
  const int iphi   = entry&0x3f;
  const double etaOffsets[4] = { 0., 2.5, 3.1, 3.2 };
  const double etaGrans[4]   = { 0.1, 0.2, 0.1, 0.425 };
  const double phiGrans[4]   = { M_PI/32., M_PI/16., M_PI/16., M_PI/8. };
  
  eta = side * (etaOffsets[region] + etaGrans[region] * (ieta + 0.5));
  phi = phiGrans[region] * (iphi + 0.5);
  layer = (entry>>13)&0x1;

  if (verbose) {
    msg(MSG::VERBOSE) << "crate/module/channel " << crate << "/" << module
                      << "/" << channel << "  maps to eta/phi/layer "
  		      << eta << "/" << phi << "/" << layer << endmsg;
  }

  return true;
}

// Return crate, module and channel mapping for given eta/phi/layer

bool PpmCoolMappingTool::mapping(const double eta, const double phi,
                    const int layer, int& crate, int& module, int& channel) const
{
  if (eta <= -4.9 || eta >= 4.9 || phi <= 0. || phi >= 2.*M_PI) return false;

  const double etaOffsets[5] = { 0., 2.5, 3.1, 3.2, 4.9 };
  const double etaGrans[4]   = { 0.1, 0.2, 0.1, 0.425 };
  const double phiGrans[4]   = { M_PI/32., M_PI/16., M_PI/16., M_PI/8. };
  const int side = (eta < 0.) ? -1 : 1;
  int region = 0;
  int ieta   = 0;
  int iphi   = 0;
  const double absEta = fabs(eta);
  for (int i = 0; i < 4; ++i) {
    if (absEta < etaOffsets[i+1]) {
      region = i;
      ieta   = int((absEta - etaOffsets[i]) / etaGrans[i]);
      iphi   = int(phi / phiGrans[i]);
      break;
    }
  }

  bool verbose = msgLvl(MSG::VERBOSE);

  HWIdentifier id(0);
  HWIdentifier invalidId(0);
  try {
    if (verbose) {
      msg(MSG::VERBOSE) << "eta/phi/layer " << eta << "/" << phi << "/"
                        << layer << "  maps to side/layer/region/ieta/iphi "
                        << side << "/" << layer << "/" << region << "/"
		        << ieta << "/" << iphi << endmsg;
    }
    const Identifier ttId = m_lvl1Helper->tower_id(side, layer, region,
                                                            ieta, iphi);
    if (verbose) {
      msg(MSG::VERBOSE) << "tower_id: " << ttId << endmsg;
    }
    id = m_ttSvc->createTTChannelID(ttId, true);
    if (verbose) {
      msg(MSG::VERBOSE) << "hardware_id: " << id << endmsg;
    }
  }
  catch (const CaloID_Exception&) { id = invalidId; }
  if (id == invalidId) return false;

  const int slot = m_l1ttonlineHelper->module(id);
  const int pin  = m_l1ttonlineHelper->submodule(id);
  const int asic = m_l1ttonlineHelper->channel(id);

  crate   = m_l1ttonlineHelper->crate(id);
  module  = slot - 5;
  channel = asic * 16 + pin;

  if (verbose) {
    msg(MSG::VERBOSE) << "eta/phi/layer " << eta << "/" << phi << "/" << layer
                      << "  maps to crate/module/channel "
    		      << crate << "/" << module << "/" << channel << endmsg;
  }
    
  return true;
}

} // end namespace

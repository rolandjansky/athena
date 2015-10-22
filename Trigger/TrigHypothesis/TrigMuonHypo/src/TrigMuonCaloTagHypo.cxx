/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonCaloTagHypo.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

TrigMuonCaloTagHypo::TrigMuonCaloTagHypo(const std::string& name, ISvcLocator* pSvcLocator)
  :
  HLT::HypoAlgo(name, pSvcLocator),
  m_acceptAll(false),
  m_storeGate(nullptr),
  m_ptMin(4*CLHEP::GeV),
  m_etaMax(3.0),
  m_doTight(false),
  m_maxMissedCells(1)
{
  declareProperty("AcceptAll", m_acceptAll);
  declareProperty("PtMin", m_ptMin);
  declareProperty("EtaMax", m_etaMax);
  declareProperty("TightCaloTag", m_doTight);
  declareProperty("MaxMissingCells", m_maxMissedCells);
  declareProperty("TrackContainerName", m_ctTrackContName,"MuonEFInfo_CaloTagTrackParticles");
}

TrigMuonCaloTagHypo::~TrigMuonCaloTagHypo()
{
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltInitialize()
{
  if (m_acceptAll) {
    ATH_MSG_INFO("Accepting all events");
  }

  ATH_MSG_INFO("Initialized TrigMuonCaloTagHypo with threshold " << m_ptMin << " GeV");
  ATH_MSG_INFO("Completed intialization successfully");
  return HLT::OK;
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  m_storeGate = store();

  ATH_MSG_INFO("In execute");
  
  if (m_acceptAll) {
    pass = true;
    ATH_MSG_INFO("Accepting event");
    return HLT::OK;
  }

  //Retrieve tagged track particles
  const xAOD::TrackParticleContainer* tpCont = nullptr;
  HLT::ErrorCode status = getFeature<xAOD::TrackParticleContainer>(outputTE, tpCont, m_ctTrackContName);

  if (status != HLT::OK) {
    ATH_MSG_WARNING("Failed to retrieve xAOD::TrackParticleContainer " << m_ctTrackContName);
  }
  else {
    ATH_MSG_INFO("Retrieved track particles");
  }

  //Check whether a particle passes the selection
  for(const xAOD::TrackParticle* tp : *tpCont) {
    auto calotag = tp->auxdata<unsigned short>("CaloTag");
    ATH_MSG_INFO("Track Particle: pt, eta, phi = " << tp->pt() << ", " << tp->eta() << ", " << tp->phi() << ", Tag: " << calotag);

    if (tp->pt() < m_ptMin) continue;
    if (fabs(tp->eta()) > m_etaMax) continue;
    
    if (m_doTight){
      if (calotag < (4 - m_maxMissedCells)*10) continue;
    }else{
      if (calotag < (4 - m_maxMissedCells)) continue;
    }
    
    pass = true;
    break;
  }

  return HLT::OK;
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltFinalize()
{
  ATH_MSG_INFO("In finalize");

  return HLT::OK;
}

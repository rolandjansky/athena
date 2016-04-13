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
  m_ptThresholds(),
  m_etaMax(3.0),
  m_doLH(true),
  m_lhCut(0.95),
  m_doTight(true),
  m_maxMissedCells(1),
  m_ctTrackContName("MuonEFInfo_CaloTagTrackParticles")
{
  declareProperty("AcceptAll", m_acceptAll);
  declareProperty("PtThresholds", m_ptThresholds);
  declareProperty("EtaMax", m_etaMax);
  declareProperty("UseLH",        m_doLH);
  declareProperty("LHCut",        m_lhCut);
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

  std::string thrsh_str;
  for(const auto& t : m_ptThresholds)
    thrsh_str = std::to_string(t) + "GeV, ";

  ATH_MSG_INFO("Initialized TrigMuonCaloTagHypo with thresholds " << thrsh_str);
  ATH_MSG_INFO("Completed intialization successfully");
  return HLT::OK;
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  m_storeGate = store();

  ATH_MSG_DEBUG("In execute");
  
  if (m_acceptAll) {
    pass = true;
    ATH_MSG_DEBUG("Accepting event");
    return HLT::OK;
  }

  //Retrieve tagged track particles
  const xAOD::TrackParticleContainer* tpCont(0);

  if (getFeature<xAOD::TrackParticleContainer>(outputTE, tpCont, m_ctTrackContName)!= HLT::OK || tpCont==0) {
    ATH_MSG_DEBUG("Failed to retrieve xAOD::TrackParticleContainer " << m_ctTrackContName);
    return HLT::MISSING_FEATURE;
  }

  //Check whether a particle passes the selection
  unsigned int nPassed = 0;
  for(const xAOD::TrackParticle* tp : *tpCont) {
    ATH_MSG_DEBUG("Track Particle: pt, eta, phi, ptcut = " << tp->pt() << ", " << tp->eta() << ", " << tp->phi() << ", " << m_ptThresholds[nPassed]);

    if (tp->pt() < m_ptThresholds[nPassed]) continue;
    if (fabs(tp->eta()) > m_etaMax) continue;
    
    if (m_doLH) {
      auto lh      = tp->auxdata<double>("CaloTagLH");
      ATH_MSG_DEBUG("Track Particle: lh, lhcut = " << ", LH: " << lh << ", cut: " << m_lhCut);
      if (lh < m_lhCut) continue;
    } 
    else {
      auto calotag = tp->auxdata<unsigned short>("CaloTag");
      ATH_MSG_DEBUG("Track Particle: ct = " << calotag );
      if (m_doTight){
        if (calotag < (4 - m_maxMissedCells)*10) continue;
      }else{
        if (calotag < (4 - m_maxMissedCells)) continue;
      }
    }

    ++nPassed; 
    ATH_MSG_DEBUG("Muon passed");
    //Check if we've found all required muons
    if (nPassed >= m_ptThresholds.size()) break;
  }

  pass = (nPassed >= m_ptThresholds.size());
  ATH_MSG_DEBUG("Found muons: " << nPassed << ", passed = " << pass);

  return HLT::OK;
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltFinalize()
{
  ATH_MSG_DEBUG("In finalize");

  return HLT::OK;
}

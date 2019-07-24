/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HnlSkimmingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Avishek Chatterjee (avishek.chatterjee@cern.ch)
// Heavily borrowed from DerivationFramework/DerivationFrameworkExamples/src/SkimmingToolExample.cxx 
// and DerivationFramework/DerivationFrameworkMuons/src/dimuonTaggingTool.cxx

#include "LongLivedParticleDPDMaker/HnlSkimmingTool.h"

#include "CLHEP/Units/SystemOfUnits.h"

// Constructor
DerivationFramework::HnlSkimmingTool::HnlSkimmingTool(const std::string& t,
                                                      const std::string& n,
                                                      const IInterface* p):
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);

  declareProperty("TrigDecisionTool", m_trigDecisionTool, "Tool to access the trigger decision");
  declareProperty("Triggers", m_triggers=std::vector< std::string >());

  declareProperty("MuonContainerKey", m_muonSGKey="Muons");
  declareProperty("Mu1PtMin", m_mu1PtMin=-1.);
  declareProperty("Mu1AbsEtaMax", m_mu1AbsEtaMax=2.5);
  declareProperty("Mu1Types", m_mu1Types=std::vector< int >());
  declareProperty("Mu1IsoType", m_mu1IsoType=xAOD::Iso::ptcone30);
  declareProperty("Mu1IsoCutIsRel", m_mu1IsoCutIsRel=1, "Cut is on relative isolation");
  declareProperty("Mu1IsoCut", m_mu1IsoCut=0.05);

  declareProperty("Mu2PtMin", m_mu2PtMin=-1.);
  declareProperty("Mu2AbsEtaMax", m_mu2AbsEtaMax=2.5);
  declareProperty("Mu2Types", m_mu2Types=std::vector< int >());
  declareProperty("Mu2IsoType", m_mu2IsoType=xAOD::Iso::ptcone30);
  declareProperty("Mu2IsoCutIsRel", m_mu2IsoCutIsRel=1, "Cut is on relative isolation");
  declareProperty("Mu2IsoCut", m_mu2IsoCut=1.);
  declareProperty("Mu2d0Min", m_mu2d0Min=0.1, "Unit is mm");
}

// Athena initialize and finalize
StatusCode DerivationFramework::HnlSkimmingTool::initialize()
{
  ATH_MSG_VERBOSE("HnlSkimmingTool initialize() ...");

  ATH_CHECK(m_trigDecisionTool.retrieve());
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::HnlSkimmingTool::finalize()
{
  ATH_MSG_VERBOSE("HnlSkimmingTool finalize() ...");

  return StatusCode::SUCCESS;
}

// The filter itself

// Check for the chi2 cut
bool DerivationFramework::HnlSkimmingTool::isGood(const xAOD::Muon& mu) const
{
  if (mu.muonType() != xAOD::Muon::Combined) return false;

  float chi2 = 0.;
  if (not mu.parameter(chi2, xAOD::Muon::msInnerMatchChi2)) return false;

  int dof = 1;
  if (not mu.parameter(dof, xAOD::Muon::msInnerMatchDOF)) return false;
  if (dof == 0) dof = 1;

  return (chi2 / static_cast<float>(dof)) < 5.;
}
/////////////////////End of check for the chi2 cut

bool DerivationFramework::HnlSkimmingTool::eventPassesFilter() const
{
  bool acceptEvent = false;

  // Trigger check
  bool passedTrigger = true;
  if (m_triggers.size()>0) {
    passedTrigger = false;
    for (const std::string& trigger : m_triggers) {
      bool decision = m_trigDecisionTool->isPassed(trigger);
      if (decision) {
        passedTrigger = true;
        break;
      }
    }
  }
  if (not passedTrigger) return acceptEvent;

  // Retrieve muon container 
  const xAOD::MuonContainer* muons = nullptr;
  StatusCode sc = evtStore()->retrieve(muons, m_muonSGKey);  
  if (sc.isFailure()) {
    ATH_MSG_FATAL("No muon collection with name " << m_muonSGKey << " found in StoreGate!");
    return false;
  }

  bool muon1passed = false;
  const xAOD::Muon* muon1 = nullptr;
  for (const xAOD::Muon* muon : *muons) {
    if (not (muon->pt()>m_mu1PtMin)) continue;

    if (not (std::abs(muon->eta())<m_mu1AbsEtaMax)) continue;

    bool passTypeCut = true;
    if (m_mu1Types.size()>0) {
      passTypeCut = false;
      int type = muon->muonType();
      for (const int& allowedType : m_mu1Types) {
        if (allowedType==type) {
          passTypeCut=true;
          break;
        }
      }
    }
    if (not passTypeCut) continue;

    bool isIso = false;
    float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu1IsoType);
    if (m_mu1IsoCutIsRel==0) {
      if (muon->isolation(isoValue, isoType) and (isoValue < m_mu1IsoCut)) isIso = true;
    } else {
      if (muon->isolation(isoValue, isoType) and (isoValue/muon->pt() < m_mu1IsoCut)) isIso = true;
    }
    if (isIso) {
      muon1passed = true;
      muon1 = muon;
      break;
    }
  }
  if (not muon1passed) return acceptEvent;

  bool muon2passed = false;
  for (const xAOD::Muon* muon : *muons) {
    if (muon==muon1) continue;

    if (not (muon->pt()>m_mu2PtMin)) continue;

    if (not (std::abs(muon->eta())<m_mu2AbsEtaMax)) continue;

    bool passTypeCut = true;
    int type = muon->muonType();
    if (m_mu2Types.size()>0) {
      passTypeCut = false;
      for (const int& allowedType : m_mu2Types) {
        if (type==allowedType) {
          passTypeCut = true;
          break;
        }
      }
    }
    if (not passTypeCut) continue;

    bool isIso = false;
    float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu2IsoType);
    if (m_mu2IsoCutIsRel==0) {
      if (muon->isolation(isoValue, isoType) and (isoValue < m_mu2IsoCut)) isIso = true;
    } else {
      if (muon->isolation(isoValue, isoType) and isoValue/muon->pt() < m_mu2IsoCut) isIso = true;
    }
    if (not isIso) continue;

    bool passD0cut = true;
    if (type==xAOD::Muon::Combined) { // d0 cut is for combined muons only
      passD0cut = false;
      if (isGood(*muon)) { // if muon has a good chi2/dof
        if (std::abs(muon->primaryTrackParticle()->d0())>m_mu2d0Min) passD0cut = true;
      } else passD0cut = true;
    }
    if (passD0cut) {
      muon2passed = true;
      break;
    }
  }

  if (muon2passed) acceptEvent = true;

  return acceptEvent;
}

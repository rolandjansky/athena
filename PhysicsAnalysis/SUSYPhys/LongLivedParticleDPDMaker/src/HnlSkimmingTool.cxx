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

  // Lepton types
  declareProperty("IsPromptMuon", m_isPromptMuon=true);
  declareProperty("IsDisplacedMuon", m_isDisplacedMuon=true);

  // Triggers
  declareProperty("TrigDecisionTool", m_trigDecisionTool, "Tool to access the trigger decision");
  declareProperty("Triggers", m_triggers=std::vector<std::string>());

  // Prompt muons
  declareProperty("Mu1PtMin", m_mu1PtMin=-1.);
  declareProperty("Mu1AbsEtaMax", m_mu1AbsEtaMax=2.5);
  declareProperty("Mu1Types", m_mu1Types=std::vector<int>());
  declareProperty("Mu1IsoType", m_mu1IsoType=xAOD::Iso::ptcone30);
  declareProperty("Mu1IsoCutIsRel", m_mu1IsoCutIsRel=true, "Cut is on relative isolation");
  declareProperty("Mu1IsoCut", m_mu1IsoCut=0.05);
  // Displaced muons
  declareProperty("Mu2PtMin", m_mu2PtMin=-1.);
  declareProperty("Mu2AbsEtaMax", m_mu2AbsEtaMax=2.5);
  declareProperty("Mu2Types", m_mu2Types=std::vector<int>());
  declareProperty("Mu2IsoType", m_mu2IsoType=xAOD::Iso::ptcone30);
  declareProperty("Mu2IsoCutIsRel", m_mu2IsoCutIsRel=true, "Cut is on relative isolation");
  declareProperty("Mu2IsoCut", m_mu2IsoCut=1.);
  declareProperty("Mu2d0Min", m_mu2d0Min=0.1, "Unit is mm");

  // Prompt electrons
  declareProperty("El1PtMin", m_el1PtMin=-1.);
  declareProperty("El1AbsEtaMax", m_el1AbsEtaMax=2.5);
  declareProperty("El1ID", m_el1IDKey="LHLoose");
  declareProperty("El1IsoType", m_el1IsoType=xAOD::Iso::ptcone30);
  declareProperty("El1IsoCutIsRel", m_el1IsoCutIsRel=true, "Cut is on relative isolation");
  declareProperty("El1IsoCut", m_el1IsoCut=0.05);
  // Displaced electrons
  declareProperty("El2PtMin", m_el2PtMin=-1.);
  declareProperty("El2AbsEtaMax", m_el2AbsEtaMax=2.5);
  declareProperty("El2IsoType", m_el2IsoType=xAOD::Iso::ptcone30);
  declareProperty("El2IsoCutIsRel", m_el2IsoCutIsRel=true, "Cut is on relative isolation");
  declareProperty("El2IsoCut", m_el2IsoCut=1.);
  declareProperty("El2d0Min", m_el2d0Min=1.0, "Unit is mm");

  declareProperty("dPhiMin", m_dPhiMin=0.0, "Unit is radian");
}

// Athena initialize and finalize
StatusCode DerivationFramework::HnlSkimmingTool::initialize()
{
  ATH_MSG_VERBOSE("HnlSkimmingTool initialize() ...");

  ATH_CHECK(m_trigDecisionTool.retrieve());
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);
  ATH_CHECK(m_muonSGKey.initialize());
  ATH_CHECK(m_electronSGKey.initialize());

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

  return ((chi2 / static_cast<float>(dof)) < 5.);
}
/////////////////////End of check for the chi2 cut

bool DerivationFramework::HnlSkimmingTool::eventPassesFilter() const
{
  bool acceptEvent = false;

  // Trigger check
  bool passedTrigger = true;
  if (m_triggers.size() > 0) {
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
  SG::ReadHandle<xAOD::MuonContainer> muons(m_muonSGKey);
  if (m_isPromptMuon or m_isDisplacedMuon) {
    if( !muons.isValid() ) {
      ATH_MSG_FATAL("No muon collection with name " << m_muonSGKey << " found in StoreGate!");
      return acceptEvent;
    }
  }

  // Retrieve electron container
  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronSGKey);
  if ((not m_isPromptMuon) or (not m_isDisplacedMuon)) {
    if( !electrons.isValid() ) {
      ATH_MSG_FATAL("No electron collection with name " << m_electronSGKey << " found in StoreGate!");
      return acceptEvent;
    }
  }

  // Prompt leptons
  std::vector<const xAOD::Muon*> promptMuonCandidates;
  std::vector<const xAOD::Electron*> promptElectronCandidates;
  if (m_isPromptMuon) {
    getPromptMuonCandidates(muons, promptMuonCandidates);
    if (promptMuonCandidates.empty()) return acceptEvent;
  } else {
    getPromptElectronCandidates(electrons, promptElectronCandidates);
    if (promptElectronCandidates.empty()) return acceptEvent;
  }

  // Displaced leptons
  std::vector<const xAOD::Muon*> displacedMuonCandidates;
  std::vector<const xAOD::Electron*> displacedElectronCandidates;
  if (m_isDisplacedMuon) {
    getDisplacedMuonCandidates(muons, displacedMuonCandidates);
    if (displacedMuonCandidates.empty()) return acceptEvent;
  } else {
    getDisplacedElectronCandidates(electrons, displacedElectronCandidates);
    if (displacedElectronCandidates.empty()) return acceptEvent;
  }

  // Final check: at least one pair of a prompt lepton candidate and
  // a displaced lepton candidate, which is different from the prompt lepton candidate
  if (m_isPromptMuon and m_isDisplacedMuon) {
    // mu-mu
    for (const xAOD::Muon* promptMuonCandidate : promptMuonCandidates) {
      for (const xAOD::Muon* displacedMuonCandidate : displacedMuonCandidates) {
        if (promptMuonCandidate!=displacedMuonCandidate) {
          double dPhi = promptMuonCandidate->phi() - displacedMuonCandidate->phi();
          while (dPhi>=+M_PI) dPhi -= 2.*M_PI;
          while (dPhi<=-M_PI) dPhi += 2.*M_PI;
          dPhi = std::abs(dPhi);
          if (dPhi>=m_dPhiMin) {
            acceptEvent = true;
            break;
          }
        }
      }
    }
  } else if (m_isPromptMuon and (not m_isDisplacedMuon)) {
    // mu-e
    for (const xAOD::Muon* promptMuonCandidate : promptMuonCandidates) {
      for (const xAOD::Electron* displacedElectronCandidate : displacedElectronCandidates) {
        double dPhi = promptMuonCandidate->phi() - displacedElectronCandidate->phi();
        while (dPhi>=+M_PI) dPhi -= 2.*M_PI;
        while (dPhi<=-M_PI) dPhi += 2.*M_PI;
        dPhi = std::abs(dPhi);
        if (dPhi>=m_dPhiMin) {
          acceptEvent = true;
          break;
        }
      }
    }
  } else if ((not m_isPromptMuon) and m_isDisplacedMuon) {
    // e-mu
    for (const xAOD::Electron* promptElectronCandidate : promptElectronCandidates) {
      for (const xAOD::Muon* displacedMuonCandidate : displacedMuonCandidates) {
        double dPhi = promptElectronCandidate->phi() - displacedMuonCandidate->phi();
        while (dPhi>=+M_PI) dPhi -= 2.*M_PI;
        while (dPhi<=-M_PI) dPhi += 2.*M_PI;
        dPhi = std::abs(dPhi);
        if (dPhi>=m_dPhiMin) {
          acceptEvent = true;
          break;
        }
      }
    }
  } else if ((not m_isPromptMuon) and (not m_isDisplacedMuon)) {
    // e-e
    for (const xAOD::Electron* promptElectronCandidate : promptElectronCandidates) {
      for (const xAOD::Electron* displacedElectronCandidate : displacedElectronCandidates) {
        if (promptElectronCandidate!=displacedElectronCandidate) {
          double dPhi = promptElectronCandidate->phi() - displacedElectronCandidate->phi();
          while (dPhi>=+M_PI) dPhi -= 2.*M_PI;
          while (dPhi<=-M_PI) dPhi += 2.*M_PI;
          dPhi = std::abs(dPhi);
          if (dPhi>=m_dPhiMin) {
            acceptEvent = true;
            break;
          }
        }
      }
    }
  }

  return acceptEvent;
}

void DerivationFramework::HnlSkimmingTool::getPromptMuonCandidates(SG::ReadHandle<DataVector<xAOD::Muon_v1>>& muons,
                                                                   std::vector<const xAOD::Muon*>& promptMuonCandidates) const
{
  for (const xAOD::Muon* muon : *muons) {
    // pT cut
    if (not (muon->pt() > m_mu1PtMin)) continue;

    // eta cut
    if (not (std::abs(muon->eta()) < m_mu1AbsEtaMax)) continue;

    // type cut
    bool passTypeCut = true;
    if (m_mu1Types.size() > 0) {
      passTypeCut = false;
      int type = muon->muonType();
      for (const int& allowedType : m_mu1Types) {
        if (allowedType==type) {
          passTypeCut = true;
          break;
        }
      }
    }
    if (not passTypeCut) continue;

    // isolation cut
    bool isIso = false;
    float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu1IsoType);
    if (not m_mu1IsoCutIsRel) {
      if (muon->isolation(isoValue, isoType) and (isoValue < m_mu1IsoCut)) isIso = true;
    } else {
      if (muon->isolation(isoValue, isoType) and (isoValue/muon->pt() < m_mu1IsoCut)) isIso = true;
    }
    if (not isIso) continue;

    promptMuonCandidates.push_back(muon);
  }
}

void DerivationFramework::HnlSkimmingTool::getDisplacedMuonCandidates(SG::ReadHandle<DataVector<xAOD::Muon_v1>>& muons,
                                                                      std::vector<const xAOD::Muon*>& displacedMuonCandidates) const
{
  for (const xAOD::Muon* muon : *muons) {
    // pT cut
    if (not (muon->pt() > m_mu2PtMin)) continue;

    // eta cut
    if (not (std::abs(muon->eta()) < m_mu2AbsEtaMax)) continue;

    // type cut
    bool passTypeCut = true;
    int type = muon->muonType();
    if (m_mu2Types.size() > 0) {
      passTypeCut = false;
      for (const int& allowedType : m_mu2Types) {
        if (type==allowedType) {
          passTypeCut = true;
          break;
        }
      }
    }
    if (not passTypeCut) continue;

    // isolation cut
    bool isIso = false;
    float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu2IsoType);
    if (not m_mu2IsoCutIsRel) {
      if (muon->isolation(isoValue, isoType) and (isoValue < m_mu2IsoCut)) isIso = true;
    } else {
      if (muon->isolation(isoValue, isoType) and isoValue/muon->pt() < m_mu2IsoCut) isIso = true;
    }
    if (not isIso) continue;

    // d0 cut
    bool passD0cut = true;
    if (type==xAOD::Muon::Combined) { // d0 cut is for combined muons only
      passD0cut = false;
      if (isGood(*muon)) { // if muon has a good chi2/dof
        if (std::abs(muon->primaryTrackParticle()->d0()) > m_mu2d0Min) passD0cut = true;
      } else {
        passD0cut = true;
      }
    }
    if (not passD0cut) continue;

    displacedMuonCandidates.push_back(muon);
  }
}

void DerivationFramework::HnlSkimmingTool::getPromptElectronCandidates(SG::ReadHandle<DataVector<xAOD::Electron_v1>>& electrons,
                                                                       std::vector<const xAOD::Electron*>& promptElectronCandidates) const
{
  for (const xAOD::Electron* electron : *electrons) {
    // pT cut
    if (not (electron->pt() > m_el1PtMin)) continue;

    // eta cut
    if (not (std::abs(electron->eta()) < m_el1AbsEtaMax)) continue;

    // eID cut
    bool passEID = false;
    if (not electron->passSelection(passEID, m_el1IDKey)) continue;
    if (not passEID) continue;

    // isolation cut
    bool isIso = false;
    float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_el1IsoType);
    if (not m_el1IsoCutIsRel) {
      if (electron->isolation(isoValue, isoType) and (isoValue < m_el1IsoCut)) isIso = true;
    } else {
      if (electron->isolation(isoValue, isoType) and (isoValue/electron->pt() < m_el1IsoCut)) isIso = true;
    }
    if (not isIso) continue;

    promptElectronCandidates.push_back(electron);
  }
}

void DerivationFramework::HnlSkimmingTool::getDisplacedElectronCandidates(SG::ReadHandle<DataVector<xAOD::Electron_v1>>& electrons,
                                                                          std::vector<const xAOD::Electron*>& displacedElectronCandidates) const
{
  for (const xAOD::Electron* electron : *electrons) {
    // pT cut
    if (not (electron->pt() > m_el2PtMin)) continue;

    // eta cut
    if (not (std::abs(electron->eta()) < m_el2AbsEtaMax)) continue;

    // isolation cut
    bool isIso = false;
    float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_el2IsoType);
    if (not m_el2IsoCutIsRel) {
      if (electron->isolation(isoValue, isoType) and (isoValue < m_el2IsoCut)) isIso = true;
    } else {
      if (electron->isolation(isoValue, isoType) and isoValue/electron->pt() < m_el2IsoCut) isIso = true;
    }
    if (not isIso) continue;

    // d0 cut
    bool passD0cut = false;
    if (std::abs(electron->trackParticle()->d0()) > m_el2d0Min) passD0cut = true;
    if (not passD0cut) continue;

    displacedElectronCandidates.push_back(electron);
  }
}

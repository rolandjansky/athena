/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TruthJetFilter.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "ParticleEvent/ParticleBaseContainer.h"
#include "xAODJet/JetContainer.h"
#include "TMath.h"

TruthJetFilter::TruthJetFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("Njet",m_Njet = -1.);
  declareProperty("NjetMinPt",m_NjetMinPt = 40*Gaudi::Units::GeV);
  declareProperty("NjetMaxEta",m_NjetMaxEta = 2.);
  declareProperty("jet_pt1",m_jet_pt1 = 80.*Gaudi::Units::GeV);
  declareProperty("applyDeltaPhiCut",m_applyDeltaPhiCut = false);
  declareProperty("MinDeltaPhi",m_MinDeltaPhi = 0.2);
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt4TruthJets");
}


StatusCode TruthJetFilter::filterInitialize() {
  ATH_MSG_INFO("Njet=" << m_Njet);
  ATH_MSG_INFO("NjetMinPt=" << m_NjetMinPt);
  ATH_MSG_INFO("NjetMaxEta=" << m_NjetMaxEta);
  ATH_MSG_INFO("jet_pt1=" << m_jet_pt1);
  ATH_MSG_INFO("applyDeltaPhiCut=" << m_applyDeltaPhiCut);
  if (m_applyDeltaPhiCut) ATH_MSG_INFO("MinDeltaPhi=" << m_MinDeltaPhi);
  ATH_MSG_INFO("xAOD::JetContainer=" << m_TruthJetContainerName);
  return StatusCode::SUCCESS;
}


StatusCode TruthJetFilter::filterEvent() {
  const xAOD::JetContainer* truthjetTES;
  CHECK(evtStore()->retrieve(truthjetTES, m_TruthJetContainerName));
  ATH_MSG_INFO("xAOD::JetContainer size = " << (*truthjetTES).size());

  int Njet=0;
  int Njet_pt1=0;
  std::vector<const xAOD::Jet*> listOfSelectedJets;
  const xAOD::Jet* leadingJet = 0;
  for (xAOD::JetContainer::const_iterator it_truth = (*truthjetTES).begin(); it_truth != (*truthjetTES).end() ; ++it_truth) {
    if ( (*it_truth)->pt() > m_NjetMinPt && std::abs( (*it_truth)->eta() ) < m_NjetMaxEta ) {
      Njet++;
      ATH_MSG_INFO("Jet pt " << (*it_truth)->pt()/Gaudi::Units::GeV);
      if (m_applyDeltaPhiCut) listOfSelectedJets.push_back(*it_truth);
    }
    if ( (*it_truth)->pt() > m_jet_pt1 && std::abs( (*it_truth)->eta() ) < m_NjetMaxEta ) {
      Njet_pt1++;
      ATH_MSG_INFO("High jet pt " << (*it_truth)->pt()/Gaudi::Units::GeV);
      if (m_applyDeltaPhiCut && (!leadingJet || (*it_truth)->pt() > leadingJet->pt())) {
        leadingJet = (*it_truth);
      }
    }
  }

  if (Njet >= m_Njet && Njet_pt1 > 0) {
    // Compute deltaPhi(leadingjet, jet)
    bool passDeltaPhi = true;
    if (m_applyDeltaPhiCut) {
      for (unsigned int iJet = 0; iJet < m_Njet; iJet++) {
        if (listOfSelectedJets[iJet] == leadingJet) continue;
        double deltaPhi = leadingJet->p4().DeltaPhi((listOfSelectedJets[iJet])->p4());
        double dPi = TMath::Pi() - std::abs(deltaPhi);
        ATH_MSG_DEBUG("deltaPhi = " << deltaPhi << ", dPi = " << dPi <<
                      " between leading jet(pt=" << leadingJet->pt() << ",eta=" << leadingJet->eta() <<
                      ") and jet(pt=" << (listOfSelectedJets[iJet])->pt() <<
                      ",eta=" << (listOfSelectedJets[iJet])->eta() << ")");
        if (dPi < m_MinDeltaPhi) passDeltaPhi = false;
      }
    }

    if (!m_applyDeltaPhiCut || passDeltaPhi) {
      return StatusCode::SUCCESS;
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}

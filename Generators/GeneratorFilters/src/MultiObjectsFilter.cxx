/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MultiObjectsFilter.h"
#include "xAODJet/JetContainer.h"
#include "GaudiKernel/SystemOfUnits.h"

MultiObjectsFilter::MultiObjectsFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator),
    m_PtCut(10.*Gaudi::Units::GeV),
    m_EtaCut(2.7),
    m_jetPtCut(10.*Gaudi::Units::GeV),
    m_jetEtaCut(5.2),
    m_sumPtCut(2000.*Gaudi::Units::GeV),
    m_useEle(false),
    m_useMuo(true),
    m_usePho(false),
    m_useJet(true),
    m_useSumPt(true),
    m_TruthJetContainerName("AntiKt4TruthJets")
    // Electrons and Photons are calculated in TruthJet.
    // You should set useEle=false,usePho=false when useJet=true
{
  declareProperty("PtCut",m_PtCut);
  declareProperty("EtaCut",m_EtaCut);
  declareProperty("JetPtCut",m_jetPtCut);
  declareProperty("JetEtaCut",m_jetEtaCut);
  declareProperty("UseSumPt",m_useSumPt);
  declareProperty("SumPtCut",m_sumPtCut);
  declareProperty("PtCutEach",m_PtCutEach);
  declareProperty("UseEle",m_useEle);
  declareProperty("UseMuo",m_useMuo);
  declareProperty("UsePho",m_usePho);
  declareProperty("UseJet",m_useJet);
  declareProperty("TruthJetContainer", m_TruthJetContainerName);
}


StatusCode MultiObjectsFilter::filterInitialize() {
  ATH_MSG_INFO("PtCut=" << m_PtCut/Gaudi::Units::GeV  << "GeV");
  ATH_MSG_INFO("EtaCut=" << m_EtaCut);
  ATH_MSG_INFO("JetPtCut=" << m_jetPtCut/Gaudi::Units::GeV  << "GeV");
  ATH_MSG_INFO("JetEtaCut=" << m_jetEtaCut);
  if (m_useEle) ATH_MSG_INFO("use electrons");
  if (m_useMuo) ATH_MSG_INFO("use muons");
  if (m_usePho) ATH_MSG_INFO("use photons");
  if (m_useJet) ATH_MSG_INFO("Using jets: jet container name = " << m_TruthJetContainerName);
  ATH_MSG_INFO("NPar=" << m_PtCutEach.size());
  for (size_t i = 0; i < m_PtCutEach.size(); ++i) {
    ATH_MSG_INFO("PtCut " << i << "=" << m_PtCutEach[i]/Gaudi::Units::GeV << "GeV");
  }
  if (m_useSumPt) ATH_MSG_INFO("SumPtCut=" << m_sumPtCut/Gaudi::Units::GeV << "GeV");

  return StatusCode::SUCCESS;
}


StatusCode MultiObjectsFilter::filterEvent() {
  std::vector <double> pt;
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto part: *genEvt) {
      if (part->status()==1 && part->momentum().perp() > m_PtCut && std::abs(part->momentum().pseudoRapidity()) < m_EtaCut) {
        //electrons
        if (m_useEle && abs(part->pdg_id()) == 11) {
          ATH_MSG_DEBUG("Found electron: PT,ETA,PHI " << part->momentum().perp()/Gaudi::Units::GeV << "GeV, " << part->momentum().pseudoRapidity() << " " << part->momentum().phi());
          pt.push_back(part->momentum().perp());
        //muons
        } else if (m_useMuo && abs(part->pdg_id()) == 13) {
          ATH_MSG_DEBUG("Found muon: PT,ETA,PHI " << part->momentum().perp()/Gaudi::Units::GeV << "GeV, " << part->momentum().pseudoRapidity() << " " << part->momentum().phi());
          pt.push_back(part->momentum().perp());
        //photons
        } else if (m_usePho && abs(part->pdg_id()) == 22) {
          ATH_MSG_DEBUG("Found photon: PT,ETA,PHI " << part->momentum().perp()/Gaudi::Units::GeV << "GeV, " << part->momentum().pseudoRapidity() << " " << part->momentum().phi());
          pt.push_back(part->momentum().perp());
        }
      }
    }
  }

  if (m_useJet) {
    const xAOD::JetContainer* truthjetTES;
    CHECK(evtStore()->retrieve(truthjetTES, m_TruthJetContainerName));
    ATH_MSG_DEBUG("xAOD::JetContainer Size = " << truthjetTES->size());

    for (xAOD::JetContainer::const_iterator jitr = truthjetTES->begin(); jitr != truthjetTES->end(); ++jitr) {
      if ((*jitr)->pt() > m_jetPtCut && std::abs((*jitr)->eta()) < m_jetEtaCut) {
        ATH_MSG_DEBUG("Found jet: pT, eta, phi = " << (*jitr)->pt()/Gaudi::Units::GeV << "GeV, " << (*jitr)->eta() << " " << (*jitr)->phi());
        pt.push_back((*jitr)->pt());
      }
    }
  }

  // Sort pTs
  std::sort(pt.begin(), pt.end(), std::greater<double>());

  // Check pt
  int ptFlag = true;
  if (m_PtCutEach.size() == 0) {
    ptFlag = false;
  } else {
    ATH_MSG_DEBUG("Number of particles = " << pt.size());
    for (size_t i = 0; i < pt.size() && i < m_PtCutEach.size(); ++i) {
      ATH_MSG_DEBUG(" " << pt[i]/Gaudi::Units::GeV << "GeV");
    }

    if (pt.size() < m_PtCutEach.size()) {
      ptFlag = false;
    } else {
      for (size_t i = 0; i < m_PtCutEach.size(); ++i) {
        if (pt[i] < m_PtCutEach[i]) {
          ptFlag = false;
          break;
        }
      }
    }
    }

  // Check Sum Pt
  int sumPtFlag = true;
  if (!m_useSumPt) {
    sumPtFlag = false;
  } else {
    double sumPt = 0;
    for (size_t i =0; i < pt.size(); ++i) sumPt += pt[i];
    ATH_MSG_DEBUG("SumPt=" << sumPt/Gaudi::Units::GeV << "GeV");
    if (sumPt < m_sumPtCut) sumPtFlag = false;
  }

  // Check flags
  if (ptFlag || sumPtFlag) {
    ATH_MSG_DEBUG("Passed MultiObjects Filter");
    return StatusCode::SUCCESS;
  }

  // If we get here we have failed
  ATH_MSG_DEBUG("Failed MultiObjects Filter");
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}

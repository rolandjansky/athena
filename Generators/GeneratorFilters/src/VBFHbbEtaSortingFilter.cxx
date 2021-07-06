/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: src/VBFHbbEtaSortingFilter.cxx
// Description: see VBFHbbEtaSortingFilter.h
//
//
// AuthorList:         
//   Yasuyuki Okumura (yasuyuki.okumura@cern.ch)

// Header for this module:-

#include "GeneratorFilters/VBFHbbEtaSortingFilter.h"
#include "GaudiKernel/SystemOfUnits.h"

// Other classes used by this class:-
#include <math.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include "xAODJet/JetContainer.h"

#include <stdio.h>

//--------------------------------------------------------------------------
VBFHbbEtaSortingFilter::VBFHbbEtaSortingFilter(const std::string& name, ISvcLocator* pSvcLocator): 
  GenFilter(name,pSvcLocator), m_total(0), m_passed_multiplicity(0), m_passed_all(0)
{
  declareProperty("Debug", m_debug = false);
  declareProperty("MinJetPt", m_MinJetPt = 20*Gaudi::Units::GeV); 
  declareProperty("MaxJetEta", m_MaxJetEta = 2.6); 
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt4TruthJets");
  declareProperty("JetTruthLabel", m_JetTruthLabelName="PartonTruthLabelID");
}

//--------------------------------------------------------------------------
 VBFHbbEtaSortingFilter::~VBFHbbEtaSortingFilter(){
   
}

//---------------------------------------------------------------------------
StatusCode VBFHbbEtaSortingFilter::filterInitialize() {
  msg(MSG:: INFO) << "VBFHbbEtaSortingFilter INITIALISING "  << endmsg;   
  msg(MSG:: INFO) << "m_MinJetPt    = " << m_MinJetPt << endmsg;
  msg(MSG:: INFO) << "m_MaxJetEta   = " << m_MaxJetEta << endmsg;  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode VBFHbbEtaSortingFilter::filterFinalize() {
  msg(MSG:: INFO) << m_passed_all << " events passed out of " << m_total 
		  << " events : efficiency: " << 100.*double(m_passed_all)/double(m_total) << "%" << endmsg;
  msg(MSG:: INFO) << "      for multiplicity requirement " << m_passed_multiplicity << " events passed" << endmsg;  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode VBFHbbEtaSortingFilter::filterEvent() {
  if (m_debug) printf("dbg> event start %10ld / %10ld / %10ld @ %d \n", 
		      m_passed_all, m_passed_multiplicity, m_total, __LINE__);
  
  m_total++;
  
  const DataHandle<xAOD::JetContainer> truthjetTES = 0;
  if (!evtStore()->contains<xAOD::JetContainer>(m_TruthJetContainerName) ||  
      evtStore()->retrieve(truthjetTES, m_TruthJetContainerName).isFailure() || !truthjetTES) {
    ATH_MSG_ERROR("No xAOD::JetContainer found in StoreGate with key " << m_TruthJetContainerName);
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  
  // step (1)
  // require four jets
  std::multimap<double, const xAOD::Jet*, std::greater<double> > jets_ptordering; // high pT to small pT

  for (const xAOD::Jet* jet : *truthjetTES) {
    if (TMath::Abs( jet->eta() ) > m_MaxJetEta) continue;
    if (jet->pt() < m_MinJetPt)  continue;
    
    jets_ptordering.insert(std::pair<double, const xAOD::Jet*> (jet->pt(), jet));
  }  
  
  if (m_debug) printf("dbg> %d @ %d \n", (int)jets_ptordering.size(), __LINE__);
  
  if (jets_ptordering.size() < 4) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  
  m_passed_multiplicity++;
  
  // step (2)
  // consider eta sorting with the leading four jets
  std::multimap<double, const xAOD::Jet*> four_jets_etaordering;
  std::multimap<double, const xAOD::Jet*>::const_iterator ite_c;
  std::multimap<double, const xAOD::Jet*>::const_iterator ite_e;
  ite_c = jets_ptordering.begin();
  ite_e = jets_ptordering.end();
  
  for (int iJet=0; ite_c!=ite_e and iJet<4; ++ite_c, ++iJet) {
    four_jets_etaordering.insert(std::pair<double, const xAOD::Jet*> ( (ite_c->second)->eta(), (ite_c->second)));
    if (m_debug) printf("dbg> ijet=%2d %10.1f @ %d \n", iJet, (ite_c->second)->pt(), __LINE__);
  }
  
  ite_c = four_jets_etaordering.begin();
  const xAOD::Jet* forward_jet_1 = (ite_c)->second; ++ite_c; // 1st jet in eta ordering
  const xAOD::Jet* central_jet_1 = (ite_c)->second; ++ite_c; // 2nd jet in eta ordering
  const xAOD::Jet* central_jet_2 = (ite_c)->second; ++ite_c; // 3rd jet in eta ordering
  const xAOD::Jet* forward_jet_2 = (ite_c)->second;          // 4th jet in eta ordering

  int forward_jet_truth_label_1 = 0;
  forward_jet_1->getAttribute(m_JetTruthLabelName, forward_jet_truth_label_1);  
  int central_jet_truth_label_1 = 0;
  central_jet_1->getAttribute(m_JetTruthLabelName, central_jet_truth_label_1);
  int central_jet_truth_label_2 = 0;
  central_jet_2->getAttribute(m_JetTruthLabelName, central_jet_truth_label_2);
  int forward_jet_truth_label_2 = 0;
  forward_jet_2->getAttribute(m_JetTruthLabelName, forward_jet_truth_label_2);
  
  if (m_debug) printf("dbg> eta1=%5.1f (%3d) eta2=%5.1f (%3d)"
		      "eta3=%5.1f (%3d) eta4=%5.1f (%3d) @ %d \n", 
		      forward_jet_1->eta(), forward_jet_truth_label_1,
		      central_jet_1->eta(), central_jet_truth_label_1,
		      central_jet_2->eta(), central_jet_truth_label_2,
		      forward_jet_2->eta(), forward_jet_truth_label_2,
		      __LINE__);
  
  if ( not (TMath::Abs(central_jet_truth_label_1)==5 and TMath::Abs(central_jet_truth_label_2)==5) ) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  
  if (m_debug) printf("dbg> pass all selection @ %d \n", __LINE__);
  m_passed_all++;
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}



/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
// This is a general-purpose multi-c-jet filter with the removal of the 
// c-hadrons orriginating from b-hadrons decay.
// It can cut on:
//    - Multiplicity of c-jets (both min and max can be specified)
//    - Multiplicity of jets (regardless of flavor)
//    - The pT of the leading jet
//
// Written by Dominik Derendarz (dominik.derendarz@cern.ch) based on MultiBjetFilter

#include <math.h>
#include <fstream>

// Header for this module:-
#include "GeneratorFilters/xAODMultiCjetFilter.h"

// Other classes used by this class:-
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODJet/JetContainer.h"
#include "McParticleEvent/TruthParticle.h"
#include "CxxUtils/BasicTypes.h"
#include "TLorentzVector.h"



xAODMultiCjetFilter::xAODMultiCjetFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator){

  // Local Member Data:-

  m_NPass = 0;
  m_Nevt = 0;
  m_SumOfWeights_Pass = 0;
  m_SumOfWeights_Evt = 0;
}

xAODMultiCjetFilter::~xAODMultiCjetFilter(){}

StatusCode xAODMultiCjetFilter::filterInitialize() {

  m_Nevt = 0;
  m_NPass = 0;
  m_SumOfWeights_Pass = 0;
  m_SumOfWeights_Evt = 0;

  ATH_MSG_INFO("Initialized");
  return StatusCode::SUCCESS;
}

StatusCode xAODMultiCjetFilter::filterFinalize() {

  ATH_MSG_INFO(m_NPass << " Events out of " << m_Nevt << " passed the filter");
  ATH_MSG_INFO(m_SumOfWeights_Pass << " out of " << m_SumOfWeights_Evt << " SumOfWeights counter, passed/total" );
  return StatusCode::SUCCESS;
}


StatusCode xAODMultiCjetFilter::filterEvent() {

  bool pass = true;
  m_Nevt++;

  // Retrieve truth jets
  const xAOD::JetContainer* truthjetTES = 0;
  StatusCode sc = evtStore()->retrieve(truthjetTES, m_TruthJetContainerName);
  if(sc.isFailure() || !truthjetTES) {
    ATH_MSG_INFO("No xAOD::JetContainer found in TDS " << m_TruthJetContainerName \
      << sc.isFailure() << " "<<   !truthjetTES );
    return StatusCode::SUCCESS;
  }

  xAOD::JetContainer::const_iterator jitr;
  double lead_jet_pt = 0.0;

  // Select jets according to kinematic cuts, record leading jet pt
  std::vector<xAOD::JetContainer::const_iterator> jets,cjets_all,cjets;
  for(jitr = (*truthjetTES).begin(); jitr !=(*truthjetTES).end(); ++jitr) {
    if((*jitr)->pt() < m_jetPtMin) continue;
    if(std::abs((*jitr)->eta()) > m_jetEtaMax) continue;
    if((*jitr)->pt() > lead_jet_pt) lead_jet_pt = (*jitr)->pt();
    jets.push_back(jitr);
  }

  // Apply leading jet pt cut
  if(lead_jet_pt < m_leadJet_ptMin || (lead_jet_pt > m_leadJet_ptMax && m_leadJet_ptMax > 0)) {
    pass = false;
    }

  // Apply jet multiplicity cut
  int njets = jets.size();
  if(njets < m_nJetsMin) pass = false;
  if(njets > m_nJetsMax && m_nJetsMax > 0) pass = false;
  int cJetCounter = 0;
  McEventCollection::const_iterator itr;

    // Retrieve full TruthEventContainer container
   const xAOD::TruthEventContainer *xTruthEventContainer=NULL;
   if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
   {
  ATH_MSG_ERROR("No TruthEvent collection with name " << "TruthEvents" << " found in StoreGate!");
    return StatusCode::FAILURE;
  }
    
        // Make a vector containing all the event's b-hadrons
        std::vector< const xAOD::TruthParticle* > bHadrons;
  for ( xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr) {
       unsigned int nPart = (*itr)->nTruthParticles();
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);

      if( !isBwithWeakDK( part->absPdgId()) ) continue;
      if( part->pt() < m_bottomPtMin ) continue;
      if( std::abs( part->abseta() ) > m_bottomEtaMax) continue;
      bHadrons.push_back(part);
    }
}
    // Make a vector containing all the event's c-hadrons
    std::vector< const xAOD::TruthParticle* > cHadrons;
  for ( xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr) {
       unsigned int nPart = (*itr)->nTruthParticles();
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);

      if( !isCwithWeakDK( part->absPdgId()) ) continue;
      if( part->pt() < m_bottomPtMin ) continue;
      if( std::abs( part->abseta() ) > m_bottomEtaMax) continue;
      cHadrons.push_back(part);
    }
}
    // Count how many truth jets contain c-hadrons
    for(uint i = 0; i < jets.size(); i++){
      for(uint j = 0; j < cHadrons.size(); j++){
        TLorentzVector genpart(cHadrons.at(j)->px(), cHadrons.at(j)->py(), cHadrons.at(j)->pz(), cHadrons.at(j)->e());
	double dR = (*jets[i])->p4().DeltaR(genpart);
	if(dR<m_deltaRFromTruth){
	  cjets_all.push_back(jets[i]);
	  break;
	} 
      } 
    } 
    
    
    // check if cjets contian b-hadron 
    // select only those withouty b-hadron 
    // @todo - removal based on parrent  
    
    for(uint i = 0; i < cjets_all.size(); i++){
      bool b_inside_cjet = false; 
      for(uint j = 0; j < bHadrons.size(); j++){
        TLorentzVector genpart(bHadrons.at(j)->px(), bHadrons.at(j)->py(), bHadrons.at(j)->pz(), bHadrons.at(j)->e());
        double dR = (*cjets_all[i])->p4().DeltaR(genpart);
        if(dR<m_deltaRFromTruth){
	  b_inside_cjet = true;        
          break;
        }
      }
      // only those without b-hadron
      if(!b_inside_cjet){ 
	cjets.push_back(cjets_all[i]);
	cJetCounter++;
      }
    }
  //}

  // Apply c-jet multiplicity cut
  if(cJetCounter < m_nCJetsMin) pass = false;
  if(cJetCounter > m_nCJetsMax && m_nCJetsMax > 0) pass = false;

  // Bookkeeping
  double weight = 1;
 for(itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
  const HepMC::GenEvent* genEvt = (*itr);
 weight = genEvt->weights().front();
}

  m_SumOfWeights_Evt += weight;
  if(pass){
    m_NPass++;
    m_SumOfWeights_Pass += weight;
  }

  setFilterPassed(pass);
  return StatusCode::SUCCESS;
}

bool xAODMultiCjetFilter::isBwithWeakDK(const int pID) const
{ 
  int id = std::abs(pID);
  return ( id == 511   ||    // B+
	   id == 521   ||    // B0
	   id == 531   ||    // Bs
	   id == 541   ||    // Bc
	   id == 5122  ||    // Lambda_B
	   id == 5132  ||    // Xi_b-
	   id == 5232  ||    // X_b0
	   id == 5112  ||    // Sigma_b-
	   id == 5212  ||    // Sigma_b0
	   id == 5222 );     // Sigma_b+
}

bool xAODMultiCjetFilter::isCwithWeakDK(const int pID) const
{
  int id = std::abs(pID);
  return ( id == 411   || // D+
	   id == 421   || // D0
	   id == 431   || // Ds
	   id == 4122  || // Lambda_C
	   id == 4132  || // Xi_C^0
	   id == 4232  || // Xi_C^+
	   id == 4212  || // Xi_C^0
	   id == 4322  || // Xi'_C+  This is in fact EM not weak
	   id == 4332);    // Omega_C
}


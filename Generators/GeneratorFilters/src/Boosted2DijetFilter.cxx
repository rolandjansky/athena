/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/Boosted2DijetFilter.cxx
// Description:
//  Designed to filter for a pair of 2 jets that are close-by 
//  and form a high pT dijet each. 
//
// AuthorList:
// S. Bieniek, L. Lambourne

// Header for this module:-
#include "GeneratorFilters/Boosted2DijetFilter.h"

// Framework Related Headers:-

// Other classes used by this class:-
#include <math.h>
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"
#include "McParticleEvent/TruthParticle.h"
#include "CxxUtils/BasicTypes.h"
#include "TLorentzVector.h"

//--------------------------------------------------------------------------
Boosted2DijetFilter::Boosted2DijetFilter(const std::string& name, 
			   ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //--------------------------------------------------------------------------    
  // Local Member Data:-
  declareProperty("DijetPtMin",m_dijet_ptMin=160/Gaudi::Units::GeV);
  declareProperty("JetPtMin",m_jetPtMin=20.0/Gaudi::Units::GeV);
  declareProperty("LargeRJetPtMin",m_akt8PtMin=160.0/Gaudi::Units::GeV);
  declareProperty("JetEtaMax",m_jetEtaMax=2.8);
  declareProperty("DijetDRMax",m_dijet_dRMax=1.5);
  declareProperty("TruthContainerName",m_TruthJetContainerName="AntiKt4TruthJets");
  declareProperty("TruthContainerName_largeR",m_TruthJetContainerName_akt8="AntiKt8TruthJets");

  m_NPass = 0;
  m_Nevt = 0;
  m_SumOfWeigths_Pass = 0;
  m_SumOfWeigths_Evt = 0;
}

//--------------------------------------------------------------------------
 Boosted2DijetFilter::~Boosted2DijetFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode Boosted2DijetFilter::filterInitialize() {
//---------------------------------------------------------------------------
  m_Nevt = 0;
  m_NPass = 0;
  m_SumOfWeigths_Pass = 0;
  m_SumOfWeigths_Evt = 0;

  ATH_MSG_INFO ( "Initialized" );
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode Boosted2DijetFilter::filterFinalize() {
//---------------------------------------------------------------------------

  ATH_MSG_INFO(m_NPass << " Events out of " << m_Nevt << " passed the filter");
  ATH_MSG_INFO(m_SumOfWeigths_Pass << " out of " << m_SumOfWeigths_Evt << " SumOfWeights counter, passed/total");
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode Boosted2DijetFilter::filterEvent() {
//---------------------------------------------------------------------------

  bool pass = false;
  m_Nevt++;

  const xAOD::JetContainer* truthjetTES = 0;
  StatusCode sc=evtStore()->retrieve( truthjetTES, m_TruthJetContainerName);
  if( sc.isFailure()  ||  !truthjetTES ) {
    ATH_MSG_WARNING(
	 "No xAOD::JetContainer found in TDS " << m_TruthJetContainerName \
	<< sc.isFailure() << " "<<   !truthjetTES);
	
    return StatusCode::SUCCESS;
  }

  //getting the akt8 jets
  const xAOD::JetContainer* truthjetTES_akt8 = 0;
  StatusCode sc_akt8=evtStore()->retrieve( truthjetTES_akt8, m_TruthJetContainerName_akt8);
  if( sc.isFailure()  ||  !truthjetTES_akt8 ) {
    ATH_MSG_WARNING(
	 "No xAOD::JetContainer found in TDS " << m_TruthJetContainerName_akt8 \
	 << sc_akt8.isFailure() << " "<<   !truthjetTES_akt8);
    return StatusCode::SUCCESS;
  }

  xAOD::JetContainer::const_iterator jitr;
  std::vector<xAOD::JetContainer::const_iterator> jets;
  xAOD::JetContainer::const_iterator jitr_akt8;
  std::vector<xAOD::JetContainer::const_iterator> jets_akt8;
  std::vector<TLorentzVector> dijets;
  
  //getting a collection of truth jets which pass some cuts 
  for (jitr = (*truthjetTES).begin(); jitr !=(*truthjetTES).end(); ++jitr) { 
    if( (*jitr)->pt() < m_jetPtMin ) continue;
    if( std::abs( (*jitr)->eta() ) > m_jetEtaMax ) continue;
    jets.push_back(jitr);
  }

  //getting the akt8 truth jets 
  for (jitr_akt8 = (*truthjetTES_akt8).begin(); jitr_akt8 !=(*truthjetTES_akt8).end(); ++jitr_akt8) { 
    if( (*jitr_akt8)->pt() < m_akt8PtMin ) continue;
    if( std::abs( (*jitr_akt8)->eta() ) > m_jetEtaMax ) continue;
    jets_akt8.push_back(jitr_akt8);
  }
  
  double weight = 1;
  m_SumOfWeigths_Evt += weight;
  std::vector<int> jetIDs;

  //Step 1: look for two unique akt4 dijets
  if(jets.size() > 2){
    //now we have jets, try to form dijets
    TLorentzVector temp_dijet;
    for(uint k =0; k < jets.size(); k++){
      for(uint l =k+1; l < jets.size(); l++){
        if(jets[k] == jets[l]) continue;
        if((*jets[k])->p4().DeltaR((*jets[l])->p4()) < m_dijet_dRMax){
          temp_dijet = (*jets[k])->p4() + (*jets[l])->p4();
          if(temp_dijet.Pt() > m_dijet_ptMin){
            dijets.push_back(temp_dijet);
            jetIDs.push_back(k);
            jetIDs.push_back(l);
          }
        }    
      }
    }

    bool has2Unique = false;
    if(dijets.size() > 1){
      //check that there are two unique dijets
      for(unsigned int m=0; m<jetIDs.size(); m+=2){
        if(has2Unique) break;
        for(unsigned int n=m+2; n<jetIDs.size(); n+=2){
          if(jetIDs[m] != jetIDs[n] && jetIDs[m] != jetIDs[n+1] && jetIDs[m+1] != jetIDs[n] && jetIDs[n] != jetIDs[n+1]){
            has2Unique = true;
            break;
          }
        } 
      }
    }

    if(has2Unique){
      m_NPass++;
      m_SumOfWeigths_Pass += weight;
      setFilterPassed(true);
      return StatusCode::SUCCESS;
    }
  }
  //Step 2: look for 2 akt8 jets
  if(jets_akt8.size() > 1){
      m_NPass++;
      m_SumOfWeigths_Pass += weight;
      setFilterPassed(true);
      return StatusCode::SUCCESS;
  }else if(jets_akt8.size() > 0 && dijets.size() > 0){
    //Step 3: look for unique dijet + akt8 jet pair
    for(unsigned int i=0;i<dijets.size();i++){
      if(dijets[i].DeltaR((*jets_akt8[0])->p4()) < 0.8){ pass = true;}
    }

  }
  if(pass){ 
    m_NPass++;
    m_SumOfWeigths_Pass += weight;
  }
  setFilterPassed(pass);
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/DiBjetFilter.cxx
// Description:
// This is a multi-purpose di-b-jet filter. It cuts on:
//    - The presence of two b-hadron match jets in the end
//    - The pT of the leading jet
//    - It can also pass light jets and reweight the event accordingly to make an 
//      statisticaly inclusive sample of jets but with baised towards 
//      heavy flavour events
//
// AuthorList:
// S. Bieniek

// Header for this module:-
#include "GeneratorFilters/DiBjetFilter.h"

// Other classes used by this class:-
#include <math.h>
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODJet/JetContainer.h"
#include "McParticleEvent/TruthParticle.h"
#include "CxxUtils/BasicTypes.h"
#include "TRandom3.h"
#include "TLorentzVector.h"


using HepMC::GenVertex;
using HepMC::GenParticle;
//--------------------------------------------------------------------------
DiBjetFilter::DiBjetFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  //--------------------------------------------------------------------------    
  // Local Member Data:-
  declareProperty("LeadJetPtMin",m_leadJet_ptMin=0/Gaudi::Units::GeV);
  declareProperty("LeadJetPtMax",m_leadJet_ptMax=50000/Gaudi::Units::GeV);
  declareProperty("BottomPtMin",m_bottomPtMin=5.0/Gaudi::Units::GeV);
  declareProperty("BottomEtaMax",m_bottomEtaMax=3.0);
  declareProperty("JetPtMin",m_jetPtMin=15.0/Gaudi::Units::GeV);
  declareProperty("JetEtaMax",m_jetEtaMax=2.7);
  declareProperty("DeltaRFromTruth",m_deltaRFromTruth=0.3);
  declareProperty("TruthContainerName",m_TruthJetContainerName="AntiKt4TruthJets");
  declareProperty("LightJetSuppressionFactor",m_LightJetSuppressionFactor=10);
  declareProperty("AcceptSomeLightEvents",m_AcceptSomeLightEvents=false);

  m_NPass = 0;
  m_Nevt = 0;
  m_SumOfWeigths_Pass = 0;
  m_SumOfWeigths_Evt = 0;
  m_ranNumGen = 0;
}

//--------------------------------------------------------------------------
 DiBjetFilter::~DiBjetFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode DiBjetFilter::filterInitialize() {
//---------------------------------------------------------------------------

  m_Nevt = 0;
  m_NPass = 0;
  m_SumOfWeigths_Pass = 0;
  m_SumOfWeigths_Evt = 0;

  if(m_AcceptSomeLightEvents){
    m_ranNumGen = new TRandom3();
    /* Set seed with respect to computer clock time */
    m_ranNumGen->SetSeed(0);
  }
  msg(MSG::INFO) << "Initialized" << endmsg;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode DiBjetFilter::filterFinalize() {
//---------------------------------------------------------------------------

  if(m_AcceptSomeLightEvents){
    delete m_ranNumGen;
  }
  msg(MSG::INFO) <<  m_NPass << " Events out of " << m_Nevt << " passed the filter" << endmsg;
  msg(MSG::INFO) <<  m_SumOfWeigths_Pass << " out of " << m_SumOfWeigths_Evt << " SumOfWeights counter, passed/total" << endmsg;
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode DiBjetFilter::filterEvent() {
//---------------------------------------------------------------------------

  bool pass = false;
  m_Nevt++;

  const xAOD::JetContainer* truthjetTES = 0;
  StatusCode sc=evtStore()->retrieve( truthjetTES, m_TruthJetContainerName);
  if( sc.isFailure()  ||  !truthjetTES ) {
    msg(MSG::WARNING)
	<< "No xAOD::JetContainer found in TDS " << m_TruthJetContainerName \
	<< sc.isFailure() << " "<<   !truthjetTES
	<< endmsg;
    return StatusCode::SUCCESS;
  }

  bool passLeadJetCut = false;
  xAOD::JetContainer::const_iterator jitr;
  double lead_jet_pt = 0.0;
  std::vector<xAOD::JetContainer::const_iterator> jets;
  for (jitr = (*truthjetTES).begin(); jitr !=(*truthjetTES).end(); ++jitr) { 
    if( (*jitr)->pt() > lead_jet_pt ){
       lead_jet_pt = (*jitr)->pt();
    }
    if( (*jitr)->pt() < m_jetPtMin ) continue;
    if( fabs( (*jitr)->eta() ) > m_jetEtaMax ) continue;
    jets.push_back(jitr);
  }

  if( lead_jet_pt > m_leadJet_ptMin && lead_jet_pt < m_leadJet_ptMax ) passLeadJetCut = true;

  int bJetCounter = 0; 
  double weight = 1;
  for (const HepMC::GenEvent* genEvt : *events()) {
    weight = genEvt->weights().front();
    HepMC::GenEvent::particle_const_iterator pitr;
    std::vector< HepMC::GenEvent::particle_const_iterator > bHadrons;
    for(pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr ) {  
      if( !isBwithWeakDK( (*pitr)->pdg_id()) ) continue;
      if( (*pitr)->momentum().perp() < m_bottomPtMin ) continue;
      if( fabs( (*pitr)->momentum().pseudoRapidity() ) > m_bottomEtaMax) continue;
      bHadrons.push_back(pitr);     
    }    
    for(uint i = 0; i < jets.size(); i++){   
      for(uint j = 0; j < bHadrons.size(); j++){
	HepMC::FourVector tmp = (*bHadrons[j])->momentum();
	TLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
	double dR = (*jets[i])->p4().DeltaR(genpart);
	if(dR<m_deltaRFromTruth){ 
	  bJetCounter++;
	  break;
	}    
      }   
    }
  }

  m_SumOfWeigths_Evt += weight;

  pass = (bJetCounter >= 2) && passLeadJetCut;
  if( (bJetCounter <= 1) && m_AcceptSomeLightEvents 
      && m_ranNumGen->Uniform() < (1.0 / m_LightJetSuppressionFactor ) 
      && passLeadJetCut ){
    /* Modify event weight to account for light jet prescale */
    for (HepMC::GenEvent* genEvt : *events()) {
      genEvt->weights().front() *= m_LightJetSuppressionFactor;
    }
    pass = true;
  }

  if(pass){ 
    m_NPass++;
    m_SumOfWeigths_Pass += weight;
  }

  setFilterPassed(pass);
  return StatusCode::SUCCESS;
}

bool DiBjetFilter::isBwithWeakDK(const int pID) const
{
   int id = abs(pID);
   if ( id == 511   ||    // B+
	id == 521   ||    // B0
	id == 531   ||    // Bs
	id == 541   ||    // Bc
	id == 5122  ||    // Lambda_B 
	id == 5132  ||    // Xi_b-         
	id == 5232  ||    // X_b0
	id == 5112  ||    // Sigma_b-
	id == 5212  ||    // Sigma_b0
	id == 5222 )      // Sigma_b+
       return true;
  else
       return false;
}


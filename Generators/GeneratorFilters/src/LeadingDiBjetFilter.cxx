/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/LeadingDiBjetFilter.cxx
// Description:
// This is a modification of the DiBjetFilter filter.  It differs in that 
//  b-hadrons must be matched to the two leading pT jets in order to be counted.
//  Otherwise it is the same.  
//   It cuts on:
//    - The presence of two b-hadron matched to the two leading pT jets.
//    - The pT of the leading jet
//    - It can also pass light jets and reweight the event accordingly to make an 
//      statisticaly inclusive sample of jets but with baised towards 
//      heavy flavour events
//
// AuthorList:
// S. Bieniek

// Header for this module:-
#include "GeneratorFilters/LeadingDiBjetFilter.h"

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
LeadingDiBjetFilter::LeadingDiBjetFilter(const std::string& name, 
			   ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //--------------------------------------------------------------------------    
  // Local Member Data:-
  declareProperty("LeadJetPtMin",m_leadJet_ptMin=0*Gaudi::Units::GeV);
  declareProperty("LeadJetPtMax",m_leadJet_ptMax=50000*Gaudi::Units::GeV);
  declareProperty("BottomPtMin",m_bottomPtMin=5.0*Gaudi::Units::GeV);
  declareProperty("BottomEtaMax",m_bottomEtaMax=3.0);
  declareProperty("JetPtMin",m_jetPtMin=15.0*Gaudi::Units::GeV);
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
 LeadingDiBjetFilter::~LeadingDiBjetFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode LeadingDiBjetFilter::filterInitialize() {
//---------------------------------------------------------------------------
  MsgStream log(messageService(), name());

  m_Nevt = 0;
  m_NPass = 0;
  m_SumOfWeigths_Pass = 0;
  m_SumOfWeigths_Evt = 0;

  if(m_AcceptSomeLightEvents){
    m_ranNumGen = new TRandom3();
    /* Set seed with respect to computer clock time */
    m_ranNumGen->SetSeed(0);
  }
  log << MSG::INFO << "Initialized" << endreq;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode LeadingDiBjetFilter::filterFinalize() {
//---------------------------------------------------------------------------
  MsgStream log(messageService(), name());

  if(m_AcceptSomeLightEvents){
    delete m_ranNumGen;
  }
  log << MSG::INFO <<  m_NPass << " Events out of " << m_Nevt << " passed the filter" << endreq;
  log << MSG::INFO <<  m_SumOfWeigths_Pass << " out of " << m_SumOfWeigths_Evt << " SumOfWeights counter, passed/total" << endreq;
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LeadingDiBjetFilter::filterEvent() {
//---------------------------------------------------------------------------
  MsgStream log(messageService(), name());

  bool pass = false;
  m_Nevt++;
  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 001" << endreq;
  const xAOD::JetContainer* truthjetTES = 0;
  //  StatusCode sc=m_sgSvc->retrieve( truthjetTES, m_TruthJetContainerName);
  StatusCode sc=evtStore()->retrieve( truthjetTES, m_TruthJetContainerName);
  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 002" << endreq;
  if( sc.isFailure()  ||  !truthjetTES ) {
    log << MSG::WARNING
	<< "No xAOD::JetContainer found in TDS " << m_TruthJetContainerName \
	<< sc.isFailure() << " "<<   !truthjetTES
	<< endreq;
    return StatusCode::SUCCESS;
  }

  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 003" << endreq;
  bool passLeadJetCut = false;
  xAOD::JetContainer::const_iterator jitr;
  double lead_jet_pt = 0.0;
  double lead_2nd_jet_pt = 0.0;
  std::vector<xAOD::JetContainer::const_iterator> jets;
  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 004" << endreq;
  for (jitr = (*truthjetTES).begin(); jitr !=(*truthjetTES).end(); ++jitr) { 
    if( (*jitr)->pt() < m_jetPtMin ) continue;
    if( fabs( (*jitr)->eta() ) > m_jetEtaMax ) continue;
    if(jets.size() == 0) {
      jets.push_back(jitr);
    }
    else if(jets.size() ==1) {
      if((*jitr)->pt()>(*jets[0])->pt()) {
 	jets.push_back(jets[0]);
	jets[0]=jitr;
      }
      else {
	jets.push_back(jitr);
     }
    }
    else {
      if((*jitr)->pt()>(*jets[0])->pt()) {
	jets[1]=jets[0];
	jets[0]=jitr;
      }
      else if((*jitr)->pt()>(*jets[1])->pt()) {
	jets[1]=jitr;
      }     
    }

      
    if( (*jitr)->pt() > lead_jet_pt ){
       lead_jet_pt = (*jitr)->pt();
    }
  }

  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 005" << endreq;
  if(jets.size() > 0) lead_jet_pt = (*jets[0])->pt();
  if(jets.size() > 1) lead_2nd_jet_pt=(*jets[1])->pt();
  if( lead_jet_pt > m_leadJet_ptMin && lead_jet_pt < m_leadJet_ptMax ) passLeadJetCut = true;
  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 006" << endreq;

  int bJetCounter = 0; 
  double weight = 1;
  int n_events_in_collection=0;
  int n_bHadrons_total=0;
  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 007" << endreq;
  McEventCollection::const_iterator itr;
  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 008" << endreq;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 009" << endreq;
    n_events_in_collection++;
    const HepMC::GenEvent* genEvt = (*itr);
    weight = genEvt->weights().front();
    HepMC::GenEvent::particle_const_iterator pitr;
    std::vector< HepMC::GenEvent::particle_const_iterator > bHadrons;
    // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 010" << endreq;
    for(pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr ) {  
      if( !isBwithWeakDK( (*pitr)->pdg_id()) ) continue;
      if( (*pitr)->momentum().perp() < m_bottomPtMin ) continue;
      if( fabs( (*pitr)->momentum().pseudoRapidity() ) > m_bottomEtaMax) continue;
      bHadrons.push_back(pitr);     
    }    
    // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 011" << endreq;
    n_bHadrons_total += bHadrons.size();
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

  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 012" << endreq;
  m_SumOfWeigths_Evt += weight;

  pass = (bJetCounter > 1) && passLeadJetCut;
  if( (bJetCounter < 2) && m_AcceptSomeLightEvents 
      && m_ranNumGen->Uniform() < (1.0 / m_LightJetSuppressionFactor ) 
      && passLeadJetCut ){
    /* Modify event weight to account for light jet prescale */
    for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
      HepMC::GenEvent* genEvt = (*itr);
      genEvt->weights().front() *= m_LightJetSuppressionFactor;
    }
    pass = true;
  }

  // if(m_Nevt > 420) log << MSG::INFO << " m_Nevt= " << m_Nevt << " filterEvent point 013" << endreq;
  if(pass){ 
    m_NPass++;
    m_SumOfWeigths_Pass += weight;
  }

  if(m_Nevt < 20 || m_Nevt%100 == 0 || pass) {
    log << MSG::INFO << " m_Nevt= " << m_Nevt << " n_events_in_collection= " << n_events_in_collection << " 1st,2nd lead_jet_pt= " << lead_jet_pt << " " <<  lead_2nd_jet_pt << " passLeadJetCut= " << passLeadJetCut 
      << " n_bHadrons_total= " << n_bHadrons_total <<  " bJetCounter= " << bJetCounter << " pass= " << pass << " m_NPass= " << m_NPass << endreq;
  }
  setFilterPassed(pass);
  return StatusCode::SUCCESS;
}

bool LeadingDiBjetFilter::isBwithWeakDK(const int pID) const
{
   int id = abs(pID);
   if ( id == 511   ||    // B+
        id == 521   ||    // B0
        id == 531   ||    // Bs
        id == 541   ||    // Bc
        id == 5122  ||    // Lambda_B
        id == 5132  ||    // Xi_b- 
        id == 5232  ||    // Xi_b0
        id == 5332 )      // Omega_b-
       return true;
  else
       return false;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
//
// File: GeneratorFilters/GapJetFilter.cxx
// Description: Filter suppresses events with low Pt
//              jets and low forward gaps
// AuthorList: Marek Tasevsky
//
// --------------------------------------------------

// Header for this module:-
#include "GeneratorFilters/GapJetFilter.h"

// Framework Related Headers:-
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/PhysicalConstants.h"

// Truth objects analysed
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

// Other classes used by this class:-
//#include <cmath>
#include <math.h>
#include <vector>
#include "TRandom3.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Pt  High --> Low
class High2LowByJetClassPt
{
public:
  bool operator () (const xAOD::Jet *t1, const xAOD::Jet *t2) const {
    return (t1->pt() > t2->pt());
  }
};

//--------------------------------------------------------------------------
GapJetFilter::GapJetFilter(const std::string & name,
					   ISvcLocator * pSvcLocator):
  GenFilter (name, pSvcLocator)
{

  std::vector<double> empty;

  //General Jets
  declareProperty("JetContainer", m_jetContainer = "AntiKt4TruthJets");

  //Allow a hard cut on the jet system
  declareProperty("MinPt1", m_minPt1 = 12.0);//In GeV
  declareProperty("MaxPt1", m_maxPt1 = 70000.0);//In GeV
  declareProperty("MinPt2", m_minPt2 = 12.0);//In GeV
  declareProperty("MaxPt2", m_maxPt2 = 70000.0);//In GeV

  declareProperty("MinPtparticle", m_PtCut = 0.);//In MeV
  declareProperty("MaxEtaparticle", m_EtaCut = 0.);

  declareProperty("weights", m_weights = empty );

  declareProperty("c0", m_c0 = 0.); 
  declareProperty("c1", m_c1 = 0.); 
  declareProperty("c2", m_c2 = 0.); 
  declareProperty("c3", m_c3 = 0.); 
  declareProperty("c4", m_c4 = 0.); 
  declareProperty("c5", m_c5 = 0.);  
  declareProperty("c6", m_c6 = 0.); 
  declareProperty("c7", m_c7 = 0.); 
  declareProperty("gapf", m_gapf = 0.);  

  m_xsgapf = 0.;

  m_storeGate = 0;
  m_myRandGen = 0;
}

//--------------------------------------------------------------------------
GapJetFilter::~GapJetFilter()
{
  //--------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
StatusCode
GapJetFilter::filterInitialize()
{
  //---------------------------------------------------------------------------

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    msg(MSG::ERROR)
         << "Unable to retrieve pointer to StoreGateSvc"
         << endmsg;
    return sc;
  }

  //Output settings to screen
  msg(MSG::INFO) << "xAOD::JetContainer: " <<  m_jetContainer << endmsg;

  //Jet Kinematic Cuts
  msg( MSG::INFO) << "Jet 1 Min Pt: " <<  m_minPt1 <<" Gaudi::Units::GeV"<< endmsg;
  msg( MSG::INFO) << "Jet 1 Max Pt: " <<  m_maxPt1 <<" Gaudi::Units::GeV"<< endmsg;
  msg( MSG::INFO) << "Jet 2 Min Pt: " <<  m_minPt2 <<" Gaudi::Units::GeV"<< endmsg;
  msg( MSG::INFO) << "Jet 2 Max Pt: " <<  m_maxPt2 <<" Gaudi::Units::GeV"<< endmsg;

  //Particle Cuts
  msg(MSG::INFO) << "Particle Min Pt: " << m_PtCut <<" Gaudi::Units::MeV" <<endmsg;
  msg(MSG::INFO) << "Particle Eta: " << m_EtaCut << endmsg;

  msg(MSG::INFO) << "Fit param. c0 = " << m_c0 << endmsg;
  msg(MSG::INFO) << "Fit param. c1 = " << m_c1 << endmsg;
  msg(MSG::INFO) << "Fit param. c2 = " << m_c2 << endmsg;
  msg(MSG::INFO) << "Fit param. c3 = " << m_c3 << endmsg;
  msg(MSG::INFO) << "Fit param. c4 = " << m_c4 << endmsg;
  msg(MSG::INFO) << "Fit param. c5 = " << m_c5 << endmsg;
  msg(MSG::INFO) << "Fit param. c6 = " << m_c6 << endmsg;
  msg(MSG::INFO) << "Fit param. c7 = " << m_c7 << endmsg;
  msg(MSG::INFO) << "Max. weighted gap = " << m_gapf << endmsg;

  m_xsgapf = m_c0*exp(m_c1+m_c2*m_gapf)+m_c3*exp(m_c4+m_c5*m_gapf)+m_c6*pow(m_gapf,m_c7);

  //Setup the random number generator for weighting
  m_myRandGen = new TRandom3();
  m_myRandGen->SetSeed(0); //completely random!

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode
GapJetFilter::filterFinalize()
{
  //---------------------------------------------------------------------------
  //Get rid of the random number generator
  delete m_myRandGen;

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode
GapJetFilter::filterEvent()
{
  //---------------------------------------------------------------------------
  StatusCode sc = StatusCode::SUCCESS;

  // Get TruthJets
  //
  msg(MSG::DEBUG) << "get truthJet container" << endmsg;
  const xAOD::JetContainer* truthjetTES;
  sc=m_storeGate->retrieve(truthjetTES, m_jetContainer);
  if( sc.isFailure() || !truthjetTES ) {
    msg(MSG::WARNING)
         << "No xAOD::JetContainer found in TDS " 
	 << m_jetContainer << " " 
	 << sc.isFailure() << " " << !truthjetTES
         << endmsg;
    return StatusCode::SUCCESS;
  }
  msg(MSG::INFO) << "xAOD::JetContainer Size = " 
       << truthjetTES->size() << endmsg;


  // Get a list of all the truth jets
  std::vector<const xAOD::Jet*> jetList;
  for (xAOD::JetContainer::const_iterator it_truth = truthjetTES->begin(); 
       it_truth != truthjetTES->end(); ++it_truth) {

      jetList.push_back(*it_truth);
  }

  // Sort of Jets by Pt
  std::sort(jetList.begin(), jetList.end(), High2LowByJetClassPt());

  
  //Apply various cuts

  // Number of Jets (need at least two)
  int flagNJets = -1;
  if (int(jetList.size()) < 2) {
    flagNJets = 0;
  } else {
    flagNJets = 1;
  }

  //
  // Leading 1st jet
  //
  int flag1stJet = -1;
  //double jetPt1 = -1.0;
  //float jetEta1 = -99.0;
  if (jetList.size() >=1) {
    const xAOD::Jet *j1 = jetList[0];
    //jetPt1 = j1->pt()/1000.0;
    flag1stJet = 1;

    //jetEta1 = j1->eta();

    if (j1->pt()/1000.0 < m_minPt1) {
      flag1stJet = 0;
    }
    if (j1->pt()/1000.0 > m_maxPt1) {
      flag1stJet = 0;
    }
  }

  //
  // Leading 2nd jet
  //
  int flag2ndJet = -1;
  //float jetEta2 = -99.0;
  if (jetList.size() >=2) {
    const xAOD::Jet *j2 = jetList[1];
    
    flag2ndJet = 1;

    //jetEta2 = j2->eta();

    if (j2->pt()/1000.0 < m_minPt2) {
      flag2ndJet = 0;
    }
    if (j2->pt()/1000.0 > m_maxPt2) {
      flag2ndJet = 0;
    }
  }
 
  msg(MSG::INFO) << "NJets  OK? : " << flagNJets  << endmsg;
  msg(MSG::INFO) << "1stJet OK? : " << flag1stJet << endmsg;
  msg(MSG::INFO) << "2ndJet OK? : " << flag2ndJet << endmsg;
  
  if (flagNJets != 0 && flag1stJet != 0 && flag2ndJet != 0) {
   
    if (m_gapf == 0) {

      return StatusCode::SUCCESS;}
    
    else if (m_gapf > 0) {


  float ptpart=-10., etapart=-100., cl_maxeta=-10., cl_mineta=10.;
  Int_t Clustag=0;
 
// Loop over all events in McEventCollection
  for (HepMC::GenEvent* genEvt : *events()) {

    // Loop over all particles in event
    HepMC::GenEvent::particle_const_iterator pitr;
    for (pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr ) {

//particles must be stable 
      ptpart = (*pitr)->momentum().perp();
      etapart = (*pitr)->momentum().pseudoRapidity();
      if((*pitr)->status()==1 && ptpart >m_PtCut && fabs(etapart) < m_EtaCut){
	Clustag=1; 
      if (etapart>cl_maxeta) cl_maxeta=etapart;
      if (etapart<cl_mineta) cl_mineta=etapart;
      }
    }
  
          float rapgap_cl=-100.; 
       if (fabs(cl_maxeta)<fabs(cl_mineta) && Clustag==1) {
         rapgap_cl = 4.9 - cl_maxeta;
       }

       else if (fabs(cl_maxeta)>fabs(cl_mineta) && Clustag==1) {
         rapgap_cl = 4.9 + cl_mineta;
       }

   double xsgap = m_c0*exp(m_c1+m_c2*rapgap_cl)+m_c3*exp(m_c4+m_c5*rapgap_cl)+m_c6*pow(rapgap_cl,m_c7);

   //cout<<"xsgapf2 = "<<std::setprecision(10) <<m_xsgapf<<endl;

   double weighting = m_xsgapf/xsgap;
   double rand = m_myRandGen->Rndm();

   double w = 0.0;
   if (rapgap_cl>m_gapf) {
       w=1.0;
   }
   else if (rapgap_cl<m_gapf){
     if (rand < weighting) w=weighting;
   }

   if (w>0.0) {
           genEvt->weights().push_back(1.0/w); 
           return StatusCode::SUCCESS;  
   } 

  }
    } //else if
  }


  //++m_nFail;
  setFilterPassed(false);
  msg(MSG::INFO) << "drop event" << endmsg;
  return StatusCode::SUCCESS;
  
}


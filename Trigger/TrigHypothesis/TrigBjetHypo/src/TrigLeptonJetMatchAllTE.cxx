/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigLeptonJetMatchAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#include "TrigBjetHypo/TrigLeptonJetMatchAllTE.h"

#include "xAODMuon/MuonContainer.h"

#include "JetEvent/Jet.h"

#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigParticle/TrigEFBjet.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigLeptonJetMatchAllTE::TrigLeptonJetMatchAllTE(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator),
  m_cutCounter(0),
  m_deltaEtaPass(0),
  m_deltaPhiPass(0)
{
  declareProperty("WorkingMode", m_workingMode = 1);
  //  declareProperty ("Instance",           m_instance);
  declareProperty("DeltaRCut",   m_deltaRCut   = 0.4);
  declareProperty("DeltaZCut",   m_deltaZCut   = 2);
//  declareProperty("JetKey",      m_jetKey      = "SplitJet");
  declareProperty("JetKey",      m_jetKey      = "");
  declareProperty("PriVtxKey",  m_priVtxKey    = "xPrimVx"); // EFHistoPrmVtx for PV from T2HistoPrmVtx

  declareProperty("EtThreshold",   m_etCut   = 10.);

  declareMonitoredVariable("CutCounter",   m_cutCounter);
  declareMonitoredVariable("DeltaEtaPass", m_deltaEtaPass);
  declareMonitoredVariable("DeltaPhiPass", m_deltaPhiPass);
  declareMonitoredVariable("MuonEFEta",    m_muonEFEta);
  declareMonitoredVariable("MuonEFPhi",    m_muonEFPhi);

  declareMonitoredVariable("DeltaRPass",    m_deltaRPass);
  declareMonitoredVariable("DeltaRAll",    m_deltaRAll);
  declareMonitoredVariable("DeltaZPass",    m_deltaZPass);
  declareMonitoredVariable("DeltaZAll",    m_deltaZAll);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigLeptonJetMatchAllTE::~TrigLeptonJetMatchAllTE() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetMatchAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Initializing TrigLeptonJetMatchAllTE" << endreq;

  //* declareProperty overview *//
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "declareProperty review:" << endreq;
        msg() << MSG::DEBUG << " WorkingMode = " << m_workingMode  << endreq; 
        //        msg() << MSG::DEBUG << " Instance = "            << m_instance << endreq;
        msg() << MSG::DEBUG << " DeltaRCut = "   << m_deltaRCut << endreq; 
        msg() << MSG::DEBUG << " DeltaZCut = "   << m_deltaZCut << endreq; 
        msg() << MSG::DEBUG << " JetKey = "   << m_jetKey << endreq;
        msg() << MSG::DEBUG << " PriVtxKey = "   << m_priVtxKey << endreq;

        msg() << MSG::DEBUG << " EtCut = "   << m_etCut << endreq;
      }
  
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigLeptonJetMatchAllTE::phiCorr(float phi) {

  if (phi < -M_PI) phi += 2*M_PI;
  if (phi >  M_PI) phi -= 2*M_PI;

  return phi;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetMatchAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigLeptonJetMatchAllTE" << endreq;

  bool pass = false;

  beforeExecMonitors().ignore();

  msg() << MSG::DEBUG 
        << "Number of input TEs is " <<  inputTE.size() 
        << " TE0: " << inputTE[0].size() 
        << " TE1: " << inputTE[1].size() <<  endreq;  

  m_cutCounter   = -1;
  m_deltaEtaPass = -9; m_muonEFEta = -9;
  m_deltaPhiPass = -9; m_muonEFPhi = -9;
  m_deltaRPass=-9;   m_deltaRAll=-9;
  m_deltaZPass=-9;   m_deltaZAll=-9;

  if (inputTE.size() < 2) {
    msg() << MSG::ERROR << "Number of input TEs is " <<  inputTE.size() << " and not 2. Configuration problem." << endreq;  
    return HLT::MISSING_FEATURE;
  }

  if(inputTE[0].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No muon TE found" << endreq;
    return HLT::OK;
  }

  if(inputTE[1].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No jet TE found" << endreq;
    return HLT::OK;
  }


 
  //
  // ==============================
  // Retrieve primary vertex
  // ==============================

  float  m_zPrmVtx=0;
  
  if(m_deltaZCut<500){
    const xAOD::VertexContainer*  pointerToPrmVtxCollections(0);
    
    if (getPrmVtxCollection(pointerToPrmVtxCollections, inputTE[1].front(), m_priVtxKey) != HLT::OK) {
      msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endreq;
      // If the ID PV-finding fails then use the PV from T2HistoPrmVtx instead
      // This is not ideal... investigate why ID PV finding fails
      if (m_priVtxKey == "xPrimVx" && getPrmVtxCollection(pointerToPrmVtxCollections, inputTE[1].front(), "EFHistoPrmVtx") != HLT::OK) {
        msg() << MSG::DEBUG<< "No primary vertex collection retrieved with name EFHistoPrmVtx either..." << endreq;
      }
      else if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Didn't manage to find " << m_priVtxKey << " PV, so using EFHistoPrmVtx instead." << endreq;
      }
    } 
    else if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Primary vertex collection retrieved" << endreq;
    }
    
    
    
    if(pointerToPrmVtxCollections == 0 ){
      msg() << MSG::DEBUG << "No vertex retrieved" << endreq;
    }
    else{
      const xAOD::Vertex* prmVertex = (*pointerToPrmVtxCollections)[0];
      m_zPrmVtx = prmVertex->z();
      msg() << MSG::DEBUG << "Found primary vertex at z = " << m_zPrmVtx << endreq;
    }
  }
  
  // ==============================
  // Retrieve tracks
  // ******************************
  //    !!!! WARNING !!!!
  // You need to retieve the tracks 
  // associated to the individual 
  // jet - at the moment you're just 
  // getting the ones associated to
  // the first jet in the event! 
  // ==============================
  
  /*  const xAOD::TrackParticleContainer*  pointerToEFTrackCollections(0);

  if ( getTrackCollection(pointerToEFTrackCollections, inputTE[1].front()) != HLT::OK) {
  msg() << MSG::DEBUG << "No track collection retrieved" << endreq;
  } 
  else if (msgLvl() <= MSG::DEBUG) {
  msg() << MSG::DEBUG << "Track collection retrieved" << endreq;
  }

  if( pointerToEFTrackCollections == 0 ){
  msg() << MSG::ERROR << "No tracks associated to the TE! " << endreq;
  return HLT::ERROR;
  }  
  else if (msgLvl() <= MSG::DEBUG)
  msg() << MSG::DEBUG << "HLT track collection retrieved with size = " << pointerToEFTrackCollections->size() << endreq;

  */

  HLT::TEVec allTEs;
  
  if ((inputTE.size()>0) && (inputTE[0].size()>0)) 
    allTEs.push_back(inputTE[0][0]);
  
  if ((inputTE.size()>0) && (inputTE[1].size()>0))
    for (unsigned int i=0; i<inputTE[1].size(); i++)
      allTEs.push_back(inputTE[1][i]);

  // ==============================
  // Retrieve HLT muons
  // ==============================

  for(unsigned int im=0; im<inputTE[0].size(); im++) {
    const xAOD::MuonContainer* m_muons=0;
    //    HLT::ErrorCode statusMuons = getFeature(inputTE[0].front(), m_muons);
    HLT::ErrorCode statusMuons = getFeature(inputTE[0][im], m_muons);

    if (statusMuons != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
        msg() << MSG::WARNING << "Failed to retrieve muons" << endreq;
      return HLT::NAV_ERROR;
    } 
    else if (m_muons==0) {
      if (msgLvl() <= MSG::WARNING)
        msg() << MSG::WARNING << "Missing muon feature." << endreq;
      return HLT::MISSING_FEATURE;
    } 
    else if (m_muons->size()==0) {
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Muon vector empty." << endreq;
      return HLT::OK;
    } 
    else {
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Retrieved " << m_muons->size() << " muons." << endreq;
    } 

    // ==============================
    // Retrieve HLT jets
    // ==============================
 
    for(unsigned int i=0; i<inputTE[1].size(); i++) {
      const xAOD::JetContainer* outJets(0);
    
      HLT::ErrorCode statJets = getFeature(inputTE[1][i], outJets, m_jetKey);
     

      //  msg() << MSG::DEBUG << "statJets " <<statJets  <<endreq;
      //
      if(statJets!=HLT::OK) {
        msg() << MSG::WARNING << " Failed to get JetCollections " << endreq;
        return HLT::OK;
      }
      //
      if( outJets == 0 ){
        msg() << MSG::DEBUG << " Got no JetCollections associated to the TE! " << endreq;
        return HLT::MISSING_FEATURE;
      }
      std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());


      //check size of JetCollection
      if( theJets.size() == 0 ){
        msg()<< MSG::DEBUG << " Size of JetCollection is 0! " << endreq;
        return HLT::OK;
      }

      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Retrieved " << theJets.size() << " jets." << endreq;
  
 
      // ===============================

      if(theJets.size()==0 ) {
        m_cutCounter=0;
        if( m_workingMode!=2 ) 
          return HLT::OK;
      }

      float muonEta=0, muonPhi=0, muonZ=0;
      float jetEta=0,  jetPhi=0, jetZ=0;
      float m_deltaEta=0, m_deltaPhi=0, m_deltaZ=0;

      std::vector<const  xAOD::Jet*>::const_iterator Jet     = theJets.begin();
      std::vector<const  xAOD::Jet*>::const_iterator lastJet = theJets.end();
 
      int j=0;
 
      for(auto Muon : *m_muons) {
        j++;

        const xAOD::Muon::MuonType muontype = Muon->muonType();
       if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Muon type "<< muontype << " and " 
	<< xAOD::Muon::MuonType::Combined 
	//<< " which gives " << muontype == xAOD::Muon::MuonType::Combined 
	<<  endreq;

	if(muontype != xAOD::Muon::MuonType::Combined) continue;

        muonEta = Muon->eta();
        muonPhi = Muon->phi();
        muonZ=0; 

        //        muonZ= (*(Muon->combinedTrackParticleLink()))->z0();
        muonZ = (*(Muon->combinedTrackParticleLink()))->z0()  + (*(Muon->combinedTrackParticleLink()))->vz();

        //   msg() << MSG::DEBUG << "Z je " <<tr->z0() << " = " <<muonZ <<  endreq;
      
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Muon "<< j+1 << "; pt " << Muon->pt()  << " eta "<< muonEta << "; phi " << muonPhi << "; z " << muonZ << endreq;
      
        for(unsigned int i=0; Jet!=lastJet; Jet++,i++) {

         if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "Jet "<< i+1 << "; et " << (*Jet)->p4().Et()/1000.
	<< " and cut " << m_etCut   <<  endreq;
	
          if((*Jet)->p4().Et() < m_etCut)continue;

          jetEta = (*Jet)->eta();
          jetPhi = (*Jet)->phi();
          jetZ=m_zPrmVtx;
	
          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "Jet "<< i+1 << "; et " << (*Jet)->p4().Et()/1000. << " eta "<< jetEta << "; phi " << jetPhi << "; z " << jetZ  <<  endreq;
	  

          m_deltaEta = muonEta - jetEta;
          m_deltaPhi = phiCorr(phiCorr(muonPhi) - phiCorr(jetPhi));
          m_deltaZ   = fabs(muonZ-jetZ);
	
          double dR = sqrt(m_deltaEta*m_deltaEta + m_deltaPhi*m_deltaPhi);

          if (msgLvl() <= MSG::DEBUG) 
            msg() << MSG::DEBUG << "deltaR = "<< dR << "; deltaZ = " << m_deltaZ <<  endreq; 

          m_deltaRAll=dR;
          m_deltaZAll=m_deltaZ;

          switch (m_workingMode) {

          case 1:
            if (dR < m_deltaRCut && m_deltaZ <= m_deltaZCut) {
              m_deltaEtaPass = m_deltaEta; m_deltaPhiPass = m_deltaPhi; 
              m_muonEFEta = muonEta; m_muonEFPhi = muonPhi;
              m_deltaRPass=dR;
              m_deltaZPass=m_deltaZ;
              pass = true;
              break;
            }
            break;
	  
          case 2:
            if (dR < m_deltaRCut) {
              pass = false;
              break;
            }
            break;
          }
        }
      }

      if (pass) {
	
        if(msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Accepting the event" << endreq;
	
        m_cutCounter=1;
        
        break;
        
      } else {
	
        if(msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Rejecting the event" << endreq;
	
        m_cutCounter=0;
         
      }
    }
  }

  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
  outputTE->setActiveState(false); 

  if (pass==true){
    outputTE->setActiveState(true);
  }
 
  afterExecMonitors().ignore();

  return HLT::OK;  
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetMatchAllTE::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigLeptonJetMatchAllTE" << endreq;

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigLeptonJetMatchAllTE::hltEndEvent() {

  if ( !AllTEAlgo::hltEndEvent() ) return false;
  AllTEAlgo::hltEndEvent();

  return true;
}

// -----------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigLeptonJetMatchAllTE::getTrackCollection(const xAOD::TrackParticleContainer*& pointerToEFTrackCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const xAOD::TrackParticleContainer*> vectorOfEFTrackCollections;

  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFTrackCollections, ""); 

  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrackParticleContainer from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfEFTrackCollections.size() << " TrackParticleContainer" << endreq;
  
  std::vector<const xAOD::TrackParticleContainer*>::iterator pTrackColl    = vectorOfEFTrackCollections.begin();
  std::vector<const xAOD::TrackParticleContainer*>::iterator lastTrackColl = vectorOfEFTrackCollections.end();
  
  if (pTrackColl == lastTrackColl) {
    pointerToEFTrackCollections = 0;
    return HLT::ERROR;
  } else {
    pointerToEFTrackCollections = *pTrackColl;
    return HLT::OK;
  }
}

// -----------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigLeptonJetMatchAllTE::getPrmVtxCollection(const xAOD::VertexContainer*& pointerToEFPrmVtxCollections, const HLT::TriggerElement* outputTE, std::string priVtxKey) {

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Retrieving PV with key = " << priVtxKey << std::endl;

  std::vector<const xAOD::VertexContainer*> vectorOfEFPrmVtxCollections;
  
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFPrmVtxCollections, priVtxKey);
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get xAOD::VertexContainer from the trigger element" << endreq;
  } 
  else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfEFPrmVtxCollections.size() << " xAOD::VertexContainer" << endreq;
  
  std::vector<const xAOD::VertexContainer*>::iterator pPrmVtxColl    = vectorOfEFPrmVtxCollections.begin();
  std::vector<const xAOD::VertexContainer*>::iterator lastPrmVtxColl = vectorOfEFPrmVtxCollections.end();
  
  // This part is message only
  if (msgLvl() <= MSG::VERBOSE) {
    
    for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
      
      if ((*pPrmVtxColl)->size() != 0)
        msg() << MSG::VERBOSE << "xAOD::VertexContainer with label " << (*pPrmVtxColl)->front()->vertexType() << endreq;
    }
    pPrmVtxColl = vectorOfEFPrmVtxCollections.begin();
  }
  
  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Size of pPrmVtxColl = " << (*pPrmVtxColl)->size() << endreq;
    
    if ((*pPrmVtxColl)->size() != 0) {
      
      if ((*pPrmVtxColl)->front()->vertexType() == xAOD::VxType::PriVtx) { // was == 1
        if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "Selected collection with Primary Vertex label " << (*pPrmVtxColl)->front()->vertexType() << endreq;
          msg() << MSG::DEBUG << "First PV has z-position = " <<  (*pPrmVtxColl)->front()->z() << endreq;
        }
        break;
      }
    }
  }
  
  // Message up to here, or better to say, this selects primary vertex from the collection
  
  if (pPrmVtxColl == lastPrmVtxColl) {
    pointerToEFPrmVtxCollections = 0;
    msg() << MSG::DEBUG << "No primary vertex collection found" << endreq;
    return HLT::ERROR;
  } 
  else {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Found a primary vertex collection." << endreq;
    }
    pointerToEFPrmVtxCollections = *pPrmVtxColl;
    return HLT::OK;
  }
}


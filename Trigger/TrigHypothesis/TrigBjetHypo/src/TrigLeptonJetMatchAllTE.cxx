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
    msg() << MSG::INFO << "Initializing TrigLeptonJetMatchAllTE" << endmsg;

  //* declareProperty overview *//
      if (msgLvl() <= MSG::DEBUG) {
	ATH_MSG_DEBUG( "declareProperty review:" );
	ATH_MSG_DEBUG( " WorkingMode = " << m_workingMode );
        //        msg() << MSG::DEBUG << " Instance = "            << m_instance << endmsg;
	ATH_MSG_DEBUG( " DeltaRCut = "   << m_deltaRCut );
	ATH_MSG_DEBUG( " DeltaZCut = "   << m_deltaZCut );
	ATH_MSG_DEBUG( " JetKey = "   << m_jetKey );
	ATH_MSG_DEBUG( " PriVtxKey = "   << m_priVtxKey );

	ATH_MSG_DEBUG( " EtCut = "   << m_etCut );
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
    ATH_MSG_DEBUG( "Executing TrigLeptonJetMatchAllTE" );

  bool pass = false;

  beforeExecMonitors().ignore();

  ATH_MSG_DEBUG( "Number of input TEs is " <<  inputTE.size() 
		 << " TE0: " << inputTE[0].size() 
		 << " TE1: " << inputTE[1].size() );

  m_cutCounter   = -1;
  m_deltaEtaPass = -9; m_muonEFEta = -9;
  m_deltaPhiPass = -9; m_muonEFPhi = -9;
  m_deltaRPass=-9;   m_deltaRAll=-9;
  m_deltaZPass=-9;   m_deltaZAll=-9;

  if (inputTE.size() < 2) {
    msg() << MSG::ERROR << "Number of input TEs is " <<  inputTE.size() << " and not 2. Configuration problem." << endmsg;  
    return HLT::MISSING_FEATURE;
  }

  if(inputTE[0].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      ATH_MSG_DEBUG( "No muon TE found" );
    return HLT::OK;
  }

  if(inputTE[1].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      ATH_MSG_DEBUG( "No jet TE found" );
    return HLT::OK;
  }


 
  //
  // ==============================
  // Retrieve primary vertex
  // ==============================

  float  zPrmVtx=0;
  
  if(m_deltaZCut<500){
    const xAOD::VertexContainer*  pointerToPrmVtxCollections(0);
    
    if (getPrmVtxCollection(pointerToPrmVtxCollections, inputTE[1].front(), m_priVtxKey) != HLT::OK) {
      ATH_MSG_DEBUG( "No primary vertex collection retrieved" );
      // If the ID PV-finding fails then use the PV from T2HistoPrmVtx instead
      // This is not ideal... investigate why ID PV finding fails
      if (m_priVtxKey == "xPrimVx" && getPrmVtxCollection(pointerToPrmVtxCollections, inputTE[1].front(), "EFHistoPrmVtx") != HLT::OK) {
	ATH_MSG_DEBUG( "No primary vertex collection retrieved with name EFHistoPrmVtx either..." );
      }
      else if (msgLvl() <= MSG::DEBUG) {
	ATH_MSG_DEBUG( "Didn't manage to find " << m_priVtxKey << " PV, so using EFHistoPrmVtx instead." );
      }
    } 
    else if (msgLvl() <= MSG::DEBUG) {
      ATH_MSG_DEBUG( "Primary vertex collection retrieved" );
    }
    
    
    
    if(pointerToPrmVtxCollections == 0 ){
      ATH_MSG_DEBUG( "No vertex retrieved" );
    }
    else{
      const xAOD::Vertex* prmVertex = (*pointerToPrmVtxCollections)[0];
      zPrmVtx = prmVertex->z();
      ATH_MSG_DEBUG( "Found primary vertex at z = " << zPrmVtx );
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
  msg() << MSG::DEBUG << "No track collection retrieved" << endmsg;
  } 
  else if (msgLvl() <= MSG::DEBUG) {
  msg() << MSG::DEBUG << "Track collection retrieved" << endmsg;
  }

  if( pointerToEFTrackCollections == 0 ){
  msg() << MSG::ERROR << "No tracks associated to the TE! " << endmsg;
  return HLT::ERROR;
  }  
  else if (msgLvl() <= MSG::DEBUG)
  msg() << MSG::DEBUG << "HLT track collection retrieved with size = " << pointerToEFTrackCollections->size() << endmsg;

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
    const xAOD::MuonContainer* muons=0;
    //    HLT::ErrorCode statusMuons = getFeature(inputTE[0].front(), muons);
    HLT::ErrorCode statusMuons = getFeature(inputTE[0][im], muons);

    if (statusMuons != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
        msg() << MSG::WARNING << "Failed to retrieve muons" << endmsg;
      return HLT::NAV_ERROR;
    } 
    else if (muons==0) {
      if (msgLvl() <= MSG::WARNING)
        msg() << MSG::WARNING << "Missing muon feature." << endmsg;
      return HLT::MISSING_FEATURE;
    } 
    else if (muons->size()==0) {
      if (msgLvl() <= MSG::DEBUG)
	ATH_MSG_DEBUG( "Muon vector empty." );
      return HLT::OK;
    } 
    else {
      if (msgLvl() <= MSG::DEBUG)
	ATH_MSG_DEBUG( "Retrieved " << muons->size() << " muons." );
    } 

    // ==============================
    // Retrieve HLT jets
    // ==============================
 
    for(unsigned int i=0; i<inputTE[1].size(); i++) {
      const xAOD::JetContainer* outJets(0);
    
      HLT::ErrorCode statJets = getFeature(inputTE[1][i], outJets, m_jetKey);
     

      //  msg() << MSG::DEBUG << "statJets " <<statJets  <<endmsg;
      //
      if(statJets!=HLT::OK) {
        msg() << MSG::WARNING << " Failed to get JetCollections " << endmsg;
        return HLT::OK;
      }
      //
      if( outJets == 0 ){
	ATH_MSG_DEBUG( " Got no JetCollections associated to the TE! " );
        return HLT::MISSING_FEATURE;
      }
      std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());


      //check size of JetCollection
      if( theJets.size() == 0 ){
	ATH_MSG_DEBUG( " Size of JetCollection is 0! " );
        return HLT::OK;
      }

      if (msgLvl() <= MSG::DEBUG)
	ATH_MSG_DEBUG( "Retrieved " << theJets.size() << " jets." );
  
 
      // ===============================

      if(theJets.size()==0 ) {
        m_cutCounter=0;
        if( m_workingMode!=2 ) 
          return HLT::OK;
      }

      float muonEta=0, muonPhi=0, muonZ=0;
      float jetEta=0,  jetPhi=0, jetZ=0;
      float deltaEta=0, deltaPhi=0, deltaZ=0;

      std::vector<const  xAOD::Jet*>::const_iterator Jet     = theJets.begin();
      std::vector<const  xAOD::Jet*>::const_iterator lastJet = theJets.end();
 
      int j=0;
 
      for(auto Muon : *muons) {
        j++;

        const xAOD::Muon::MuonType muontype = Muon->muonType();
       if (msgLvl() <= MSG::DEBUG)
	 ATH_MSG_DEBUG( "Muon type "<< muontype << " and " 
			<< xAOD::Muon::MuonType::Combined );
	//<< " which gives " << muontype == xAOD::Muon::MuonType::Combined 

	if(muontype != xAOD::Muon::MuonType::Combined) continue;

        muonEta = Muon->eta();
        muonPhi = Muon->phi();
        muonZ=0; 

        //        muonZ= (*(Muon->combinedTrackParticleLink()))->z0();
        muonZ = (*(Muon->combinedTrackParticleLink()))->z0()  + (*(Muon->combinedTrackParticleLink()))->vz();

        //   msg() << MSG::DEBUG << "Z je " <<tr->z0() << " = " <<muonZ <<  endmsg;
      
        if (msgLvl() <= MSG::DEBUG)
	  ATH_MSG_DEBUG( "Muon "<< j+1 << "; pt " << Muon->pt()  << " eta "<< muonEta << "; phi " << muonPhi << "; z " << muonZ );
      
        for(unsigned int i=0; Jet!=lastJet; Jet++,i++) {

         if (msgLvl() <= MSG::DEBUG)
	      ATH_MSG_DEBUG( "Jet "<< i+1 << "; et " << (*Jet)->p4().Et()/1000.
			     << " and cut " << m_etCut );
	
          if((*Jet)->p4().Et() < m_etCut)continue;

          jetEta = (*Jet)->eta();
          jetPhi = (*Jet)->phi();
          jetZ=zPrmVtx;
	
          if (msgLvl() <= MSG::DEBUG)
	    ATH_MSG_DEBUG( "Jet "<< i+1 << "; et " << (*Jet)->p4().Et()/1000. << " eta "<< jetEta << "; phi " << jetPhi << "; z " << jetZ );
	  

          deltaEta = muonEta - jetEta;
          deltaPhi = phiCorr(phiCorr(muonPhi) - phiCorr(jetPhi));
          deltaZ   = fabs(muonZ-jetZ);
	
          double dR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);

          if (msgLvl() <= MSG::DEBUG) 
	    ATH_MSG_DEBUG( "deltaR = "<< dR << "; deltaZ = " << deltaZ );

          m_deltaRAll=dR;
          m_deltaZAll=deltaZ;

          switch (m_workingMode) {

          case 1:
            if (dR < m_deltaRCut && deltaZ <= m_deltaZCut) {
              m_deltaEtaPass = deltaEta; m_deltaPhiPass = deltaPhi; 
              m_muonEFEta = muonEta; m_muonEFPhi = muonPhi;
              m_deltaRPass=dR;
              m_deltaZPass=deltaZ;
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
	  ATH_MSG_DEBUG( "Accepting the event" );
	
        m_cutCounter=1;
        
        break;
        
      } else {
	
        if(msgLvl() <= MSG::DEBUG)
	  ATH_MSG_DEBUG( "Rejecting the event" );
	
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

  msg() << MSG::INFO << "Finalizing TrigLeptonJetMatchAllTE" << endmsg;

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
    msg() << MSG::ERROR << "Failed to get TrackParticleContainer from the trigger element" << endmsg;
  } else if (msgLvl() <= MSG::DEBUG) 
    ATH_MSG_DEBUG( "Got " << vectorOfEFTrackCollections.size() << " TrackParticleContainer" );
  
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
    ATH_MSG_DEBUG( "Retrieving PV with key = " << priVtxKey );

  std::vector<const xAOD::VertexContainer*> vectorOfEFPrmVtxCollections;
  
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFPrmVtxCollections, priVtxKey);
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get xAOD::VertexContainer from the trigger element" << endmsg;
  } 
  else if (msgLvl() <= MSG::DEBUG) 
    ATH_MSG_DEBUG( "Got " << vectorOfEFPrmVtxCollections.size() << " xAOD::VertexContainer" );
  
  std::vector<const xAOD::VertexContainer*>::iterator pPrmVtxColl    = vectorOfEFPrmVtxCollections.begin();
  std::vector<const xAOD::VertexContainer*>::iterator lastPrmVtxColl = vectorOfEFPrmVtxCollections.end();
  
  // This part is message only
  if (msgLvl() <= MSG::VERBOSE) {
    
    for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
      
      if ((*pPrmVtxColl)->size() != 0)
	ATH_MSG_VERBOSE( "xAOD::VertexContainer with label " << (*pPrmVtxColl)->front()->vertexType() );
    }
    pPrmVtxColl = vectorOfEFPrmVtxCollections.begin();
  }
  
  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
    
    if (msgLvl() <= MSG::DEBUG)
      ATH_MSG_DEBUG( "Size of pPrmVtxColl = " << (*pPrmVtxColl)->size() );
    
    if ((*pPrmVtxColl)->size() != 0) {
      
      if ((*pPrmVtxColl)->front()->vertexType() == xAOD::VxType::PriVtx) { // was == 1
        if (msgLvl() <= MSG::DEBUG) {
	  ATH_MSG_DEBUG( "Selected collection with Primary Vertex label " << (*pPrmVtxColl)->front()->vertexType() );
	  ATH_MSG_DEBUG( "First PV has z-position = " <<  (*pPrmVtxColl)->front()->z() );
        }
        break;
      }
    }
  }
  
  // Message up to here, or better to say, this selects primary vertex from the collection
  
  if (pPrmVtxColl == lastPrmVtxColl) {
    pointerToEFPrmVtxCollections = 0;
    ATH_MSG_DEBUG( "No primary vertex collection found" );
    return HLT::ERROR;
  } 
  else {
    if (msgLvl() <= MSG::DEBUG) {
      ATH_MSG_DEBUG( "Found a primary vertex collection." );
    }
    pointerToEFPrmVtxCollections = *pPrmVtxColl;
    return HLT::OK;
  }
}


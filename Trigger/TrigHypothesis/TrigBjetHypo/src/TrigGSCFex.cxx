/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigGSCFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// ************************************************

#include "TrigBjetHypo/TrigGSCFex.h"

// ONLINE INFRASTRUCTURE
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h" // TO BE REMOVED

// BEAMSPOT
#include "InDetBeamSpotService/IBeamCondSvc.h"

// EDM

#include "xAODBase/IParticle.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrigSteeringEvent/PhiHelper.h"

// ----------------------------------------------------------------------------------------------------------------- 


TrigGSCFex::TrigGSCFex(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator)

{
  declareProperty("setupOfflineTools",  m_setupOfflineTools = false);

  declareProperty("TrackKey",           m_trackKey = "");
  declareProperty("JetKey",             m_jetKey = "");
  declareProperty("JetOutputKey",       m_jetOutputKey = "GSCJet");
  declareProperty("PriVtxKey",          m_priVtxKey = "");
  declareProperty("GSCCalibrationTool", m_jetGSCCalib_tool);

  // Run-2 monitoring
  
  declareMonitoredVariable("gsc_ntrk",    m_mon_gsc_ntrk,    AutoClear);
  declareMonitoredVariable("gsc_width",   m_mon_gsc_width,   AutoClear);
  declareMonitoredVariable("gsc_ptsum",   m_mon_gsc_ptsum,   AutoClear);
  declareMonitoredVariable("gsc_ptdiff",  m_mon_gsc_ptdiff,  AutoClear);
  declareMonitoredVariable("gsc_ptratio", m_mon_gsc_ptratio, AutoClear);


}


// ----------------------------------------------------------------------------------------------------------------- 


TrigGSCFex::~TrigGSCFex() {

}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigGSCFex::hltInitialize() {

  // Get message service 
  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Initializing TrigGSCFex, version " << PACKAGE_VERSION << endmsg;

  // declareProperty overview 
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;

  }

  if(m_setupOfflineTools) {

    if(m_jetGSCCalib_tool.empty()){
      if(m_jetGSCCalib_tool.retrieve().isFailure()) {
	msg() << MSG::FATAL << "Failed to locate tool " << m_jetGSCCalib_tool << endmsg;
	return HLT::BAD_JOB_SETUP;
      } else 
	msg() << MSG::INFO << "Retrieved tool " << m_jetGSCCalib_tool << endmsg;	
    } else if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No GSCCalibrationTool tool to retrieve" << endmsg;

  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigGSCFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigGSCFex" << endmsg;

  // RETRIEVE INPUT CONTAINERS

  // Get EF jet 
  const xAOD::JetContainer* jets = nullptr;
  if(getFeature(inputTE, jets, m_jetKey) == HLT::OK && jets != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::JetContainer: " << "nJets = " << jets->size() << endmsg;
  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - No xAOD::JetContainer" << endmsg;
    return HLT::MISSING_FEATURE;
  }

  // Get primary vertex 
  const xAOD::VertexContainer* vertexes = nullptr;
  if (getFeature(outputTE, vertexes, m_priVtxKey) == HLT::OK && vertexes != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: " << "nVertexes = " << vertexes->size() << endmsg;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "INPUT - No xAOD::VertexContainer" << endmsg;
    return HLT::MISSING_FEATURE;
  }

  // Get tracks 
  const xAOD::TrackParticleContainer* tracks = nullptr;
  if(getFeature(outputTE, tracks, m_trackKey) == HLT::OK && tracks != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::TrackParticleContainer: " << "nTracks = " << tracks->size() << endmsg;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "INPUT - No xAOD::TrackParticleContainer" << endmsg;
    return HLT::MISSING_FEATURE;
  }

  // PREPARE PROCESSING AND OUTPUT CONTAINERS

  //
  // get primary vertex 
  //
  auto vtxitr = vertexes->begin();
  const auto primaryVertex = *vtxitr;

  // Prepare jet tagging - create temporary jet copy 
  auto jetitr=jets->begin();
  xAOD::Jet jet;
  jet.makePrivateStore(**jetitr);

  //std::cout << "TrigGSCFex: jet"
  //	    << " pt: "  << jet.p4().Pt()
  //	    << " eta: " << jet.p4().Eta()
  //	    << " phi: " << jet.p4().Phi()
  //	    << " m: "   << jet.p4().M()
  //	    << std::endl;
  
  //std::cout << "primaryVertex z" << primaryVertex->z() << std::endl;

  // Compute and store GSC moments from precision tracks

  //=======================================================

  unsigned int nTrk = 0;
  double       width=0;
  double       ptsum=0;

  for(const xAOD::TrackParticle* trk : *tracks ){
    
//std::cout << "\tTrigGSCFex: track"
//	      << " pt: "  << trk->p4().Pt()
//	      << " eta: " << trk->p4().Eta()
//	      << " phi: " << trk->p4().Phi()
//	      << " m: "   << trk->p4().M()
//	      << std::endl;

    //
    // trkIsGood() defined below
    //
    if(trkIsGood(trk) && fabs((trk->z0() + trk->vz() - primaryVertex->z())*sin(trk->theta())) <= 1.0){

      float dEta = (trk->eta() - jet.p4().Eta());
      float dPhi =  HLT::deltaPhi(trk->phi(), jet.p4().Phi());
      width += trk->pt() * sqrt( dEta*dEta + dPhi*dPhi);
      ptsum += trk->pt();

      ++nTrk;
    }

  }

  if(ptsum > 0) width = width / ptsum;
  else          width = -1.;

  //
  // Set moments in the jet
  //
  std::vector<float> widthVector;
  widthVector.push_back(width);
  jet.setAttribute("TrackWidthPt1000", widthVector);

  std::vector<int> ntrkVector;
  ntrkVector.push_back(nTrk);
  jet.setAttribute("NumTrkPt1000", ntrkVector);  

  //
  // Muon segments must also be set for calibration tool to work
  // fill w/ 0 as a dummy
  int gmsc = 0;
  jet.setAttribute("GhostMuonSegmentCount", gmsc);


  // EXECUTE OFFLINE TOOLS


  // calJet is a pointer to the new, calibrated jet
  xAOD::Jet* calJet = nullptr;
  m_jetGSCCalib_tool->calibratedCopy(jet,calJet);

//  std::cout << "TrigGSCFex: New jet"
//	    << " pt: "  << calJet->p4().Pt()
//	    << " eta: " << calJet->p4().Eta()
//	    << " phi: " << calJet->p4().Phi()
//	    << " m: "   << calJet->p4().M()
//	    << std::endl;
  
  xAOD::JetTrigAuxContainer trigJetTrigAuxContainer;
  xAOD::JetContainer* jc = new xAOD::JetContainer;
  jc->setStore(&trigJetTrigAuxContainer);
  jc->push_back ( new xAOD::Jet(*calJet) );
  delete calJet;

//  std::cout << "TrigGSCFex: New jet back"
//	    << " pt: "  << jc->back()->p4().Pt()
//	    << " eta: " << jc->back()->p4().Eta()
//	    << " phi: " << jc->back()->p4().Phi()
//	    << " m: "   << jc->back()->p4().M()
//	    << std::endl;
//

  // do not allow GSC track to give large negative energy corrections
  // to jets with only a few tracks
  if(jc->back()->p4().Et() < jet.p4().Et()*0.95 && nTrk<= 3){
    float newET = jet.p4().Et()*0.95;
    xAOD::JetFourMom_t p4 = jet.jetP4();
    p4.SetPt(sqrt(newET*newET - (jet.m())*(jet.m())));
    jc->back()->setJetP4( p4 );
  }
  
  HLT::ErrorCode hltStatus = attachFeature(outputTE, jc, m_jetOutputKey); 
  if (hltStatus != HLT::OK) {
    msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer (" << m_jetOutputKey << ") as feature jet eta, phi " << jc->back()->eta() << ", " << jc->back()->phi() << endmsg;
    return hltStatus;
  }


  // Fill monitoring variables
  m_mon_gsc_ntrk  = nTrk;
  m_mon_gsc_width = width;
  m_mon_gsc_ptsum = ptsum; 
  m_mon_gsc_ptdiff = jet.p4().Pt() - jc->back()->p4().Pt(); 
  if( jc->back()->p4().Pt() != 0 ) m_mon_gsc_ptratio = ( m_mon_gsc_ptdiff )/( jc->back()->p4().Pt() ) ; 
  else m_mon_gsc_ptratio = -999.;
  
  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigGSCFex::hltFinalize() {

  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Finalizing TrigGSCFex" << endmsg;

  return HLT::OK;
}


bool TrigGSCFex::trkIsGood(const xAOD::TrackParticle *trk){

  uint8_t numberOfPixelHits=0;
  if( ! trk->summaryValue (numberOfPixelHits, xAOD::SummaryType::numberOfPixelHits) )
    std::cout<<"numberOfPixelHits not available"<<std::endl;
  uint8_t numberOfPixelHoles=0;
  if( ! trk->summaryValue (numberOfPixelHoles, xAOD::SummaryType::numberOfPixelHoles) )
    std::cout<<"numberOfPixelHoles not available"<<std::endl;
  uint8_t numberOfSCTHits=0;
  if( ! trk->summaryValue (numberOfSCTHits, xAOD::SummaryType::numberOfSCTHits) )
    std::cout<<"numberOfSCTHits not available"<<std::endl;
  uint8_t numberOfSCTHoles=0;
  if( ! trk->summaryValue (numberOfSCTHoles, xAOD::SummaryType::numberOfSCTHoles) )
    std::cout<<"numberOfSCTHoles not available"<<std::endl;
  uint8_t numberOfPixelSharedHits=0;
  if( ! trk->summaryValue (numberOfPixelSharedHits, xAOD::SummaryType::numberOfPixelSharedHits) )
    std::cout<<"numberOfPixelSharedHits not available"<<std::endl;

  if(trk->pt() >= 1000. //not 400 MeV                                                                                                         
     && fabs(trk->eta()) <= 2.5
     && (numberOfPixelHits + numberOfSCTHits) >= 7
     && (numberOfPixelHoles + numberOfSCTHoles) <= 2
     && numberOfPixelHoles <= 1
     && numberOfPixelSharedHits <= 1) //this should be shared modules, but including SCT doesn't seem straightforward           
    return true;

  return false;
}

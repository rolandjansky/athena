// emacs: this is -*- c++ -*-
//
//   @file    TrigJetSplitterAllTE.cxx        
//
//            Splits a jet collection into separate TEs
//                   
//  
//   Copyright (C) 2014 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigJetSplitterAllTE.cxx, v0.0   Tue 17 Jun 2014 03:26:44 CEST sutt $


#include "GaudiKernel/IToolSvc.h"

#include "TrigBjetHypo/TrigJetSplitterAllTE.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigNavigation/TriggerElement.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "xAODJet/JetContainer.h"
#include "AthContainers/ConstDataVector.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitterAllTE::TrigJetSplitterAllTE(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty ("JetInputKey",        m_jetInputKey        = "TrigJetRec");
  declareProperty ("JetOutputKey",       m_jetOutputKey       = "SplitJet");
  declareProperty ("PriVtxKey",          m_priVtxKey          = "xPrimVx"); //"EFHistoPrmVtx" 
  declareProperty ("UsePriVtxKeyBackup", m_usePriVtxKeyBackup = true);
  declareProperty ("PriVtxKeyBackup",    m_priVtxKeyBackup    = "EFHistoPrmVtx");
  declareProperty ("EtaHalfWidth",       m_etaHalfWidth       = 0.4);
  declareProperty ("PhiHalfWidth",       m_phiHalfWidth       = 0.4);
  declareProperty ("ZHalfWidth",         m_zHalfWidth         = 20.0);// in mm?
  declareProperty ("JetMinEt",           m_minJetEt           = 15.0); // in GeV ==> Can't be any higher than the lowest pT chain that will run
  declareProperty ("JetMaxEta",          m_maxJetEta          = 2.5+m_etaHalfWidth);  // tracker acceptance + jet half-width
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigJetSplitterAllTE, version " << PACKAGE_VERSION << endreq;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " JetInputKey  = "  << m_jetInputKey << endreq; 
    msg() << MSG::DEBUG << " JetOutputKey = " << m_jetOutputKey << endreq;
    msg() << MSG::DEBUG << " PriVtxKey    = " << m_priVtxKey    << endreq; 
    msg() << MSG::DEBUG << " EtaHalfWidth = " << m_etaHalfWidth << endreq; 
    msg() << MSG::DEBUG << " PhiHalfWidth = " << m_phiHalfWidth << endreq; 
    msg() << MSG::DEBUG << " ZHalfWidth   = " << m_zHalfWidth   << endreq; 
    msg() << MSG::DEBUG << " MinJetEt     = " << m_minJetEt     << endreq; 
    msg() << MSG::DEBUG << " MaxJetEta    = " << m_maxJetEta    << endreq; 
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitterAllTE::~TrigJetSplitterAllTE(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Running TrigJetSplitterAllTE::hltExecute" << endreq;

  beforeExecMonitors().ignore();

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " inputTEs.size() " << inputTEs.size() << endreq;
 
  if (inputTEs.size() == 0) {
    msg() << MSG::WARNING << "No input TEs" << endreq;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  if (inputTEs.size() == 1) {
    msg() << MSG::DEBUG << "Only one input TE.  No z-position constraint will be applied!" << endreq;
  }

  if (inputTEs.size() > 2) {
    msg() << MSG::WARNING << "Too many TEs passed as input" << endreq;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  // -----------------------
  // Retreive jets
  // -----------------------

  std::vector<HLT::TriggerElement*>& jetTE = inputTEs.at(0); // jet TE

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " jetTE.size() " << jetTE.size() << endreq;

  if (jetTE.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE (jets)" << endreq;
    afterExecMonitors().ignore();
    return HLT::MISSING_FEATURE; 
  }

  const xAOD::JetContainer* jets = 0;
  //HLT::ErrorCode statusJets = getFeature(jetTE.front(), jets, m_jetInputKey);
  HLT::ErrorCode statusJets = getFeature(jetTE.front(), jets);  // this should really be given a name - need to find out froim the jet guys what!

  if (statusJets != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (jets)" << endreq;
    return HLT::NAV_ERROR;
  }

  if(jets==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jets)." << endreq;
    return HLT::MISSING_FEATURE;
  }

  // -----------------------------------
  // Retreive primary vertex (optional)
  // -----------------------------------

  // If there is a 2nd TE given as input it should be the PV TE

  bool use_z_constraint = false;
  float prmVtx_z = -9e9;

  if (inputTEs.size() == 2) {

    std::vector<HLT::TriggerElement*>& vtxTE = inputTEs.at(1); // vertex TE

    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " vtxTE.size() " << vtxTE.size() << endreq;

    if (vtxTE.size() == 0) {
      msg() << MSG::WARNING << "Got an empty inputTE (vertex)" << endreq;
      afterExecMonitors().ignore();
      return HLT::MISSING_FEATURE; 
    }

    use_z_constraint = true;

    const xAOD::VertexContainer* vertices = 0;
    HLT::ErrorCode statusVertices = getFeature(vtxTE.front(), vertices, m_priVtxKey);  
    
    //
    // Retrieve the vertiecies (Fallback to HistoPrmVtx if failure) 
    //
    if (statusVertices != HLT::OK) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (PV) " << m_priVtxKey << endreq;
      
      //
      // Try for HistoPrmVtx instead
      //
      if(m_usePriVtxKeyBackup){
	statusVertices = getAndCheckHistoPrmVtx(vtxTE.front(), vertices);
      
	if(statusVertices != HLT::OK){
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (PV) " << m_priVtxKeyBackup << endreq;
	  return statusVertices;
	}
	
      }else{
	return HLT::NAV_ERROR;
      }
      
    }
    
    //
    //  Check pointer (Fallback to HistoPrmVtx if failure) 
    //
    if(vertices==0) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (vertices): " << m_priVtxKey << endreq;

      //
      // Try for HistoPrmVtx instead
      //
      if(m_usePriVtxKeyBackup){
	statusVertices = getAndCheckHistoPrmVtx(vtxTE.front(), vertices);
      
	if(statusVertices != HLT::OK){
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (PV) " << m_priVtxKeyBackup << endreq;
	  return statusVertices;
	}
	if(vertices==0) {
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (vertices): " << m_priVtxKeyBackup << endreq;
	  return HLT::MISSING_FEATURE;
	}
      } else {
	return HLT::MISSING_FEATURE;
      }


    }

    //
    //  Check size (Fallback to HistoPrmVtx if failure) 
    //
    if(vertices->empty()) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (vertices): " << m_priVtxKey << endreq;

      //
      // Try for HistoPrmVtx instead
      //
      if(m_usePriVtxKeyBackup){
	statusVertices = getAndCheckHistoPrmVtx(vtxTE.front(), vertices);
      
	if(statusVertices != HLT::OK){
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (PV) " << m_priVtxKeyBackup << endreq;
	  return statusVertices;
	}
	if(vertices==0) {
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (vertices): " << m_priVtxKeyBackup << endreq;
	  return HLT::MISSING_FEATURE;
	}
	if(vertices->empty()) {
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (vertices): " << m_priVtxKeyBackup << endreq;
	  return HLT::MISSING_FEATURE;
	}
      } else {
	return HLT::MISSING_FEATURE;
      }


    }

    //
    // Get the first primary vertex
    //
    const xAOD::Vertex* prmVtx = getPrimaryVertex(vertices);

    //
    // If no PV revert to the HistoPV
    //
    if(!prmVtx){
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " No primary vertices in " << m_priVtxKey << " checking HistoPV" << endreq;      

      //
      // Try for HistoPrmVtx instead
      //
      if(m_usePriVtxKeyBackup){
	statusVertices = getAndCheckHistoPrmVtx(vtxTE.front(), vertices);
	if(statusVertices != HLT::OK){
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (PV) " << m_priVtxKeyBackup << endreq;
	  return statusVertices;
	}

	const xAOD::Vertex* prmVtx = getPrimaryVertex(vertices);      
	if(!prmVtx){
	  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " No primary vertices in " << m_priVtxKeyBackup << " return HLT::OL" << endreq;      
	  return HLT::OK;
	}
      } else{
	return HLT::OK;
      }

    }

    prmVtx_z = prmVtx->z();
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Primary vertex z-position = " << prmVtx_z << endreq;
  }

  // -----------------------
 
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Found " << jets->size() << " jets, creating corresponding RoIs" << endreq; 

  HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
  
  unsigned int i = 0;
  for ( xAOD::JetContainer::const_iterator jetitr=jets->begin() ; jetitr!=jets->end() ; jetitr++ ) { 
  
    ++i;
    auto jet = *jetitr;

    float jetEt  = jet->p4().Et()*0.001;
    float jetEta = jet->eta();
    float jetPhi = jet->phi();

    if (jetEt < m_minJetEt) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet "<< i << " below the " << m_minJetEt << " GeV threshold; Et " << jetEt << "; skipping this jet." << endreq;
      continue;
    }
    if (fabs(jetEta) > m_maxJetEta) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet "<< i << " outside the |eta| < 2.5 requirement; Eta = " << jetEta << "; skipping this jet." << endreq;
      continue;
    }
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Jet "<< i << "; Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi << endreq;

    // Create an output TE seeded by an empty vector
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode( initialTE, output );
    outputTE->setActiveState(true);

    // Make deep copy of vertex and attach to output TE

    /// create RoI correspondinding to the jet
    double phiMinus = HLT::wrapPhi(jetPhi-m_phiHalfWidth); 
    double phiPlus  = HLT::wrapPhi(jetPhi+m_phiHalfWidth); 

    double etaMinus = jetEta-m_etaHalfWidth;  
    double etaPlus  = jetEta+m_etaHalfWidth;  

    double zMinus   = prmVtx_z-m_zHalfWidth;
    double zPlus    = prmVtx_z+m_zHalfWidth;

    // Use z, zplus and zminus info if available when constructing the ROI.
    // Otherwise, just do it the good old fashioned way.

    TrigRoiDescriptor* roi;
    if (use_z_constraint && prmVtx_z > -9e9 ) { 
      roi =  new TrigRoiDescriptor(jetEta,   etaMinus, etaPlus, 
				   jetPhi,   phiMinus, phiPlus,
				   prmVtx_z, zMinus,   zPlus  );
    }
    else {
      roi =  new TrigRoiDescriptor(jetEta,   etaMinus, etaPlus, 
				   jetPhi,   phiMinus, phiPlus);
    }

    HLT::ErrorCode hltStatus = attachFeature(outputTE, roi, m_jetOutputKey);
    if ( hltStatus != HLT::OK ) {
      msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature " << *roi << endreq;
      return hltStatus;
    }

    // Ideally would have liked to attach a view container
    // but these cannot be persistified at the moment...
    // ConstDataVector<xAOD::JetContainer>* jc = new ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
    // jc->clear(SG::VIEW_ELEMENTS); 
    // jc->push_back ( *jetitr );

    // ... so for the time being do a deep copy
    xAOD::JetTrigAuxContainer trigJetTrigAuxContainer;
    xAOD::JetContainer* jc = new xAOD::JetContainer;
    jc->setStore(&trigJetTrigAuxContainer);
    jc->push_back ( new xAOD::Jet(**jetitr) );
    
    // for checking Et and eta of jets in hypos later
    TrigOperationalInfo* trigInfoJetEt = new TrigOperationalInfo();
    trigInfoJetEt->set("EFJetEt", jetEt);
    HLT::ErrorCode hltEtStatus = attachFeature(outputTE, trigInfoJetEt, "EFJetInfo"); 
    if (hltEtStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach TrigOperationalInfo (jet Et) as feature" << endreq;
      return hltEtStatus;
    }

    TrigOperationalInfo* trigInfoJetEta = new TrigOperationalInfo();
    trigInfoJetEta->set("EFJetEta", jetEta);
    HLT::ErrorCode hltEtaStatus = attachFeature(outputTE, trigInfoJetEta, "EFJetInfo"); 
    if (hltEtaStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach TrigOperationalInfo (jet eta) as feature" << endreq;
      return hltEtaStatus;
    }
  
    hltStatus = attachFeature(outputTE, jc, m_jetOutputKey); 
    if (hltStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer (" << m_jetOutputKey << ") as feature jet eta, phi " << jet->eta() << ", " << jet->phi() << endreq;
      return hltStatus;
    }
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}



HLT::ErrorCode TrigJetSplitterAllTE::getAndCheckHistoPrmVtx(HLT::TriggerElement* vtxTriggerElement , const xAOD::VertexContainer* vertices) {
  //std::cout << "checking histoprmvtx " << std::endl;
  HLT::ErrorCode statusHistoPrmVtx = getFeature(vtxTriggerElement, vertices, m_priVtxKeyBackup);
      
  if(statusHistoPrmVtx != HLT::OK){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (PV) " << m_priVtxKeyBackup << endreq;
    return HLT::NAV_ERROR;
  }

  if(vertices == 0){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (vertices): " << m_priVtxKeyBackup << endreq;
    return HLT::MISSING_FEATURE;
  }

  return HLT::OK;
}


const xAOD::Vertex* TrigJetSplitterAllTE::getPrimaryVertex(const xAOD::VertexContainer* vertexContainer)
{
  // vertex types are listed on L328 of                                                                                                                                                                                              
  // https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTracking/trunk/xAODTracking/TrackingPrimitives.h                                                                                                                    
  for( auto vtx_itr : *vertexContainer )
    {
      if(vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) { continue; }
      return vtx_itr;
    }
  Warning("HelperFunctions::getPrimaryVertex()","No primary vertex was found! Returning nullptr");

  return 0;
}


// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrigJetSplitterAllTE.cxx        
//
//            Splits a jet collection into separate TEs
//                   
//  
//
//   $Id: TrigJetSplitterAllTE.cxx, v0.0   Tue 17 Jun 2014 03:26:44 CEST sutt $


#include "GaudiKernel/IToolSvc.h"

#include "TrigBjetHypo/TrigJetSplitterAllTE.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
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
  declareProperty ("JetInputKey",            m_jetInputKey            = "TrigJetRec");
  declareProperty ("JetOutputKey",           m_jetOutputKey           = "SplitJet");
  declareProperty ("PriVtxKey",              m_priVtxKey              = "xPrimVx"); //"EFHistoPrmVtx" 
  declareProperty ("UsePriVtxKeyBackup",     m_usePriVtxKeyBackup     = true);
  declareProperty ("PriVtxKeyBackup",        m_priVtxKeyBackup        = "EFHistoPrmVtx");
  declareProperty ("EtaHalfWidth",           m_etaHalfWidth           = 0.4);
  declareProperty ("PhiHalfWidth",           m_phiHalfWidth           = 0.4);
  declareProperty ("ZHalfWidth",             m_zHalfWidth             = 10.0); // in mm
  declareProperty ("JetMinEt",               m_minJetEt               = 15.0); // in GeV ==> Can't be any higher than the lowest pT chain that will run
  declareProperty ("JetMaxEta",              m_maxJetEta              = 3.2);  //2.5+m_etaHalfWidth);  // tracker acceptance + jet half-width
  declareProperty ("DynamicEtaPhiHalfWidth", m_dynamicEtaPhiHalfWidth = "off"); // "off", "loose", "medium", "tight", "loose_exp", "medium_exp", "tight_exp"
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigJetSplitterAllTE, version " << PACKAGE_VERSION << endmsg;

  //* declareProperty overview *//
  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG( " JetInputKey            = "  << m_jetInputKey           );
  ATH_MSG_DEBUG( " JetOutputKey           = " << m_jetOutputKey           );
  ATH_MSG_DEBUG( " PriVtxKey              = " << m_priVtxKey              );
  ATH_MSG_DEBUG( " EtaHalfWidth           = " << m_etaHalfWidth           );
  ATH_MSG_DEBUG( " PhiHalfWidth           = " << m_phiHalfWidth           );
  ATH_MSG_DEBUG( " ZHalfWidth             = " << m_zHalfWidth             );
  ATH_MSG_DEBUG( " MinJetEt               = " << m_minJetEt               );
  ATH_MSG_DEBUG( " MaxJetEta              = " << m_maxJetEta              );
  ATH_MSG_DEBUG( " DynamicEtaPhiHalfWidth = " << m_dynamicEtaPhiHalfWidth );

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitterAllTE::~TrigJetSplitterAllTE(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  ATH_MSG_DEBUG( "Running TrigJetSplitterAllTE::hltExecute" );

  beforeExecMonitors().ignore();

  ATH_MSG_DEBUG( " inputTEs.size() " << inputTEs.size() );
 
  if (inputTEs.size() == 0) {
    msg() << MSG::WARNING << "No input TEs" << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  if (inputTEs.size() == 1) {
    ATH_MSG_DEBUG( "Only one input TE.  No z-position constraint will be applied!" );
  }

  if (inputTEs.size() > 2) {
    msg() << MSG::WARNING << "Too many TEs passed as input" << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  // -----------------------
  // Retreive jets
  // -----------------------

  std::vector<HLT::TriggerElement*>& jetTE = inputTEs.at(0); // jet TE

  ATH_MSG_DEBUG( " jetTE.size() " << jetTE.size() );

  if (jetTE.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE (jets)" << endmsg;
    afterExecMonitors().ignore();
    return HLT::MISSING_FEATURE; 
  }

  const xAOD::JetContainer* jets = 0;
  //HLT::ErrorCode statusJets = getFeature(jetTE.front(), jets, m_jetInputKey);
  HLT::ErrorCode statusJets = getFeature(jetTE.front(), jets);  // this should really be given a name - need to find out froim the jet guys what!

  if (statusJets != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (jets)" << endmsg;
    return HLT::NAV_ERROR;
  }

  if(jets==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jets)." << endmsg;
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

    ATH_MSG_DEBUG( " vtxTE.size() " << vtxTE.size() );

    if (vtxTE.size() == 0) {
      msg() << MSG::WARNING << "Got an empty inputTE (vertex)" << endmsg;
      afterExecMonitors().ignore();
      return HLT::MISSING_FEATURE; 
    }

    use_z_constraint = true;

    // Get primary vertex 
    const xAOD::VertexContainer* vertexes = nullptr;
    const xAOD::Vertex* prmVtx            = nullptr;
    bool usePVBackup = true;

    if (getFeature(vtxTE.front(), vertexes, m_priVtxKey) == HLT::OK && vertexes != nullptr) {
      ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: " << m_priVtxKey << " has nVertexes = " << vertexes->size() );
      prmVtx = getPrimaryVertex(vertexes);
      if (prmVtx){
	usePVBackup=false;
	ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: valid vertex found in " << m_priVtxKey );
      }
    }


    if(m_usePriVtxKeyBackup && usePVBackup) {
      vertexes = nullptr;
      ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: NO valid vertex found in " << m_priVtxKey << " - proceeding with backup option" );
      if (getFeature(vtxTE.front(), vertexes, m_priVtxKeyBackup) == HLT::OK && vertexes != nullptr) {
	ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: " << m_priVtxKeyBackup << " has nVertexes = " << vertexes->size() );
	prmVtx = getPrimaryVertex(vertexes);	
	if (prmVtx){
	  usePVBackup=false;
	  ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: valid vertex found in " << m_priVtxKeyBackup );
	}
      }
    }


    if(usePVBackup) {
      ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: NO valid vertex found in " << m_priVtxKeyBackup << " - aborting..." );
      return HLT::MISSING_FEATURE;
    }

    prmVtx_z = prmVtx->z();
    ATH_MSG_DEBUG( "Primary vertex z-position = " << prmVtx_z );
  }

  // -----------------------
 
  ATH_MSG_DEBUG( "Found " << jets->size() << " jets, creating corresponding RoIs" );

  HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
  
  unsigned int i = 0;
  for ( xAOD::JetContainer::const_iterator jetitr=jets->begin() ; jetitr!=jets->end() ; jetitr++ ) { 
  
    ++i;
    auto jet = *jetitr;

    float jetEt  = jet->p4().Et()*0.001;
    float jetEta = jet->eta();
    float jetPhi = jet->phi();

    if (jetEt < m_minJetEt) {
      ATH_MSG_DEBUG( "Jet "<< i << " below the " << m_minJetEt << " GeV threshold; Et " << jetEt << "; skipping this jet." );
      continue;
    }
    if (fabs(jetEta) > m_maxJetEta) {
      ATH_MSG_DEBUG( "Jet "<< i << " outside the |eta| < " << m_maxJetEta << " requirement; Eta = " << jetEta << "; skipping this jet." );
      continue;
    }
    ATH_MSG_DEBUG( "Jet "<< i << "; Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi );

    // Create an output TE seeded by an empty vector
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode( initialTE, output );
    outputTE->setActiveState(true);

    // Make deep copy of vertex and attach to output TE

    /// create RoI correspondinding to the jet
    double phiMinus = HLT::wrapPhi(jetPhi-m_phiHalfWidth); 
    double phiPlus  = HLT::wrapPhi(jetPhi+m_phiHalfWidth); 

    double etaMinus = jetEta-m_etaHalfWidth;  
    double etaPlus  = jetEta+m_etaHalfWidth;  

    // ===================================================
    // Experimental pT-dependent Eta/PhiHalfWidth options
    // ===================================================
    if (m_dynamicEtaPhiHalfWidth != "off") {
      ATH_MSG_DEBUG( "Using pT-dependent Eta/PhiHalfWidth (\'" << m_dynamicEtaPhiHalfWidth << "\' working point)" );
      double halfWidth = m_etaHalfWidth;
      if      (m_dynamicEtaPhiHalfWidth == "loose"     ) halfWidth = 0.22 + (3.4/jetEt);
      else if (m_dynamicEtaPhiHalfWidth == "medium"    ) halfWidth = 0.18 + (3.4/jetEt);
      else if (m_dynamicEtaPhiHalfWidth == "tight"     ) halfWidth = 0.14 + (3.4/jetEt);
      else if (m_dynamicEtaPhiHalfWidth == "loose_exp" ) halfWidth = 0.24 + exp(-1.22 - (0.017*jetEt));
      else if (m_dynamicEtaPhiHalfWidth == "medium_exp") halfWidth = 0.20 + exp(-1.22 - (0.017*jetEt));
      else if (m_dynamicEtaPhiHalfWidth == "tight_exp" ) halfWidth = 0.16 + exp(-1.22 - (0.017*jetEt));
      else {
	msg() << MSG::WARNING << m_dynamicEtaPhiHalfWidth << "is not a valid option for DynamicEtaPhiHalfWidth.  " 
	      << "Reverting to default (Eta/PhiHalfWidth = " << m_etaHalfWidth << ")" << endmsg;
      }
      ATH_MSG_DEBUG( "==> Eta/PhiHalfWidth = " << halfWidth );
      m_etaHalfWidth = halfWidth;
      m_phiHalfWidth = halfWidth;

      phiMinus = HLT::wrapPhi(jetPhi-m_phiHalfWidth); 
      phiPlus  = HLT::wrapPhi(jetPhi+m_phiHalfWidth); 

      etaMinus = jetEta-m_etaHalfWidth;  
      etaPlus  = jetEta+m_etaHalfWidth;  
    }


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
      msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature " << *roi << endmsg;
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
    
    hltStatus = attachFeature(outputTE, jc, m_jetOutputKey); 
    if (hltStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer (" << m_jetOutputKey << ") as feature jet eta, phi " << jet->eta() << ", " << jet->phi() << endmsg;
      return hltStatus;
    }
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}



HLT::ErrorCode TrigJetSplitterAllTE::getAndCheckHistoPrmVtx(HLT::TriggerElement* vtxTriggerElement , const xAOD::VertexContainer* vertices) {
  //std::cout << "checking histoprmvtx " << m_priVtxKeyBackup << std::endl;
  HLT::ErrorCode statusHistoPrmVtx = getFeature(vtxTriggerElement, vertices, m_priVtxKeyBackup);
      
  if(statusHistoPrmVtx != HLT::OK){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (PV) " << m_priVtxKeyBackup << endmsg;
    return HLT::NAV_ERROR;
  }

  if(vertices == 0){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (vertices): " << m_priVtxKeyBackup << endmsg;
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
  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "No primary vertex found." << endmsg;

  return 0;
}


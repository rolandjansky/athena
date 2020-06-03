// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   @file    TrigSuperRoiBuilderAllTE.cxx        
//
//            Creates a super ROI from a jet collection
//                   
//  
//   Katharine (dot) Leney (at cern dot ch)
//
//   20th October 2014


#include "GaudiKernel/IToolSvc.h"

#include "TrigBjetHypo/TrigSuperRoiBuilderAllTE.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigSuperRoi.h"
#include "TrigNavigation/TriggerElement.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "xAODJet/JetContainer.h"
#include "AthContainers/ConstDataVector.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigSuperRoiBuilderAllTE::TrigSuperRoiBuilderAllTE(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty ("JetInputKey",     m_jetInputKey     = "TrigJetRec");
  declareProperty ("JetOutputKey",    m_jetOutputKey    = "SuperRoi");
  declareProperty ("EtaHalfWidth",    m_etaHalfWidth    = 0.1);
  declareProperty ("PhiHalfWidth",    m_phiHalfWidth    = 0.1);
  declareProperty ("JetMinEt",        m_minJetEt        = 30.0); // in GeV 
  declareProperty ("JetMaxEta",       m_maxJetEta       = 2.5+m_etaHalfWidth);  // tracker acceptance + jet half-width
  declareProperty ("NJetsMax",        m_nJetsMax        = -1); // option to limit the number of jets that form the super RoI
  declareProperty ("DynamicMinJetEt", m_dynamicMinJetEt = false); // if (X > -1 && nJets > X) minJetEt = m_minJetEt + (nJets-X)*Y
  declareProperty ("DynamicNJetsMax", m_dynamicNJetsMax = 9999); // variable X above
  declareProperty ("DynamicEtFactor", m_dynamicEtFactor = 0); // variable Y above
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigSuperRoiBuilderAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigSuperRoiBuilderAllTE, version " << PACKAGE_VERSION << endmsg;

  //* declareProperty overview *//
  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG( " JetInputKey     = " << m_jetInputKey     );
  ATH_MSG_DEBUG( " JetOutputKey    = " << m_jetOutputKey    );
  ATH_MSG_DEBUG( " EtaHalfWidth    = " << m_etaHalfWidth    );
  ATH_MSG_DEBUG( " PhiHalfWidth    = " << m_phiHalfWidth    );
  ATH_MSG_DEBUG( " MinJetEt        = " << m_minJetEt        );
  ATH_MSG_DEBUG( " MaxJetEta       = " << m_maxJetEta       );
  ATH_MSG_DEBUG( " NJetsMax        = " << m_nJetsMax        );
  ATH_MSG_DEBUG( " DynamicMinJetEt = " << m_dynamicMinJetEt );
  ATH_MSG_DEBUG( " DynamicNJetsMax = " << m_dynamicNJetsMax );
  ATH_MSG_DEBUG( " DynamicEtFactor = " << m_dynamicEtFactor );

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigSuperRoiBuilderAllTE::~TrigSuperRoiBuilderAllTE(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigSuperRoiBuilderAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  ATH_MSG_DEBUG( "Running TrigSuperRoiBuilderAllTE::hltExecute" );

  beforeExecMonitors().ignore();

  // Sanity check that we're not running two optimisations at the same time
  // can be removed once a default method is decided upon
  if (m_nJetsMax > 0 && m_dynamicMinJetEt) {
    msg() << MSG::WARNING << "Two incompatible CPU optimisation methods chosen (DynamicMinJetEt and NJetsMax).  Switching off DynamicMinJetEt." << endmsg;
    m_dynamicMinJetEt = false;
  }

  if (inputTEs.size() != 1) {
    msg() << MSG::WARNING << "Got more than one inputTE" << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  std::vector<HLT::TriggerElement*>& inputTE = inputTEs.at(0);

  if (inputTE.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE" << endmsg;
    afterExecMonitors().ignore();
    return HLT::MISSING_FEATURE; 
  }

  ATH_MSG_DEBUG( " inputTEs.size() " << inputTEs.size() << " inputTE.size() " << inputTE.size() );


  // xAOD conversion const JetCollection* outJets(0);
  const xAOD::JetContainer* jets = 0;
  //HLT::ErrorCode statusJets = getFeature(inputTE.front(), jets, m_jetInputKey);
  HLT::ErrorCode statusJets = getFeature(inputTE.front(), jets); // this should really be given a name - need to find out froim the jet guys what!
  
  if (statusJets != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features" << endmsg;
    return HLT::NAV_ERROR;
  } 

  if(jets==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature." << endmsg;
    return HLT::MISSING_FEATURE;
  }

  ATH_MSG_DEBUG( "Found " << jets->size() << " jets, creating corresponding RoIs" );

  // Create a superROI to add all the jet ROIs to.
  TrigRoiDescriptor* superRoi = new TrigRoiDescriptor();
  superRoi->setComposite(true);

  //HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
  //HLT::TriggerElement* outputTE  = config()->getNavigation()->addNode(initialTE,output);

  std::vector<HLT::TriggerElement*> empty;
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(empty, output);

  outputTE->setActiveState(true);
  std::string key = "";

  int i = 0;
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
   
    // For high pile-up situations, raise the pT threshold of the jets considered after checking the first N (=m_dynamicNJetsMax) jets
    if (m_dynamicMinJetEt && i > m_dynamicNJetsMax ) {
      float dynamicMinJetEt = m_minJetEt + ((i - m_dynamicNJetsMax) * m_dynamicEtFactor); 
      if (jetEt < dynamicMinJetEt) {
	ATH_MSG_DEBUG( "Jet "<< i << " below the dynamic " << dynamicMinJetEt << " GeV ( = " 
		       << m_minJetEt << " + (" << i << " - " << m_dynamicNJetsMax << ") * " << m_dynamicEtFactor << ")"
		       << " threshold; Et " << jetEt << "; skipping this jet." );
	continue;
      }    
    }

    if (m_nJetsMax > 0 && i > m_nJetsMax) {
      ATH_MSG_DEBUG( "Maximum allowed jet multiplicity = "<< m_nJetsMax << "; skipping jet " << i << "." );
      continue;
    }

    ATH_MSG_DEBUG( "Jet "<< i << "; Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi );

    // create RoI correspondinding to the jet
    double phiMinus = HLT::wrapPhi(jetPhi-m_phiHalfWidth); 
    double phiPlus  = HLT::wrapPhi(jetPhi+m_phiHalfWidth); 
    double etaMinus = jetEta-m_etaHalfWidth;  
    double etaPlus  = jetEta+m_etaHalfWidth;  

    TrigRoiDescriptor* roi =  new TrigRoiDescriptor(jetEta, etaMinus, etaPlus, 
						    jetPhi, phiMinus, phiPlus );

    ATH_MSG_DEBUG( "Adding ROI descriptor to superROI!" );
    superRoi->push_back( roi );

    // ConstDataVector<xAOD::JetContainer>* jc = new ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
    // jc->clear(SG::VIEW_ELEMENTS); 
    // jc->push_back ( *jetitr );

    // ... so for the time being do a deep copy
    xAOD::JetTrigAuxContainer trigJetTrigAuxContainer;
    xAOD::JetContainer* jc = new xAOD::JetContainer;
    jc->setStore(&trigJetTrigAuxContainer);
    jc->push_back ( new xAOD::Jet(**jetitr) );

    HLT::ErrorCode hltStatus = attachFeature(outputTE, jc, m_jetOutputKey); 
    if (hltStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer (" << m_jetOutputKey << ") as feature jet eta, phi " << jet->eta() << ", " << jet->phi() << endmsg;
      return hltStatus;
    }

  }

  ATH_MSG_DEBUG( "Attaching feature" );
  // was attached as m_jetOutputKey, now try hardcoding the name
  HLT::ErrorCode hltStatus = attachFeature(outputTE, superRoi, m_jetOutputKey);
  if (hltStatus != HLT::OK) {
    msg() << MSG::ERROR << "Failed to attach SuperRoi as feature" << endmsg;
    return hltStatus;
  }
  
  // Check that we created the superRoi correctly and can retreive it
  // This is really just a sanity check - can probably be removed...
  const TrigRoiDescriptor* newSuperRoi;
  if (getFeature(outputTE, newSuperRoi) != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endmsg;
    
    return HLT::NAV_ERROR;
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigSuperRoiBuilderAllTE::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}




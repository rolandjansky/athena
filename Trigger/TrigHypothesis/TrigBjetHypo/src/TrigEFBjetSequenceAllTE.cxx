/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigEFBjetSequenceAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
//
// ************************************************

#include "GaudiKernel/IToolSvc.h"

#include "TrigBjetHypo/TrigEFBjetSequenceAllTE.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigSuperRoi.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "TrigNavigation/TriggerElement.h"

#include "AthContainers/ConstDataVector.h"

//#include "JetEvent/Jet.h"
//#include "JetEvent/JetCollection.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "FourMomUtils/P4DescendingSorters.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigEFBjetSequenceAllTE::TrigEFBjetSequenceAllTE(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty ("JetInputKey",  m_jetInputKey  = "TrigJetRec");
  declareProperty ("JetOutputKey", m_jetOutputKey = "TrigJetRec");
  declareProperty ("UseSuperRoi",  m_useSuperRoi  = false);
  declareProperty ("EtaHalfWidth", m_etaHalfWidth = 0.2);
  declareProperty ("PhiHalfWidth", m_phiHalfWidth = 0.2);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigEFBjetSequenceAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigEFBjetSequenceAllTE, version " << PACKAGE_VERSION << endmsg;

  //* declareProperty overview *//
  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG( " JetInputKey = "  << m_jetInputKey  );
  ATH_MSG_DEBUG( " JetOutputKey = " << m_jetOutputKey );
  ATH_MSG_DEBUG( " UseSuperRoi  = " << m_useSuperRoi  );
  ATH_MSG_DEBUG( " EtaHalfWidth = " << m_etaHalfWidth );
  ATH_MSG_DEBUG( " PhiHalfWidth = " << m_phiHalfWidth );

  msg() << MSG::INFO << " UseSuperRoi  = " << m_useSuperRoi  << endmsg;

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigEFBjetSequenceAllTE::~TrigEFBjetSequenceAllTE(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigEFBjetSequenceAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {
  
  ATH_MSG_DEBUG( "Running TrigEFBjetSequenceAllTE::hltExecute" );
  
  beforeExecMonitors().ignore();
  
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
  
  const xAOD::JetContainer* jets_EF = 0;
  HLT::ErrorCode statusJets = getFeature(inputTE.front(), jets_EF); // or ""
  const xAOD::JetContainer* jets = jets_EF;
  
  if (statusJets != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) { 
      msg() << MSG::WARNING << "Failed to retrieve features" << endmsg;
    }
    return HLT::NAV_ERROR;
  } 
  
  if(jets_EF==0) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Missing feature." << endmsg;
    return HLT::MISSING_FEATURE;
  }
  
  ATH_MSG_DEBUG( "Found " << jets->size() << " jets, creating corresponding RoIs" );
  
  HLT::TriggerElement* outputTE = 0;
  std::string key = "";
  
  // This is a pretty horrible way to do things but will all change later anyway, so probably doesn't matter too much...
  if ( m_useSuperRoi ) {
    
    ATH_MSG_DEBUG( "Using super ROI" );
    
    TrigRoiDescriptor* superRoi = new TrigRoiDescriptor();
    superRoi->setComposite(true);
    
    // Create an output TE seeded by an empty vector
    HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
    outputTE = config()->getNavigation()->addNode(initialTE,output);
    outputTE->setActiveState(true);
    
    unsigned int i = 0;
    for (xAOD::JetContainer::const_iterator jet = jets->begin(); jet != jets->end(); jet++){
      
      ++i;
      float jet_Et = (*jet)->p4().Et()*0.001;
      
      if (jet_Et < 15) {
	ATH_MSG_DEBUG( "Jet "<< i << " below the 15 GeV threshold; Et " << jet_Et << "; skipping this jet." );
	continue;
      }
      
      ATH_MSG_DEBUG( "Jet "<< i << "; Et " << jet_Et << "; eta "<< (*jet)->eta() << "; phi " << (*jet)->phi() );

      std::cout << "Jet "<< i << "; Et " << jet_Et << "; eta "<< (*jet)->eta() << "; phi " << (*jet)->phi() << std::endl;
      
      // create RoI correspondinding to the jet
      double phiMin = HLT::wrapPhi((*jet)->phi()-m_phiHalfWidth); 
      double phiMax = HLT::wrapPhi((*jet)->phi()+m_phiHalfWidth); 
      
      double etaMin = (*jet)->eta()-m_etaHalfWidth;  
      double etaMax = (*jet)->eta()+m_etaHalfWidth; 
      
      TrigRoiDescriptor* newRoiDescriptor =  new TrigRoiDescriptor((*jet)->eta(), etaMin, etaMax, 
								   (*jet)->phi(), phiMin, phiMax );
      
      ATH_MSG_DEBUG( "Adding ROI descriptor to superROI - ta da!!" );
      superRoi->push_back( newRoiDescriptor );

    }
    
    ATH_MSG_DEBUG( "Attaching feature" );
    HLT::ErrorCode hltStatus = attachFeature(outputTE, superRoi, m_jetOutputKey);
    if (hltStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach SuperRoi as feature" << endmsg;
      return hltStatus;
    }
    
    // Check that we created the superRoi correctly and can retreive it
    const TrigSuperRoi* newSuperRoi;
    if (getFeature(outputTE, newSuperRoi) != HLT::OK) {
      if (msgLvl() <= MSG::WARNING) 
	msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endmsg;
      
      return HLT::NAV_ERROR;
    }
  }
  
  else {
    
    ATH_MSG_DEBUG( "Using normal ROI" );

    unsigned int i = 0;
    for (xAOD::JetContainer::const_iterator jet = jets->begin(); jet != jets->end(); jet++){
      
      ++i;
      float jet_Et = (*jet)->p4().Et()*0.001;
      
      if (jet_Et < 15) {
	ATH_MSG_DEBUG( "Jet "<< i << " below the 15 GeV threshold; Et " << jet_Et << "; skipping this jet." );
	continue;
      }
      
      ATH_MSG_DEBUG( "Jet "<< i << "; Et " << jet_Et << "; eta "<< (*jet)->eta() << "; phi " << (*jet)->phi() );
      
      // Create an output TE seeded by an empty vector
      HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
      outputTE = config()->getNavigation()->addNode(initialTE,output);
      outputTE->setActiveState(true);

      // create RoI correspondinding to the jet
      double phiMin = HLT::wrapPhi((*jet)->phi()-m_phiHalfWidth); 
      double phiMax = HLT::wrapPhi((*jet)->phi()+m_phiHalfWidth); 
      
      double etaMin = (*jet)->eta()-m_etaHalfWidth;  
      double etaMax = (*jet)->eta()+m_etaHalfWidth; 

      TrigRoiDescriptor* newRoiDescriptor =  new TrigRoiDescriptor((*jet)->eta(), etaMin, etaMax,
								   (*jet)->phi(), phiMin, phiMax );
      
      HLT::ErrorCode hltStatus = attachFeature(outputTE, newRoiDescriptor, m_jetOutputKey);
      if ( hltStatus != HLT::OK ) {
	msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature" << endmsg;
	return hltStatus;
      }
      
      // Make a deep copy of the jet, because can't persistify view containers at the moment
      xAOD::JetTrigAuxContainer trigJetTrigAuxContainer;
      xAOD::JetContainer* trigJetContainer = new xAOD::JetContainer();
      trigJetContainer->setStore(&trigJetTrigAuxContainer);
      xAOD::Jet* Jet= new xAOD::Jet(*(*jet));
      trigJetContainer->push_back(Jet);

      hltStatus = attachFeature(outputTE, trigJetContainer, "EFJet"); 
      if (hltStatus != HLT::OK) {
        msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer as feature" << endmsg;
        return hltStatus;
      }
    }
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigEFBjetSequenceAllTE::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}




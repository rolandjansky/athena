/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2CentralJetAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigJetHypo/TrigL2CentralJetAllTE.h"
// #include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include <math.h>
#include <algorithm>


#include "TrigInterfaces/AllTEAlgo.h"

TrigL2CentralJetAllTE::TrigL2CentralJetAllTE(const std::string & name, ISvcLocator* pSvcLocator)
 : HLT::AllTEAlgo(name, pSvcLocator),
   m_executedEvent_L2CentralJet(false)
{
declareProperty("eta_range",   eta_range = 1.1);

declareMonitoredVariable("all_jet0_et",  all_jet0_et);
declareMonitoredVariable("all_jet0_eta", all_jet0_eta);
declareMonitoredVariable("passed_jet0_et",  passed_jet0_et);
declareMonitoredVariable("passed_jet0_eta", passed_jet0_eta);
}

HLT::ErrorCode TrigL2CentralJetAllTE::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Initialization. The correct configuration of this algorithm "
     << "requires jets ordered in increasing energy" << endreq;


  m_acceptedEvts = 0;
  m_rejectedEvts = 0;
  m_errorEvts    = 0;

  return HLT::OK;
}

TrigL2CentralJetAllTE::~TrigL2CentralJetAllTE(){
}

HLT::ErrorCode TrigL2CentralJetAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
				     unsigned int output)
{

  if (m_executedEvent_L2CentralJet) {
     if (msgLvl() <= MSG::DEBUG) {
           msg() << MSG::DEBUG << "*** Not Executing this TrigCheckForTracks " << name() << ", already executed"  << endreq;
     }
     return HLT::OK;
  }

  if (msgLvl() <= MSG::DEBUG) {
     msg() << MSG::DEBUG << "***  Executing this TrigCheckForTracks : " << name() << endreq;
  }

  if (tes_in.size() < 1) {
    msg() << MSG::WARNING << "No TriggerElements provided -> do nothing" << endreq;
    m_executedEvent_L2CentralJet = true;
    m_errorEvts+=1;
    m_rejectedEvts+=1;
    return HLT::MISSING_FEATURE; // since no TE are found while there should be something (why would the chain start otherwise?)
  }


  beforeExecMonitors().ignore();

  const HLT::TEVec& allTEs = tes_in[0];
  
//   // start loop over TEs
//   for (std::vector<HLT::TEVec>::const_iterator it = tes_in.begin();
//        it != tes_in.end(); ++it) {

//     if(it->size()<1) continue;

  if ( allTEs.empty() ){
    return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::MISSING_FEATURE);
  }
  
  
  HLT::TriggerElement *leadingTE(0);
  const TrigT2Jet *leadingJet(0);
  
  // start loop over TE vectors
  for (HLT::TEVec::const_iterator inner_it = allTEs.begin();
       inner_it != allTEs.end(); ++inner_it) {
    if (msgLvl()<=MSG::DEBUG) {
      msg() << MSG::DEBUG  << "TE iterator count = " << inner_it - allTEs.begin() << endreq;
      
      //      const TrigRoiDescriptor* roiDescriptor = 0;
      //      if (getFeature((*inner_it),roiDescriptor) != HLT::OK || roiDescriptor==0)   
      //	msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Element! " << endreq;
      //      else if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG  << "Looking at ROI descriptor" << *roiDescriptor << endreq;
    }
    //allTEs.push_back(*inner_it);
    
    //std::vector<const TrigT2Jet*> vectorOfJets;
    const TrigT2Jet* jet= 0;
    HLT::ErrorCode status = getFeature((*inner_it), jet);
    
    if( (status != HLT::OK) || jet == 0) {
      if (msgLvl()<=MSG::DEBUG) {
	msg() << MSG::DEBUG << " Failed to get TrigT2Jet" << endreq;
      }
    } else if (msgLvl()<=MSG::DEBUG) {
          msg() << MSG::DEBUG << " Got jet " << *jet << endreq;
    }
    if (leadingJet == 0) {
      leadingJet = jet;
      leadingTE  = *inner_it; 
      } else {
      if (jet->et() > leadingJet->et() ) {
	  leadingJet = jet;
	  leadingTE  = *inner_it;
      }
    }
  }
  
  
  //bool passCentralCut = false;
  all_jet0_et  = leadingJet->et();
  all_jet0_eta = leadingJet->eta();
  
  if (fabs(leadingJet->eta()) < eta_range ) {
    
    passed_jet0_et  = leadingJet->et();
    passed_jet0_eta = leadingJet->eta();
    
    //passCentralCut = true;
    HLT::TriggerElement* out_te = config()->getNavigation()->addNode(leadingTE, output);
    out_te->setActiveState(true);
    m_acceptedEvts+=1;
  }
  else m_rejectedEvts+=1;
  
  m_executedEvent_L2CentralJet = true;
  afterExecMonitors().ignore();
  return HLT::OK;
  
}


HLT::ErrorCode TrigL2CentralJetAllTE::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO ) {
    msg() << MSG::INFO << "in finalize()" << endreq;
    msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_acceptedEvts <<" / "<< m_rejectedEvts << " / "<< m_errorEvts << endreq; 
  }
  return HLT::OK;
}




bool TrigL2CentralJetAllTE::reset() {
  if ( !AllTEAlgo::reset() ) return false;
    m_executedEvent_L2CentralJet = false;
    AllTEAlgo::reset();   
    return true;
  }

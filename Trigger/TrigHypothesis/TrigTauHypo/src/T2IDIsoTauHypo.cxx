/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------
// NAME:     TrigIDIsoCaloHypo.cxx
// PACKAGE:  /Trigger/TrigHypothesis/TrigTauHypo/
//
// AUTHOR:   Tofolet, 2012
//
//----------------------------------------
#include <math.h>

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigTauHypo/T2IDIsoTauHypo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


//----------------------------------------
// CONSTRUCTOR:
//----------------------------------------
T2IDIsoTauHypo::T2IDIsoTauHypo(const std::string& name, ISvcLocator* pSvcLocator):
        HLT::HypoAlgo(name, pSvcLocator),
        m_sumPtRatio_Cut(9999.9),
        m_sumPtRatio_1PCut(9999.9),
        m_sumPtRatio_MPCut(9999.9),
        m_cutCounter(0),
        m_sumPtRatio(0.0),
        m_nTracks(0)
{

  // Cuts and their definition...
  /** cut on ratio of sumPt in isolation to core */
  declareProperty("SumPtRatioMax",   m_sumPtRatio_Cut   = 9999.9 );//disabled by default

  /** cut on ratio of sumPt in isolation to core, 1P */
  declareProperty("SumPtRatioMax1P", m_sumPtRatio_1PCut = 9999.9 );//disabled by default

  /** cut on ratio of sumPt in isolation to core, MP */
  declareProperty("SumPtRatioMaxMP", m_sumPtRatio_MPCut = 9999.9 );//disabled by default


  // Cut counter
  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("nTrks",      m_nTracks);
  declareMonitoredVariable("sumPtRatio", m_sumPtRatio);
}



//----------------------------------------
// DESTRUCTOR:
//----------------------------------------
T2IDIsoTauHypo::~T2IDIsoTauHypo()
{  
}

//----------------------------------------
// INITIALIZE:
//----------------------------------------
HLT::ErrorCode T2IDIsoTauHypo::hltInitialize()
{

  if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "in initialize()" << endmsg;
  

  //check that only general or 1P/MP cuts are enabled
  if ( m_sumPtRatio_Cut < 100.0  &&  ( m_sumPtRatio_1PCut < 100.0 ||  m_sumPtRatio_MPCut < 100.0 )  ) {
    msg() << MSG::ERROR << "Configuration error: global and prong-dependent cuts on sumPtRatio enabled!!!: " 
          << "  SumPtRatioMax = " << m_sumPtRatio_Cut
          << ", SumPtRatioMax1P = " << m_sumPtRatio_1PCut
          << ", SumPtRatioMaxMP = " << m_sumPtRatio_MPCut
          << endmsg;
    return HLT::BAD_JOB_SETUP;
  }


  // Print put default settings
  if( m_sumPtRatio_Cut < 100.0 )   msg() << MSG::INFO << "REGTEST: general sumPtRatio <= "               << m_sumPtRatio_Cut << endmsg;
  else                             msg() << MSG::INFO << "REGTEST: cut on general sumPtRatio disabled: " << m_sumPtRatio_Cut << endmsg; 

  if( m_sumPtRatio_1PCut < 100.0 ) msg() << MSG::INFO << "REGTEST: 1P sumPtRatio <= "                    << m_sumPtRatio_1PCut << endmsg;
  else                             msg() << MSG::INFO << "REGTEST: cut on 1P sumPtRatio disabled: "      << m_sumPtRatio_1PCut << endmsg; 

  if( m_sumPtRatio_MPCut < 100.0 ) msg() << MSG::INFO << "REGTEST: MP sumPtRatio <= "                    << m_sumPtRatio_MPCut << endmsg;
  else                             msg() << MSG::INFO << "REGTEST: cut on MP sumPtRatio disabled: "      << m_sumPtRatio_MPCut << endmsg;



  //For internal use set prong dependent cuts in case only general cut is setup
  if ( m_sumPtRatio_Cut < 100.0 ) {
    msg() << MSG::INFO << "General sumPtRatio enabled: " << m_sumPtRatio_Cut
          << ", copying to 1P (" << m_sumPtRatio_1PCut << ") and MP (" <<m_sumPtRatio_MPCut << ") for internal use" << endmsg;
    m_sumPtRatio_1PCut = m_sumPtRatio_Cut;
    m_sumPtRatio_MPCut = m_sumPtRatio_Cut;
  }

 return HLT::OK;
}



// ----------------------------------------------------------------------
HLT::ErrorCode T2IDIsoTauHypo::hltFinalize()
// ----------------------------------------------------------------------
{
  if( msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in finalize()" << endmsg;
  return  HLT::OK;
}



// ----------------------------------------------------------------------
HLT::ErrorCode T2IDIsoTauHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) 
// ----------------------------------------------------------------------
{

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << name() << ": in execute()" << endmsg;

  pass         = false;
  m_cutCounter = 0;
  m_sumPtRatio = -1.0;
  m_nTracks    = -1;   

  //debug
  if(msgLvl() <= MSG::DEBUG) {

    //get RoI descriptor
    const TrigRoiDescriptor* roiDescriptor = 0;
    HLT::ErrorCode status = getFeature(inputTE, roiDescriptor); 
    if ( status != HLT::OK || roiDescriptor == 0 ) msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    else msg() << MSG::DEBUG << "REGTEST: RoI id : "
               << roiDescriptor->roiId() << "/ with LVL1 id :" << roiDescriptor->l1Id()
               << " / located at phi = " <<  roiDescriptor->phi()
               << ", eta = " << roiDescriptor->eta() << endmsg;
    

    //get run/event #, if debug
    const EventInfo* pEventInfo;
    int IdRun=0;
    int IdEvent=0;
    if ( !store() || store()->retrieve(pEventInfo).isFailure() ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endmsg;
    else {
      IdRun   = pEventInfo->event_ID()->run_number();
      IdEvent = pEventInfo->event_ID()->event_number();
      msg() << MSG::DEBUG << "REGTEST: event : " << IdEvent << ", run " << IdRun << endmsg;
    }
  }



  // get tracks
  const TrigTauTracksInfo * tracksInfo(0);
  HLT::ErrorCode status = getFeature(inputTE, tracksInfo);
  if(status != HLT::OK ||tracksInfo  == 0 ){  
    msg() << MSG::DEBUG <<"No input  TrigTauTracksInfo found in the chain" << endmsg;
    return HLT::OK;
  }
  m_cutCounter++;//1
  

  //just for monitoring
  m_nTracks = tracksInfo->nCoreTracks();

  
  pass = true;

  //sumPtRatio
  if( fabs(tracksInfo->scalarPtSumCore()) > 1 ) m_sumPtRatio = tracksInfo->scalarPtSumIso() / tracksInfo->scalarPtSumCore();
  else                                          m_sumPtRatio = tracksInfo->scalarPtSumIso() *(1. / 40000.); // assume 40 GeV is average candidate. 

  if ( m_nTracks == 1 && m_sumPtRatio_1PCut < 100.0 && m_sumPtRatio >= m_sumPtRatio_1PCut) pass = false;
  if ( m_nTracks >  1 && m_sumPtRatio_MPCut < 100.0 && m_sumPtRatio >= m_sumPtRatio_MPCut) pass = false;
  if( pass )  m_cutCounter++;//2


  //done!!!
  if( msgLvl() <= MSG::DEBUG ) {
    if ( m_sumPtRatio_1PCut < 100.0 || m_sumPtRatio_MPCut ) 
                msg() << MSG::DEBUG << "REGTEST: sumPt_core = " << tracksInfo->scalarPtSumCore() 
                      << ", sumPt_isol = " << tracksInfo->scalarPtSumIso() 
                      << ", ratio = "<< m_sumPtRatio <<endmsg;
    
    if ( pass ) msg() << MSG::DEBUG << "REGTEST: RoI is accepted ! " << endmsg;
    else        msg() << MSG::DEBUG << "REGTEST: RoI is rejected!! " << endmsg;
  }
  
  return HLT::OK;
}


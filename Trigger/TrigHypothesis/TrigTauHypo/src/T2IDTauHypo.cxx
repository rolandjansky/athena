/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------
// NAME:     TrigIDCaloHypo.cxx
// PACKAGE:  /Trigger/TrigHypothesis/TrigTauHypo/
//
//----------------------------------------
#include <math.h>

#include "GaudiKernel/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "T2IDTauHypo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


T2IDTauHypo::T2IDTauHypo(const std::string& name, ISvcLocator* pSvcLocator):
        HLT::HypoAlgo(name, pSvcLocator),
        m_nTrkMin_Cut(9999),
        m_nTrkMax_Cut(-1),
        m_sumPtRatio_Cut(9999.9),
        m_sumPtRatio_1PCut(9999.9),
        m_sumPtRatio_MPCut(9999.9),
        m_ldTrkPt_Cut(-9999.9),
        m_cutCounter(0),
        m_nTracks(0),
        m_sumPtRatio(0.0),
        m_ldTrkPt(0.0)
{
  
  // Cuts and their definition...
  /** Cut on minimum number of tracks Ntrk >= NtrkMinCut */
  declareProperty("NTrkMin",         m_nTrkMin_Cut      = 9999 ); 

  /** Cut on maximum number of tracks Ntrk <= NtrkMaxCut */
  declareProperty("NTrkMax",         m_nTrkMax_Cut      = -1 ); 

  /** cut on ratio of sumPt in isolation to core */
  declareProperty("SumPtRatioMax",   m_sumPtRatio_Cut   = 9999.9 );//disabled by default

  /** cut on ratio of sumPt in isolation to core, 1P */
  declareProperty("SumPtRatioMax1P", m_prop_sumPtRatio_1PCut = 9999.9 );//disabled by default

  /** cut on ratio of sumPt in isolation to core, MP */
  declareProperty("SumPtRatioMaxMP", m_prop_sumPtRatio_MPCut = 9999.9 );//disabled by default

  /** leading track pt, used by HadCalib chains in 2011 */
  declareProperty("LdTrkPtMin",      m_ldTrkPt_Cut      = -9999.9 *Gaudi::Units::GeV ); //disabled by default

  /** Cut counter */
  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("nTrks",      m_nTracks);
  declareMonitoredVariable("sumPtRatio", m_sumPtRatio);
  declareMonitoredVariable("ldTrkPt",    m_ldTrkPt);
}

T2IDTauHypo::~T2IDTauHypo()
{  }


// ----------------------------------------------------------------------
HLT::ErrorCode T2IDTauHypo::hltInitialize()
// ----------------------------------------------------------------------
{

  m_sumPtRatio_MPCut = m_prop_sumPtRatio_MPCut.value();
  m_sumPtRatio_1PCut = m_prop_sumPtRatio_1PCut.value();

  if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "in initialize()" << endmsg;
  
  //check that at least one track if LdTrkPt required
  if ( m_ldTrkPt_Cut >= 0.0 && m_nTrkMin_Cut < 1 ) {
    msg() << MSG::WARNING 
          << "Misconfiguration: cut on leading track pt enabled: ldTrkPtMin= " << m_ldTrkPt_Cut
          << ", but not requiring at least one track: nTrkMin = " << m_nTrkMin_Cut
          << ". Changing to nTrkMin = 1 " << endmsg;
    m_nTrkMin_Cut = 1;
  }


  //check that at nTrkMax>=nTrkMin
  if ( m_nTrkMin_Cut > m_nTrkMax_Cut ) {
    msg() << MSG::ERROR << "Configuration error: " 
	  << " m_nTrkMin_Cut ("<< m_nTrkMin_Cut<< ") > m_nTrkMax_Cut (" <<m_nTrkMax_Cut  << "),"
	  << " algorithm is uninitialized"
	  << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  

  //check that only general or 1P/MP cuts are enabled
  if ( m_sumPtRatio_Cut < 100.0  &&  ( m_sumPtRatio_1PCut < 100.0 ||  m_sumPtRatio_MPCut < 100.0 )  ) {
    msg() << MSG::ERROR << "Configuration error: global and prong-dependent cuts on sumPtRatio enabled!!!: " 
          << "  SumPtRatioMax = " << m_sumPtRatio_Cut
          << ", SumPtRatioMax1P = " << m_sumPtRatio_1PCut
          << ", SumPtRatioMaxMP = " << m_sumPtRatio_MPCut
          << endmsg;
    return HLT::BAD_JOB_SETUP;
  }


  // Print settings
  msg() << MSG::INFO << "REGTEST: " << m_nTrkMin_Cut << " <= nTracks <= "<<  m_nTrkMax_Cut << endmsg;
  
  if( m_sumPtRatio_Cut < 100.0 )   msg() << MSG::INFO << "REGTEST: general sumPtRatio <= "               << m_sumPtRatio_Cut << endmsg;
  else                             msg() << MSG::INFO << "REGTEST: cut on general sumPtRatio disabled: " << m_sumPtRatio_Cut << endmsg; 

  if( m_sumPtRatio_1PCut < 100.0 ) msg() << MSG::INFO << "REGTEST: 1P sumPtRatio <= "                    << m_sumPtRatio_1PCut << endmsg;
  else                             msg() << MSG::INFO << "REGTEST: cut on 1P sumPtRatio disabled: "      << m_sumPtRatio_1PCut << endmsg; 

  if( m_sumPtRatio_MPCut < 100.0 ) msg() << MSG::INFO << "REGTEST: MP sumPtRatio <= "                    << m_sumPtRatio_MPCut << endmsg;
  else                             msg() << MSG::INFO << "REGTEST: cut on MP sumPtRatio disabled: "      << m_sumPtRatio_MPCut << endmsg; 
  
  
  if ( m_ldTrkPt_Cut >= 0.0 )      msg() << MSG::INFO << "REGTEST: leading track pt => "               << m_ldTrkPt_Cut << endmsg;
  else                             msg() << MSG::INFO << "REGTEST: cut on leading track pt disabled: " << m_ldTrkPt_Cut << endmsg;
  


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
HLT::ErrorCode T2IDTauHypo::hltFinalize()
// ----------------------------------------------------------------------
{
  if( msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in finalize()" << endmsg;
  return  HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode T2IDTauHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) 
// ----------------------------------------------------------------------
{
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << name() << ": in execute()" << endmsg;
  
  pass         = false;
  m_cutCounter = 0;
  m_nTracks    = -1;   
  m_sumPtRatio = -1.0;
  m_ldTrkPt    = -1.0;
  

  //debug
  if(msgLvl() <= MSG::DEBUG) {
    const TrigRoiDescriptor* roiDescriptor = 0;
    HLT::ErrorCode status = getFeature(inputTE, roiDescriptor); 
    if ( status != HLT::OK || roiDescriptor == 0 ) msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    else msg() << MSG::DEBUG << "REGTEST: RoI id : "
               << roiDescriptor->roiId() << "/ with LVL1 id :" << roiDescriptor->l1Id()
               << " / located at phi = " <<  roiDescriptor->phi()
               << ", eta = " << roiDescriptor->eta() << endmsg;
    
    
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

  
  //get tracks
  const TrigTauTracksInfo * tracksInfo(0);
  HLT::ErrorCode status = getFeature(inputTE, tracksInfo);
  if(status != HLT::OK ||tracksInfo  == 0 ){  
    msg() << MSG::DEBUG << "No input TrigTauTracksInfo found in the chain" << endmsg;
    return HLT::OK;
  }
  m_cutCounter++;//1
  

  pass = true;

  //nTrks
  m_nTracks = tracksInfo->nCoreTracks();
  if( m_nTracks < m_nTrkMin_Cut || m_nTracks > m_nTrkMax_Cut  ) pass = false;
  if( pass ) m_cutCounter++;//2
  
  
  //LdTrkPt
  m_ldTrkPt = tracksInfo->leadingTrackPt();
  if ( m_ldTrkPt_Cut >= 0.0 && m_ldTrkPt < m_ldTrkPt_Cut ) pass = false;
  if ( pass ) m_cutCounter++;//3
  
  
  //sumPtRatio
  if( fabs(tracksInfo->scalarPtSumCore()) > 1 ) m_sumPtRatio = tracksInfo->scalarPtSumIso() / tracksInfo->scalarPtSumCore();
  else                                          m_sumPtRatio = tracksInfo->scalarPtSumIso() *(1. / 40000.); // assume 40 GeV is average candidate. 

  if ( m_nTracks == 1 && m_sumPtRatio_1PCut < 100.0 && m_sumPtRatio >= m_sumPtRatio_1PCut) pass = false;
  if ( m_nTracks >  1 && m_sumPtRatio_MPCut < 100.0 && m_sumPtRatio >= m_sumPtRatio_MPCut) pass = false;
  if( pass )  m_cutCounter++;//4
  
  
  //debug print out
  if( msgLvl() <= MSG::DEBUG ) {
                                msg() << MSG::DEBUG << "REGTEST: nTrks = "      << m_nTracks << endmsg ;

    if ( m_ldTrkPt_Cut >= 0.0 ) msg() << MSG::DEBUG << "REGTEST: ldTrkPt = "    << m_ldTrkPt << endmsg;

    if ( m_sumPtRatio_1PCut < 100.0 || m_sumPtRatio_MPCut ) 
                                msg() << MSG::DEBUG << "REGTEST: sumPt_core = " << tracksInfo->scalarPtSumCore() 
                                      << ", sumPt_isol = " << tracksInfo->scalarPtSumIso() 
                                      << ", ratio = "<< m_sumPtRatio <<endmsg;
    
    if ( pass ) msg() << MSG::DEBUG << "REGTEST: RoI is accepted ! " << endmsg;
    else        msg() << MSG::DEBUG << "REGTEST: RoI is rejected!! " << endmsg;
  }
  
  return HLT::OK;
}


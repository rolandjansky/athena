/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigIDCaloHypo.cxx
// PACKAGE:  /Trigger/TrigHypothesis/TrigTauHypo/
//
// ********************************************************************
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigParticle/TrigTau.h"
#include "TrigTauHypo/T2TauHypo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


T2TauHypo::T2TauHypo(const std::string& name, ISvcLocator* pSvcLocator):
        HLT::HypoAlgo(name, pSvcLocator),
        m_emRadius3S1p_Cut(-9999.9),
        m_etOverPtLeadTrk_Cut(9999.9),
        m_etOverPtLeadTrk_1PCut(9999.9),
        m_etOverPtLeadTrk_MPCut(9999.9),
        m_trkAvgDist_Cut(9999.9),
        m_trkAvgDist_1PCut(9999.9),
        m_trkAvgDist_MPCut(9999.9),
        m_cutCounter(0),
        m_nTrks2011(0),
        m_nTrks2012(0),
        m_etOverPtLeadTrk(0.0),
        m_trkAvgDist(0.0)
{
  
  // Cuts and their definition...
  /** EMRadius3S for 1p */
  declareProperty("EmRadius3S1pMax",      m_emRadius3S1p_Cut      = 9999.9 ); //disabled by default

  /** etOverPtLeadTrk */
  declareProperty("EtOverPtLeadTrkMax",   m_etOverPtLeadTrk_Cut   = 9999.9 ); //disabled by default
  /** etOverPtLeadTrk 1P*/
  declareProperty("EtOverPtLeadTrkMax1P", m_etOverPtLeadTrk_1PCut = 9999.9 ); //disabled by default
  /** etOverPtLeadTrk MP*/
  declareProperty("EtOverPtLeadTrkMaxMP", m_etOverPtLeadTrk_MPCut = 9999.9 ); //disabled by default

  /** trkAvgDist */
  declareProperty("TrkAvgDistMax",        m_trkAvgDist_Cut        = 9999.9 ); //disabled by default
  /** trkAvgDist 1P */
  declareProperty("TrkAvgDistMax1P",      m_trkAvgDist_1PCut      = 9999.9 ); //disabled by default
  /** trkAvgDist MP */
  declareProperty("TrkAvgDistMaxMP",      m_trkAvgDist_MPCut      = 9999.9 ); //disabled by default

  
  /** Cut counter */
  declareMonitoredVariable("CutCounter",      m_cutCounter);
  declareMonitoredVariable("nTrks2011",       m_nTrks2011);
  declareMonitoredVariable("nTrks2012",       m_nTrks2012);
  declareMonitoredVariable("etOverPtLeadTrk", m_etOverPtLeadTrk);
  declareMonitoredVariable("trkAvgDist",      m_trkAvgDist);

}

T2TauHypo::~T2TauHypo()
{  }


// ----------------------------------------------------------------------
HLT::ErrorCode T2TauHypo::hltInitialize()
// ----------------------------------------------------------------------
{

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in initialize()" << endmsg;

  //EtOverPtLeadTrk check that only general or 1P/MP cuts are enabled
  if ( m_etOverPtLeadTrk_Cut < 100.0 && ( m_etOverPtLeadTrk_1PCut < 100.0 || m_etOverPtLeadTrk_MPCut < 100.0 ) ) {
    msg() << MSG::ERROR << "Configuration error: global and prong-dependent cuts on EtOverPtLeadTrk enabled!!!: " 
          << "  EtOverPtLeadTrkMax = "   << m_etOverPtLeadTrk_Cut
          << ", EtOverPtLeadTrkMax1P = " << m_etOverPtLeadTrk_1PCut
          << ", EtOverPtLeadTrkMaxMP = " << m_etOverPtLeadTrk_MPCut
          << endmsg;
    return HLT::BAD_JOB_SETUP;
  }


  //TrkAvgDist check that only general or 1P/MP cuts are enabled
  if ( m_trkAvgDist_Cut < 100.0 && ( m_trkAvgDist_1PCut < 100.0 || m_trkAvgDist_MPCut < 100.0 ) ) {
    msg() << MSG::ERROR << "Configuration error: global and prong-dependent cuts on TrkAvgDist enabled!!!: " 
          << "  TrkAvgDistMax = "   << m_trkAvgDist_Cut
          << ", TrkAvgDistMax1P = " << m_trkAvgDist_1PCut
          << ", TrkAvgDistMaxMP = " << m_trkAvgDist_MPCut
          << endmsg;
    return HLT::BAD_JOB_SETUP;
  }



  if ( m_emRadius3S1p_Cut      < 100.0 ) msg() << MSG::INFO << "REGTEST: EmRadius3S1p cut = "                     << m_emRadius3S1p_Cut << endmsg;
  else                                   msg() << MSG::INFO << "REGTEST: EmRadius3S1p cut disabled : "            << m_emRadius3S1p_Cut << endmsg;
  
  if ( m_etOverPtLeadTrk_Cut   < 100.0 ) msg() << MSG::INFO << "REGTEST: EtOverPtLeadTrk general cut = "          << m_etOverPtLeadTrk_Cut << endmsg;
  else                                   msg() << MSG::INFO << "REGTEST: EtOverPtLeadTrk general cut disabled : " << m_etOverPtLeadTrk_Cut << endmsg;
  if ( m_etOverPtLeadTrk_1PCut < 100.0 ) msg() << MSG::INFO << "REGTEST: EtOverPtLeadTrk 1P cut = "               << m_etOverPtLeadTrk_1PCut << endmsg;
  else                                   msg() << MSG::INFO << "REGTEST: EtOverPtLeadTrk 1P cut disabled : "      << m_etOverPtLeadTrk_1PCut << endmsg;
  if ( m_etOverPtLeadTrk_MPCut < 100.0 ) msg() << MSG::INFO << "REGTEST: EtOverPtLeadTrk MP cut = "               << m_etOverPtLeadTrk_MPCut << endmsg;
  else                                   msg() << MSG::INFO << "REGTEST: EtOverPtLeadTrk MP cut disabled : "      << m_etOverPtLeadTrk_MPCut << endmsg;

  if ( m_trkAvgDist_Cut        < 100.0 ) msg() << MSG::INFO << "REGTEST: TrkAvgDist general cut = "               << m_trkAvgDist_Cut << endmsg;
  else                                   msg() << MSG::INFO << "REGTEST: TrkAvgDist general cut disabled : "      << m_trkAvgDist_Cut << endmsg;
  if ( m_trkAvgDist_1PCut      < 100.0 ) msg() << MSG::INFO << "REGTEST: TrkAvgDist 1P cut = "                    << m_trkAvgDist_1PCut << endmsg;
  else                                   msg() << MSG::INFO << "REGTEST: TrkAvgDist 1P cut disabled : "           << m_trkAvgDist_1PCut << endmsg;
  if ( m_trkAvgDist_MPCut      < 100.0 ) msg() << MSG::INFO << "REGTEST: TrkAvgDist MP cut = "                    << m_trkAvgDist_MPCut << endmsg;
  else                                   msg() << MSG::INFO << "REGTEST: TrkAvgDist MP cut disabled : "           << m_trkAvgDist_MPCut << endmsg;


  //For internal use set prong dependent cuts in case only general cut is setup
  if ( m_etOverPtLeadTrk_Cut   < 100.0 ) {
    msg() << MSG::INFO << "General etOverPtLeadTrk enabled: " << m_etOverPtLeadTrk_Cut 
          << ", copying to 1P (" << m_etOverPtLeadTrk_1PCut << ") and MP (" << m_etOverPtLeadTrk_MPCut << ") for internal use" << endmsg;
    m_etOverPtLeadTrk_1PCut = m_etOverPtLeadTrk_Cut;
    m_etOverPtLeadTrk_MPCut = m_etOverPtLeadTrk_Cut;
  }

  if ( m_trkAvgDist_Cut   < 100.0 ) {
    msg() << MSG::INFO << "General trkAvgDist enabled: " << m_trkAvgDist_Cut 
          << ", copying to 1P (" << m_trkAvgDist_1PCut << ") and MP (" << m_trkAvgDist_MPCut << ") for internal use" << endmsg;
    m_trkAvgDist_1PCut = m_trkAvgDist_Cut;
    m_trkAvgDist_MPCut = m_trkAvgDist_Cut;
  }



  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode T2TauHypo::hltFinalize()
// ----------------------------------------------------------------------
{
  return  HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode T2TauHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass)
// ----------------------------------------------------------------------
{
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << name() << ": in execute()" << endmsg;

  pass         = false;
  m_cutCounter = 0;
  
  
  // Print debug info of RoIDescriptor: eta/phi/RoIid/evt number
  if(msgLvl() <= MSG::DEBUG) {
    // we actually do not use eta,phi below, so this is only for DEBUG info.
    const TrigRoiDescriptor* roiDescriptor = 0;
    HLT::ErrorCode status = getFeature(inputTE, roiDescriptor); 
    if ( status != HLT::OK || roiDescriptor == 0 ) {
      msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
      //return status;
    } else {
    
      const EventInfo* pEventInfo;
      //int IdRun=0;
      int IdEvent=0;
      if ( !store() || store()->retrieve(pEventInfo).isFailure() ) {
        msg()  << MSG::DEBUG << "Failed to get EventInfo " << endmsg;
      } else {
        //IdRun   = pEventInfo->event_ID()->run_number();
        IdEvent = pEventInfo->event_ID()->event_number();
        msg() << MSG::DEBUG << "REGTEST: event : " << IdEvent << "/ RoI id : "
            << roiDescriptor->roiId() << "/ with LVL1 id :" << roiDescriptor->l1Id()
            << " / located at phi = " <<  roiDescriptor->phi()
            << ", eta = " << roiDescriptor->eta() << endmsg;
      }
    }
  }
  
  
  pass = true;

  //EmRadius3S cut
  float emRad3S = -9999.9; //use unphysical values if cut not used
  m_nTrks2011   = -1;      //use unphysical values if cut not used
  if ( m_emRadius3S1p_Cut < 100.0 ) {

    //get cluster
    const TrigTauCluster* pClus(0);
    HLT::ErrorCode status = getFeature(inputTE, pClus);
    if(status != HLT::OK || pClus == 0) {
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"No input TrigTauCluster* is found in the chain" << endmsg;
    }

    //and tracks
    const TrigTauTracksInfo* tracksInfo(0);
    status = getFeature(inputTE, tracksInfo);
    if(status != HLT::OK || tracksInfo == 0 ){
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"No input TrigTauTracksInfo* is found in the chain" << endmsg;
    }

    //apply cut
    if (pClus != 0 && tracksInfo != 0) {
      emRad3S   = pClus->EMRadius3S();
      m_nTrks2011 = tracksInfo->nCoreTracks() + tracksInfo->nIsoTracks();//2011 definition!!!
      if ( m_nTrks2011 == 1 && emRad3S > m_emRadius3S1p_Cut ) pass = false;
    } else {
      msg() << MSG::DEBUG << "m_emRadius3S1p_Cut cannot be applied : " << m_emRadius3S1p_Cut << ", accept event" << endmsg;
    }
  }
  if ( pass ) m_cutCounter++;//1
  



  //get trigger tau
  const TrigTau* tau(0);
  const TrigTauTracksInfo* tracksInfo(0);
  if ( m_etOverPtLeadTrk_1PCut < 100.0 || m_etOverPtLeadTrk_MPCut < 100.0 || m_trkAvgDist_1PCut < 100.0  || m_trkAvgDist_MPCut < 100.0) {
    HLT::ErrorCode status = getFeature(inputTE, tau);
    if(status != HLT::OK || tau == 0 ){
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"No input TrigTau* is found in the chain" << endmsg;
    }
    
    //and tracks
    status = getFeature(inputTE, tracksInfo);
    if(status != HLT::OK || tracksInfo == 0 ){
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"No input TrigTauTracksInfo* is found in the chain" << endmsg;
    }
  }



  //etOvLdTrkPt
  m_etOverPtLeadTrk = -9999.9; //use unphysical values if cut not used
  m_nTrks2012       = -1;      //use unphysical values if cut not used
  if ( m_etOverPtLeadTrk_1PCut < 100.0 ||  m_etOverPtLeadTrk_MPCut < 100.0) {

    if ( tau != 0 && tracksInfo != 0 ) {
      m_etOverPtLeadTrk = tau->etOverPtLeadTrk();
      m_nTrks2012       = tracksInfo->nCoreTracks();//2012 definition of prongness!!
      
      if ( m_nTrks2012 == 1 && m_etOverPtLeadTrk_1PCut < 100.0 && m_etOverPtLeadTrk > m_etOverPtLeadTrk_1PCut ) pass = false;
      if ( m_nTrks2012 > 1  && m_etOverPtLeadTrk_MPCut < 100.0 && m_etOverPtLeadTrk > m_etOverPtLeadTrk_MPCut ) pass = false;

    } else {
      msg() << MSG::DEBUG << "m_etOverPtLeadTrk_Cut cannot be applied (1P: " << m_etOverPtLeadTrk_1PCut << ", MP: " << m_etOverPtLeadTrk_MPCut << "), accept event" << endmsg;  
    }
  }
  if ( pass ) m_cutCounter++;//2
  

  
  //trkAvgDist
  m_trkAvgDist = -9999.9;//use unphysical values if cut not used
  if ( m_trkAvgDist_1PCut < 100.0 || m_trkAvgDist_MPCut < 100.0 ) {

    if ( tau != 0 && tracksInfo != 0 ) {
      m_trkAvgDist = tau->trkAvgDist();
      m_nTrks2012  = tracksInfo->nCoreTracks();//2012 definition of prongness!!

      if ( m_nTrks2012 == 1 && m_trkAvgDist_1PCut < 100.0 && m_trkAvgDist > m_trkAvgDist_1PCut ) pass = false;
      if ( m_nTrks2012 >  1 && m_trkAvgDist_MPCut < 100.0 && m_trkAvgDist > m_trkAvgDist_MPCut ) pass = false;

    } else {
      msg() << MSG::DEBUG << "m_trkAvgDist_Cut cannot be applied (1P: " << m_trkAvgDist_1PCut << ", MP: " << m_trkAvgDist_MPCut << "), accept event" << endmsg;  
    }
  }
  if ( pass ) m_cutCounter++;//3
  
  
  //debug print outs
  if(msgLvl() <= MSG::DEBUG) {
    if ( m_emRadius3S1p_Cut < 100.0 )    msg() << MSG::DEBUG << "REGTEST: nTrks2011 = " << m_nTrks2011 << endmsg;
    if ( m_emRadius3S1p_Cut < 100.0 )    msg() << MSG::DEBUG << "REGTEST: emRad3S = "   << emRad3S     << endmsg;

    if ( m_etOverPtLeadTrk_1PCut < 100.0 || m_etOverPtLeadTrk_MPCut < 100.0 ) msg() << MSG::DEBUG << "REGTEST: etOverPtLeadTrk = " << m_etOverPtLeadTrk << endmsg;
    if ( m_trkAvgDist_1PCut < 100.0      || m_trkAvgDist_MPCut < 100.0 )      msg() << MSG::DEBUG << "REGTEST: trkAvgDist= "       << m_trkAvgDist      << endmsg;

    if ( m_etOverPtLeadTrk_1PCut < 100.0 || m_etOverPtLeadTrk_MPCut < 100.0 ||  
         m_trkAvgDist_1PCut < 100.0      || m_trkAvgDist_MPCut < 100.0 ) 
        msg() << MSG::DEBUG << "REGTEST: nTrks2012 = " << m_nTrks2012 << endmsg;
    
    if (pass) msg() << MSG::DEBUG << "REGTEST: RoI is accepted !! " << endmsg;
    else      msg() << MSG::DEBUG << "REGTEST: RoI is rejected !! " << endmsg;
  }
  
  return HLT::OK;
}

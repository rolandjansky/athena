/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2CaloTauHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigTauHypo/T2CaloTauHypo.h"
#include "TrigTauHypo/ITrigTauClusterTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//#include "TrigInDetEvent/TrigInDetTrack.h"

class ISvcLocator;

T2CaloTauHypo::T2CaloTauHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HypoAlgo(name, pSvcLocator),
  m_etRaw_Cut(0.0),
  m_emRadius3S_Cut(9999.9),
  m_coreFraction_Cut(-9999.9),
  m_cutCounter(0)
{

  /** Raw energy, cone size defined in FEX!! */
  declareProperty( "EtRawMin",            m_etRaw_Cut        = 0.0);
  /** EMRadius3S, cone size defined in FEX!! */
  declareProperty( "EmRadius3SMax",       m_emRadius3S_Cut   = 9999.9);//disabled by default
  /** Core Fraction  */
  declareProperty( "CoreFractionMin",     m_coreFraction_Cut = -9999.9);//disabled by default

  /** Variable for monitoring */
  declareMonitoredVariable( "CutCounter", m_cutCounter       = 0);
}

T2CaloTauHypo::~T2CaloTauHypo(){
}

HLT::ErrorCode T2CaloTauHypo::hltInitialize(){

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: initialization..." << endmsg;

  msg() << MSG::INFO << "REGTEST: EtRaw cut = " <<  m_etRaw_Cut << endmsg;

  if ( m_emRadius3S_Cut < 100.0 ) msg() << MSG::INFO << "REGTEST: EmRadius3S cut = "   <<  m_emRadius3S_Cut << endmsg;
  else                            msg() << MSG::INFO << "REGTEST: EmRadius3S cut disabled" << endmsg;

  if ( m_coreFraction_Cut > 0.0 ) msg() << MSG::INFO << "REGTEST: CoreFraction cut = " <<  m_coreFraction_Cut <<  endmsg;
  else                            msg() << MSG::INFO << "REGTEST: CoreFraction cut disabled" << endmsg;
      
  
  return HLT::OK;
}

HLT::ErrorCode T2CaloTauHypo::hltFinalize(){
  return HLT::OK;
}

HLT::ErrorCode T2CaloTauHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endmsg;

  pass         = false;
  m_cutCounter = 0;

  
  // Some debug output:
  if(msgLvl() <= MSG::DEBUG) {
    const TrigRoiDescriptor* roiDescriptor = 0;
    HLT::ErrorCode status = getFeature(outputTE, roiDescriptor); 
    if ( status == HLT::OK && roiDescriptor != 0 ) {
      msg() << MSG::DEBUG << "REGTEST: " //event : " << IdEvent  
            << "/ RoI id : " << roiDescriptor->roiId() << "/ with LVL1 id :" << roiDescriptor->l1Id() 
            << " / located at phi = " <<  roiDescriptor->phi()
            << ", eta = " << roiDescriptor->eta() << endmsg;
    }
  }
  
  
  // Get vector of of pointers to all TrigTauClusters with label "uses":
  std::vector<const TrigTauCluster*> vectorOfClusters; 
  HLT::ErrorCode hltstatus = getFeatures(outputTE, vectorOfClusters, ""); 


  if( hltstatus != HLT::OK ) {
     msg() << MSG::ERROR << "Retrieval of std::vector<TrigTauCluster*> failed" << endmsg;
     return hltstatus;
  }
  m_cutCounter++;//1

  
  // Check that there is only one RecEmTauROI. We only expect ONE input RoI.
  if (vectorOfClusters.size() != 1){
    msg() << MSG::ERROR << "Size of std::vector<TrigTauCluster*> is not 1" << endmsg;
    return HLT::OK;
  }
  m_cutCounter++;//2


  // Get first (and only) RoI:
  const TrigTauCluster* pCluster = vectorOfClusters.front();
  if(!pCluster){
    msg() << MSG::ERROR << "Retrieval of RoI from vector failed"  << endmsg;
    return HLT::OK;
  }
  m_cutCounter++;//3


  pass = true;
  //Energy 
  float etRaw = ( pCluster->EMenergy() + pCluster->HADenergy() ) / cosh( pCluster->eta() );
  if ( etRaw < m_etRaw_Cut )  pass = false;
  if ( pass ) m_cutCounter++;//4


  //EmRadius3S
  float emRad3S = pCluster->EMRadius3S();
  if ( m_emRadius3S_Cut < 100.0 && emRad3S > m_emRadius3S_Cut ) pass = false;
  if ( pass ) m_cutCounter++;//5


  //Core Fraction
  float coreFrac = pCluster->CoreFrac();
  if ( m_coreFraction_Cut > 0.0 && coreFrac <  m_coreFraction_Cut ) pass = false;
  if ( pass ) m_cutCounter++;//6


  // Reach this point successfully
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST: etRaw :"        << etRaw    << endmsg;
    if ( m_emRadius3S_Cut < 100.0 ) msg() << MSG::DEBUG << "REGTEST: emRad3S :"      << emRad3S  << endmsg;
    if ( m_coreFraction_Cut > 0.0 ) msg() << MSG::DEBUG << "REGTEST: CoreFraction :" << coreFrac << endmsg;

    if ( pass ) msg() << MSG::DEBUG << "REGTEST: RoI is accepted " << endmsg;
    else        msg() << MSG::DEBUG << "REGTEST: RoI is rejected " << endmsg;
  }

  return HLT::OK;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFJetHypoNoise.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR: Denis Oliveira Damazio
//
// ********************************************************************
//
#include <algorithm>
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

//#include "TrigConfHLTData/HLTTriggerElement.h"

//#include "TrigSteeringEvent/TriggerElement.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/Enums.h"

#include "TrigJetHypo/TrigEFJetHypoNoise.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetHypoNoise::TrigEFJetHypoNoise(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Etcut",   m_EtCut = 40*CLHEP::GeV); // Default: 40 GeV
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);

  declareProperty( "BadFEBCut", m_MinBadFEB=5 );

  declareMonitoredVariable("CutCounter", m_cutCounter);

  // Monitored variables...
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
}

TrigEFJetHypoNoise::~TrigEFJetHypoNoise()
{  }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetHypoNoise::hltInitialize()
  // ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endreq;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	  << ": Unable to locate TrigTimer Service" << endreq;
  }
  if (m_timersvc) {

    TrigTimer* tmp = m_timersvc->addItem("TrigEFJetHypoNoise.TrigEFJetHypoTot");
    m_timers.push_back(tmp);
  }

  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  
  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetHypoNoise::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endreq;
  return HLT::OK;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetHypoNoise::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------


  // Time total TrigEFJetHypo execution time.
  // -------------------------------------
  //  if (m_timersvc) m_timers[0]->start();

  m_cutCounter = -1;

  pass=false;

  m_njet = 0.0;
  m_et = -99000.;
  m_eta = -99.;
  m_phi = -99.;

  const xAOD::CaloClusterContainer* outJets = 0;
  HLT::ErrorCode ec = getFeature(outputTE, outJets);
  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get JetCollections " << endreq;
    return ec;
  }
  xAOD::CaloClusterContainer::const_iterator itr =  outJets->begin();
  xAOD::CaloClusterContainer::const_iterator itrE = outJets->end();
  double NBadFEBEMBA = 0.0;
  double NBadFEBEMBC = 0.0;
  double NBadFEBEMECA = 0.0;
  double NBadFEBEMECC = 0.0;
  for( ; itr != itrE ; ++itr ){
	NBadFEBEMECA = (*itr)->rawE();
        NBadFEBEMBA =  (*itr)->e();
	NBadFEBEMECC = (*itr)->time();
        NBadFEBEMBC =  (*itr)->m();
  }
    uint32_t BadFEBPartitions = 0;
  if ( NBadFEBEMBA  > m_MinBadFEB )  BadFEBPartitions |= 1;
  if ( NBadFEBEMBC  > m_MinBadFEB )  BadFEBPartitions |= 2;
  if ( NBadFEBEMECA  > m_MinBadFEB )  BadFEBPartitions |=4;
  if ( NBadFEBEMECC  > m_MinBadFEB )  BadFEBPartitions |=8;
  bool badFEBFlag = (BadFEBPartitions != 0);
  if ( msgLvl() <= MSG::DEBUG ) {
     msg(MSG::DEBUG) << "BadFEBPartitions : 0x" <<
        std::hex << BadFEBPartitions << std::dec << endreq;
     if ( badFEBFlag ) {
        msg(MSG::DEBUG) << "Noisy Event accepted!" << endreq;
     } else {
        msg(MSG::DEBUG) << "Noisy Event rejected!" << endreq;
     }
  }
  if ( badFEBFlag ) 
        pass=true;
  else
        pass=false;


  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------

  //    if (m_timersvc) m_timers[0]->stop();

  return HLT::OK;

}

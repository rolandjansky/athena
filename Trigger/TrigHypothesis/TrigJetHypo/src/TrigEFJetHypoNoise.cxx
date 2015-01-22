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
#include "xAODEventInfo/EventInfo.h"
#include "LArCellRec/LArNoisyROTool.h"
#include "LArRecEvent/LArNoisyROSummary.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/Enums.h"

#include "TrigJetHypo/TrigEFJetHypoNoise.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#ifdef ONLINEIS
#include "hltinterface/ContainerFactory.h"
#endif

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetHypoNoise::TrigEFJetHypoNoise(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator), m_isInterface(false), m_noisyROTool("",this) {

  declareProperty("Etcut",   m_EtCut = 40*CLHEP::GeV); // Default: 40 GeV
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);
  declareProperty("NoiseTool",   m_noisyROTool);

  declareProperty( "BadFEBCut", m_MinBadFEB=5 );

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
  m_isInterface = false;

  if ( m_noisyROTool.retrieve().isFailure() ){
	msg() << MSG::WARNING << "Could not retrieve tool, no noise burst hunting" << endreq;
	return HLT::OK;
  }

#ifdef ONLINEIS
  auto cfact = hltinterface::ContainerFactory::getInstance();
  if ( cfact ) {
      msg() << MSG::DEBUG << "Got the factory for TDAQ interface, will try to register vectors" << endreq;
      try {
          m_IsObject = cfact->constructContainer("LArISInfo","LArNoiseBurstCandidates");
          m_evntPos = cfact->addIntVector(m_IsObject,"Flag",hltinterface::GenericHLTContainer::LASTVALUE);
          m_timeTagPos = cfact->addIntVector(m_IsObject,"TimeStamp",hltinterface::GenericHLTContainer::LASTVALUE);
          m_timeTagPosns = cfact->addIntVector(m_IsObject,"TimeStamp_ns",hltinterface::GenericHLTContainer::LASTVALUE);
          hltinterface::IInfoRegister::instance()->registerObject("/HLTObjects/",m_IsObject);
          m_isInterface = true;
      }
      catch (std::exception& ex ) {
	  msg() << MSG::WARNING << "Cannot really use ISInfo publication. got exception " << ex.what() << endreq;
          m_isInterface = false;
      }
  } // if cfact
#endif
  
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

  pass=false;
  bool msgDebug = msgLvl(MSG::DEBUG);

  if ( !m_noisyROTool ) return HLT::OK;
  // no tool running anyway

  const CaloCellContainer* outCells(0);
  HLT::ErrorCode ec = getFeature(outputTE, outCells);
  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get CellCollections " << endreq;
    return ec;
  }

  
  char flag = 0;
  if ( outCells ) {
	if ( msgDebug ) msg() << MSG::DEBUG << "Got cell container, will process it" << endreq;
	std::unique_ptr<LArNoisyROSummary> noisyRO = m_noisyROTool->process(outCells);
	if ( msgDebug ) msg() << MSG::DEBUG << "processed it" << endreq;
        if ( noisyRO->BadFEBFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : BadFEBFlaggedPartitions" << endreq;
	      flag |= 0x1;
        }
        if ( noisyRO->BadFEB_WFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : BadFEB_WFlaggedPartitions" << endreq;
	      flag |= 0x8;
        }
        if ( noisyRO->SatTightFlaggedPartitions() ) {
	      if ( msgDebug ) msg() << MSG::DEBUG << "Passed : SatTightFlaggedPartitions" << endreq;
	      flag |= 0x2;
        }
  } // end of if outCells

  if ( msgDebug ) msg() << MSG::DEBUG << "got the flag : " << (unsigned int)flag << endreq;
  


  if ( flag != 0x0 ) {
	if ( msgDebug ) msg() << MSG::INFO << "LAr Noise detected : ";
	pass = true;
  }
        else if ( msgDebug ) msg() << MSG::INFO << "LAr Noise not detected!" << endreq;

  if ( pass ) {
	const xAOD::EventInfo* evt;
	if ( (store()->retrieve(evt)).isFailure() ) {
		msg(MSG::DEBUG) << endreq;
		msg(MSG::ERROR) << "Cannot access eventinfo" << endreq;
	}
	else {
		if ( msgDebug ) msg() << MSG::DEBUG << "at event number : "
			<< evt->eventNumber() << "; LB : "
			<< evt->lumiBlock() << "; timeStamp : "
			<< evt->timeStamp() << "; timeStamp ns : "
			<< evt->timeStampNSOffset() << endreq;
#ifdef ONLINEIS
		if ( m_isInterface ) {
		    m_IsObject->appendField(m_evntPos,std::vector<long>{flag});
		    m_IsObject->appendField(m_timeTagPos,std::vector<long>{(long int)evt->timeStamp()});
		    m_IsObject->appendField(m_timeTagPosns,std::vector<long>{(long int)evt->timeStampNSOffset()});
		}
#endif
	}
  }

  return HLT::OK;

}

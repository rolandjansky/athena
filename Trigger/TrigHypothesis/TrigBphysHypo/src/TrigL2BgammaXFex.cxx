/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigL2BgammaXFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk
// copied from Jpsiee, dummy algo for now
// ********************************************************************

#include <math.h> 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigBphysHypo/TrigL2BgammaXFex.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

//#include <map.h>

class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigL2BgammaXFex::TrigL2BgammaXFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator)
/*------------------------------------------------------------------------------------*/
{

  // Read cuts 
  declareProperty( "AcceptAll", m_acceptAll  = false );

}

/*-------------------------------------------*/
TrigL2BgammaXFex::~TrigL2BgammaXFex()
/*-------------------------------------------*/
{

}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2BgammaXFex::hltInitialize()
/*-------------------------------------------*/
{

  
  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigL2BgammaXFex::hltFinalize()
/*-------------------------------------------*/
{

  
  return HLT::OK;
}


/*--------------------------------------------------------------*/
HLT::ErrorCode TrigL2BgammaXFex::acceptInput(const HLT::TriggerElement* inputTE, bool& pass)
/*--------------------------------------------------------------*/
{
  bool result = false;

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            //mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo


  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	  << endreq;

    pass = true;
  } 
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	  << endreq;
    pass=false;
  }

// get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(inputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " In L2BgammaXFEX acceptInput" << endreq;

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using inputTE("<< inputTE <<")->getId(): " << inputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  } 


  result=true;
  
  if (!m_acceptAll) pass = result; 

  return HLT::OK;
}


/*--------------------------------------------------------------*/
HLT::ErrorCode TrigL2BgammaXFex::hltExecute(const HLT::TriggerElement* /* inputTE */, HLT::TriggerElement* outputTE)
/*--------------------------------------------------------------*/
{
   if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " In L2BgammaXFEX hltExecute" << endreq;

// get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  } 


     return HLT::OK;
}


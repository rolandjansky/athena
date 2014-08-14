// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFTrkMassHypo_xAOD.cxx
 **
 **   Description: L2 hypothesis algorithms for tag and probe (tracks with mass cut)
 **                Create TrigL2Bphys
 **
 **   Author: J.Kirk
 **
 **   Created:   14.10.2007
 **   Modified:
 **
 **************************************************************************/

#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigBphysHypo/TrigEFTrkMassHypo_xAOD.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

class ISvcLocator;

TrigEFTrkMassHypo_xAOD::TrigEFTrkMassHypo_xAOD(const std::string & name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator)
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll=true);

  declareMonitoredVariable("CutCounter", mon_cutCounter);
  declareMonitoredVariable("NBphys",     mon_NBphys);


}

TrigEFTrkMassHypo_xAOD::~TrigEFTrkMassHypo_xAOD()
{ }

HLT::ErrorCode TrigEFTrkMassHypo_xAOD::hltInitialize()
{

  if(msgLvl() <= MSG::DEBUG) {

    msg() << MSG::DEBUG << "AcceptAll            = "
        << (m_acceptAll==true ? "True" : "False") << endreq;
  }

  m_lastEvent = -1;
  m_lastEventPassed = -1;
  m_countTotalEvents =0;
  m_countTotalRoI =0;
  m_countPassedEvents =0;
  m_countPassedRoIs =0;

  return HLT::OK;
}

HLT::ErrorCode TrigEFTrkMassHypo_xAOD::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFTrkMassHypo_xAOD -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFTrkMassHypo_xAOD::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  bool result = false;
  mon_cutCounter = -1;

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

    if (IdEvent != m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;


  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
    pass = true;
    //return HLT::OK;
  }
  if(msgLvl() <= MSG::DEBUG) {
    if (m_acceptAll) {
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
          << endreq;
    } else {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
          << endreq;
    }
  }
  // for now pass all events - JK changed to false 9/2/10
  pass=false;
//  create vector for TrigEFBphys particles
    //const TrigEFBphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer * xAODTrigBphysColl(0);
    
  HLT::ErrorCode status = getFeature(outputTE, xAODTrigBphysColl, "EFTrackMass");

  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get xAODTrigBphysColl collection" << endreq;
    }

    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Retrieved Bphys collection  xAODTrigBphysColl = " << xAODTrigBphysColl << endreq;
  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( xAODTrigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;

    mon_NBphys=0;
    return HLT::OK;
  }

  mon_NBphys=xAODTrigBphysColl->size();
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got TrigBphys collection with " << xAODTrigBphysColl->size()
        << " TrigBphys particles " << endreq;
  }

  mon_cutCounter = 0;

  TrigPassBits *bits = HLT::makeTrigPassBits(xAODTrigBphysColl);

  // now loop over Bphys particles to set passBits
  xAOD::TrigBphysContainer::const_iterator bphysIter = xAODTrigBphysColl->begin();
  for ( ; bphysIter != xAODTrigBphysColl->end(); ++bphysIter) {
    HLT::markPassing(bits, *bphysIter, xAODTrigBphysColl);
  }

  result=true;

  if (result) {
    m_countPassedRoIs++;
    if (IdEvent!= m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
    pass=true;
  }

  // store result
  if ( attachBits(outputTE, bits) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endreq;
  }

  return HLT::OK;
}

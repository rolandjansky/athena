// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFTrkMassHypo.cxx
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

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigEFTrkMassHypo.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "TrigSteeringEvent/TrigPassBits.h"
#include "xAODTrigger/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

class ISvcLocator;

TrigEFTrkMassHypo::TrigEFTrkMassHypo(const std::string & name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator)
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll=true);

  declareMonitoredVariable("CutCounter", m_mon_cutCounter);
  declareMonitoredVariable("NBphys",     m_mon_NBphys);


}

TrigEFTrkMassHypo::~TrigEFTrkMassHypo()
{ }

HLT::ErrorCode TrigEFTrkMassHypo::hltInitialize()
{

  ATH_MSG_DEBUG("AcceptAll            = "<< (m_acceptAll==true ? "True" : "False"));

  m_lastEvent = -1;
  m_lastEventPassed = -1;
  m_countTotalEvents =0;
  m_countTotalRoI =0;
  m_countPassedEvents =0;
  m_countPassedRoIs =0;

  return HLT::OK;
}

HLT::ErrorCode TrigEFTrkMassHypo::hltFinalize()
{
  ATH_MSG_INFO("in finalize()" );
  ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigEFTrkMassHypo -------------|" );
  ATH_MSG_INFO("Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI );
  ATH_MSG_INFO("Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs );

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFTrkMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  bool result = false;
  m_mon_cutCounter = -1;

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        ATH_MSG_DEBUG("Failed to get xAOD::EventInfo " );
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            ATH_MSG_DEBUG("Failed to get EventInfo " );
            //m_mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            ATH_MSG_DEBUG(" Run " << IdRun << " Event " << IdEvent );
        }// found old event info
    }else { // found the xAOD event info
        ATH_MSG_DEBUG(" Run " << evtInfo->runNumber() << " Event " << evtInfo->eventNumber() );
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
  ATH_MSG_DEBUG("AcceptAll is set to : " << (m_acceptAll ? "True, taking all events " : "False, applying selection" ));

  // for now pass all events - JK changed to false 9/2/10
  pass=false;
//  create vector for TrigEFBphys particles
    //const TrigEFBphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer * xAODTrigBphysColl(0);
    
  HLT::ErrorCode status = getFeature(outputTE, xAODTrigBphysColl, "EFTrackMass");

  if ( status != HLT::OK ) {
    ATH_MSG_WARNING("Failed to get xAODTrigBphysColl collection" );
    return HLT::OK;
  }

  ATH_MSG_DEBUG(" Retrieved Bphys collection  xAODTrigBphysColl = " << xAODTrigBphysColl );
  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( xAODTrigBphysColl == 0 ) {
    ATH_MSG_DEBUG("No Bphys particles to analyse, leaving!" );
    m_mon_NBphys=0;
    return HLT::OK;
  }

  m_mon_NBphys=xAODTrigBphysColl->size();
  ATH_MSG_DEBUG("Got TrigBphys collection with " << xAODTrigBphysColl->size() << " TrigBphys particles " );

  m_mon_cutCounter = 0;

  //TrigPassBits *bits = HLT::makeTrigPassBits(xAODTrigBphysColl);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::TrigBphysContainer>(xAODTrigBphysColl);

  // now loop over Bphys particles to set passBits
  xAOD::TrigBphysContainer::const_iterator bphysIter = xAODTrigBphysColl->begin();
  for ( ; bphysIter != xAODTrigBphysColl->end(); ++bphysIter) {
    //HLT::markPassing(bits, *bphysIter, xAODTrigBphysColl);
    xBits->markPassing((*bphysIter),xAODTrigBphysColl,true);
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
  //if ( attachBits(outputTE, bits) != HLT::OK ) {
  //  msg() << MSG::ERROR << "Problem attaching TrigPassBits! " );
  //}
  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");

  return HLT::OK;
}

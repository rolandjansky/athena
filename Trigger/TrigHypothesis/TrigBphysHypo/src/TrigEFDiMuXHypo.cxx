// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFDiMuXHypo.cxx
 **
 **   Description: EF hypothesis algorithms for B+ -> K+ mu+ mu-
 **
 **   Author: Cristina Adorisio (Cristina.Adorisio@cern.ch)
 **
 **   Created:   10.03.2009
 **
 **   Modified:
 **
 **
 **************************************************************************/
#include "TrigBphysHypo/TrigEFDiMuXHypo.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

/*------------------------------------------------------------------------------------*/
TrigEFDiMuXHypo::TrigEFDiMuXHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
/*------------------------------------------------------------------------------------*/
{

  // Read cuts
  declareProperty("AcceptAll", m_acceptAll  = false );
  declareProperty("LowerBplusMassCut", m_lowerBplusMassCut = 5100.0); //default=5100.0
  declareProperty("UpperBplusMassCut", m_upperBplusMassCut = 5500.0); //default=5500.0

  declareMonitoredStdContainer("Bmass",    mon_bmass         , AutoClear);

  m_lastEvent       = -1;
  m_lastEventPassed = -1;

  m_countTotalEvents     = 0;
  m_countTotalRoI        = 0;

  m_countPassedEvents    = 0;
  m_countPassedRoIs      = 0;
  m_countPassedBplusMass = 0;
}

/*-------------------------------------------*/
TrigEFDiMuXHypo::~TrigEFDiMuXHypo()
    /*-------------------------------------------*/
{
}

/*-------------------------------------------*/
HLT::ErrorCode TrigEFDiMuXHypo::hltInitialize()
    /*-------------------------------------------*/
{

  msg() << MSG::INFO << "|----------------------- INFO FROM TrigEFDiMuXHypo -------------------|" << endreq;
  msg() << MSG::INFO << "Inizializing TrigEFDiMuXHypo" << endreq;
  msg() << MSG::INFO << "AcceptAll       = " << (m_acceptAll==true ? "True" : "False") << endreq;
  msg() << MSG::INFO << "B+ mass window:   " << m_lowerBplusMassCut << " < Mass(KplusMuMu) < " << m_upperBplusMassCut << endreq;
  msg() << MSG::INFO << "|---------------------------------------------------------------------|" << endreq;

  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigEFDiMuXHypo::hltFinalize()
    /*-------------------------------------------*/
{

  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFDiMuXHypo ----------------|" << endreq;
  msg() << MSG::INFO << "Finalizing TrigEFDiMuXHypo" << endreq;
  msg() << MSG::INFO << "Run on events/RoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/RoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed BplusMass: " << m_countPassedBplusMass << endreq;
  msg() << MSG::INFO << "|---------------------------------------------------------------------|" << endreq;
  return HLT::OK;
}

/*-----------------------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFDiMuXHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
    /*------------------------------------------------------------------------------------------*/
{
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "In TrigEFDiMuXHypo::hltExecute()" << endreq;
  }


  bool PassedBplusMass = false;
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
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endreq;
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
    return HLT::OK;
  } else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
          << endreq;
  }

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

  // create vector for TrigEFBphys particles
  const TrigEFBphysContainer* trigBphysColl = 0;

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "EFDiMuXFex");

  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigBphysics collection" << endreq;
    }
    return HLT::OK;
  }
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "Retrieved Bphys collection  trigBphysColl = " << trigBphysColl << endreq;
  }
  if ( trigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;
    return HLT::OK;
  }

  if (IdEvent != (int) m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got TrigBphys collection with " << trigBphysColl->size() << " TrigBphys particles " << endreq;
  }

  // if no Bphys particles were found, just leave TrigBphysColl empty and leave
  if ( trigBphysColl->size() == 0 ) {
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;
    }
    return HLT::OK;
  }

  TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);

  // now loop over Bphys particles to see if one passes cuts
  for ( TrigEFBphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << "Bphys particle type: " << (*bphysIter)->particleType() << " with mass " << (*bphysIter)->mass() << endreq;
    }
    if ( (*bphysIter)->particleType() == TrigEFBphys::BMUMUX ) {   //TrigEFBphys::BMUMUX = 3

      float BplusMass = (*bphysIter)->mass();
      if ( msgLvl() <= MSG::DEBUG ){
        msg() << MSG::DEBUG << "BplusMass = " << BplusMass << endreq;
      }
      bool thisPassedBplusMass = false;
      if ( BplusMass > m_lowerBplusMassCut && BplusMass < m_upperBplusMassCut ) {
        if ( msgLvl() <= MSG::DEBUG ){
          msg() << MSG::DEBUG << "Mass =  " << BplusMass  << " -> Bplus Mass passed " << endreq;
        }
        PassedBplusMass=true;
        thisPassedBplusMass = true;
        mon_bmass.push_back((BplusMass*0.001));
      }

      if ( thisPassedBplusMass ) {
        result=true;
        HLT::markPassing(bits, *bphysIter, trigBphysColl);
      }
    }
  }

  if (PassedBplusMass) m_countPassedBplusMass++;
  if (result) {
    m_countPassedRoIs++;
    if (IdEvent!= (int) m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
  }

  // Reach this point successfully
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;
  }
  if (!m_acceptAll) pass = result;

  // store result
  if ( attachBits(outputTE, bits) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endreq;
  }

  return HLT::OK;
}


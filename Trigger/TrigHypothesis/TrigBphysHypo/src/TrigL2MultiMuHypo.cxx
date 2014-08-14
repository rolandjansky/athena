// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigL2MultiMuHypo.cxx
 **
 **   Description: L2 hypothesis algorithms for 3 or 4 muon events
 **                Create TrigL2Bphys
 **
 **   Author: J.Kirk
 **
 **
 **************************************************************************/

#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigBphysHypo/TrigL2MultiMuHypo.h"
#include "TrigParticle/TrigL2BphysContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"


// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

class ISvcLocator;

TrigL2MultiMuHypo::TrigL2MultiMuHypo(const std::string & name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator)
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("LowerMassCut", m_lowerMassCut=4000.0);
  declareProperty("UpperMassCut", m_upperMassCut=6000.0);
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
  declareProperty("ApplyChi2Cut", m_ApplyChi2Cut=true);
  declareProperty("Chi2VtxCut", m_Chi2VtxCut=20.0);

  declareMonitoredVariable(    "CutCounter", mon_cutCounter);
  declareMonitoredStdContainer("MuMumass",   mon_MuMumass   , AutoClear);


}

TrigL2MultiMuHypo::~TrigL2MultiMuHypo()
{ }

HLT::ErrorCode TrigL2MultiMuHypo::hltInitialize()
{

  if(msgLvl() <= MSG::DEBUG) {

    msg() << MSG::DEBUG << "AcceptAll            = "
        << (m_acceptAll==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "OppositeCharge       = "
        << (m_oppositeCharge==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
    msg() << MSG::DEBUG << "ApplyUpperMassCut         = " << m_ApplyupperMassCut << endreq;
  }

  m_lastEvent = -1;
  m_lastEventPassed = -1;
  m_countTotalEvents =0;
  m_countTotalRoI =0;
  m_countPassedEvents =0;
  m_countPassedRoIs =0;
  m_countPassedBsMass =0;
  m_countPassedChi2Cut =0;

  return HLT::OK;
}

HLT::ErrorCode TrigL2MultiMuHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigL2MultiMuHypo -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed BsMass: "  << m_countPassedBsMass << endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigL2MultiMuHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  //const double BSMASS = 5369.3;
  //const double BDMASS = 5279.2;

  //bool PassedMuMuPair=false;
  bool PassedBsMass=false;
  bool PassedChi2Cut=false;
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
//  create vector for TrigL2Bphys particles
  const TrigL2BphysContainer* trigBphysColl = 0;

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "L2MultiMuFex");

  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigBphysics collection" << endreq;
    }

    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Retrieved Bphys collection  trigBphysColl = " << trigBphysColl << endreq;
  if ( trigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;

    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got TrigBphys collection with " << trigBphysColl->size()
        << " TrigBphys particles " << endreq;
  }

  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( trigBphysColl->size() == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;

    return HLT::OK;
  }


  mon_cutCounter = 0;

  TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);

  // now loop over Bphys particles to see if one passes cuts
  for (TrigL2BphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {

    if ((*bphysIter)->particleType() == TrigL2Bphys::MULTIMU ) {
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Got Bphys partcile with mass " <<  (*bphysIter)->mass() << " and chi2 " <<
            (*bphysIter)->fitchi2() << endreq;
      float BsMass = (*bphysIter)->mass();
      bool thisPassedBsMass = (m_lowerMassCut < BsMass && (BsMass < m_upperMassCut || (!m_ApplyupperMassCut)));
      PassedBsMass |= thisPassedBsMass;

      bool thisPassedChi2Cut = ((!m_ApplyChi2Cut) || ((*bphysIter)->fitchi2() < m_Chi2VtxCut && (*bphysIter)->fitchi2() != -99) );
      PassedChi2Cut |= thisPassedBsMass;
      if (thisPassedBsMass)    mon_MuMumass.push_back((BsMass*0.001));

      if( thisPassedBsMass && thisPassedChi2Cut )
      {
        HLT::markPassing(bits, *bphysIter, trigBphysColl);
      }
    }
  }

  if (PassedBsMass)  {
    m_countPassedBsMass++;
    mon_cutCounter++;
  }
  if (PassedChi2Cut) { m_countPassedChi2Cut++; mon_cutCounter++; }
  if ( PassedBsMass && PassedChi2Cut ) {
    result = true;
  }

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

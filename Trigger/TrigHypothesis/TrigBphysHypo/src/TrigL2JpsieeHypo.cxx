/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2JpsieeHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk
// ********************************************************************

#include <math.h>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigBphysHypo/TrigL2JpsieeHypo.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"


class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigL2JpsieeHypo::TrigL2JpsieeHypo(const std::string & name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator)
    /*------------------------------------------------------------------------------------*/
{

  // Read cuts
  declareProperty( "AcceptAll", m_acceptAll  = false );
  declareProperty("JpsiMasslow_cut", m_JpsiMasslow_cut=1500.);
  declareProperty("JpsiMasshigh_cut", m_JpsiMasshigh_cut=3800.);

  declareMonitoredVariable(    "CutCounter", mon_cutCounter);
  declareMonitoredStdContainer("Mass",       mon_Mass,         AutoClear);

  // zero counters
  m_lastEvent = 999;
  m_lastEventPassed = 999;
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedJpsiMass=0;

}

/*-------------------------------------------*/
TrigL2JpsieeHypo::~TrigL2JpsieeHypo()
    /*-------------------------------------------*/
{
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2JpsieeHypo::hltInitialize()
    /*-------------------------------------------*/
{

  // print out properties, cuts


  msg() << MSG::INFO
      << "Phi mass cuts: " << m_JpsiMasslow_cut << " < Mass(phi) < " << m_JpsiMasshigh_cut
      << endreq;
  msg() << MSG::INFO
      << "Initialization completed successfully"
      << endreq;


  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigL2JpsieeHypo::hltFinalize()
    /*-------------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Run on events/RoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/RoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed JpsiMass: "  << m_countPassedJpsiMass << endreq;

  return HLT::OK;
}

/*--------------------------------------------------------------*/
HLT::ErrorCode TrigL2JpsieeHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
    /*--------------------------------------------------------------*/
{
  bool PassedJpsiMass=false;
  bool result = false;

  mon_cutCounter = -1; // at the begining of execution

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
  if (IdEvent != (int) m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;


  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
          << endreq;

    pass = true;
    return HLT::OK;
  }
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
          << endreq;
  }

  // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor, "initialRoI") != HLT::OK) roiDescriptor = 0;

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



//  create vector for TrigL2Bphys particles
  const TrigL2BphysContainer* trigBphysColl = 0;



  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "L2JpsieeFex");

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


  mon_cutCounter = 0; // this is to separate bad input TE and true behaviour of the first cut

  TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);

  // now loop over Bphys particles to see if one passes cuts
  for (TrigL2BphysContainer::const_iterator bphysIter = trigBphysColl->begin();
       bphysIter !=  trigBphysColl->end(); ++bphysIter)
  {

    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bphys particle type: " << (*bphysIter)->particleType() << " with mass "
          << (*bphysIter)->mass() << endreq;
    if ((*bphysIter)->particleType() == TrigL2Bphys::JPSIEE ) {
      float JpsiMass = (*bphysIter)->mass();
      bool thisPassedJpsiMass=false;
      if (JpsiMass > m_JpsiMasslow_cut && JpsiMass < m_JpsiMasshigh_cut) {
        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " mass =  " << JpsiMass  << " EVENT PASSES!! " << endreq;
        PassedJpsiMass=true;
        thisPassedJpsiMass=true;
        mon_Mass.push_back((JpsiMass*0.001));
      }

      if (thisPassedJpsiMass) {
        result=true;

        HLT::markPassing(bits, *bphysIter, trigBphysColl); // store decision per object

      }

    }
  }

  if (PassedJpsiMass) {
    mon_cutCounter++;
  }



  if (PassedJpsiMass) m_countPassedJpsiMass++;

  if (result) {
    m_countPassedRoIs++;
    if (IdEvent!= (int) m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
  }
//   Reach this point successfully
  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;


  if (!m_acceptAll) pass = result;

    // store result
  if ( attachBits(outputTE, bits) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endreq;
  }

  return HLT::OK;
}


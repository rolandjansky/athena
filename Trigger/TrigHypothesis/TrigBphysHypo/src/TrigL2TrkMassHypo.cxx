// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigL2TrkMassHypo.cxx
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

#include "TrigL2TrkMassHypo.h"
//#include "TrigParticle/TrigL2BphysContainer.h"
//#include "TrigParticle/TrigL2Bphys.h"
//
//#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/DataHandle.h"
//
#include <math.h>
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "TrigBphysHelperUtilsTool.h"
#include "xAODTrigBphys/TrigBphysContainer.h"


class ISvcLocator;

TrigL2TrkMassHypo::TrigL2TrkMassHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
m_bphysHelperTool("TrigBphysHelperUtilsTool")
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll=true);

  declareMonitoredVariable(    "CutCounter",  mon_cutCounter);
  declareMonitoredVariable(    "NBphys",      mon_NBphys);
  declareMonitoredStdContainer("Mass",        mon_Mass,        AutoClear);
  //declareMonitoredStdContainer("FitMass",     mon_FitMass,     AutoClear);
  //declareMonitoredStdContainer("Chi2",        mon_Chi2,        AutoClear);
  //declareMonitoredStdContainer("Chi2Prob",    mon_Chi2Prob,    AutoClear);

}

TrigL2TrkMassHypo::~TrigL2TrkMassHypo()
{ }

HLT::ErrorCode TrigL2TrkMassHypo::hltInitialize()
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
    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endreq;
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endreq;
    }

  return HLT::OK;
}

HLT::ErrorCode TrigL2TrkMassHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigL2TrkMassHypo -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigL2TrkMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  bool result = false;
  mon_cutCounter = -1;

    // Retrieve event info
    //    int IdRun   = 0;
    int IdEvent = 0;
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        msg() << MSG::ERROR << "Error retriving EventInfo" << endreq;
    }
    //    IdRun = runNumber;
    IdEvent = evtNumber;

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
    const xAOD::TrigBphysContainer* trigBphysColl = 0;

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "L2TrackMass");

  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigBphysics collection" << endreq;
    }

    return HLT::OK;
  }

   if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Retrieved Bphys collection  trigBphysColl = " << trigBphysColl << endreq;
  // if no Bphys particles were found, just leave TrigBphysColl and leave
  if ( trigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;

    mon_NBphys=0;
    return HLT::OK;
  }
  mon_cutCounter = 0;  // this is to separate bad input TE and true behaviour of the first cut

  mon_NBphys=trigBphysColl->size();
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got TrigBphys collection with " << trigBphysColl->size() << " TrigBphys particles " << endreq;
  }

  TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);

  // now loop over Bphys particles to see if one passes hypo cuts- not ready, just monitoring
  xAOD::TrigBphysContainer::const_iterator bphysIter = trigBphysColl->begin();
  for ( ; bphysIter != trigBphysColl->end(); ++bphysIter) {
    mon_Mass.push_back(((*bphysIter)->mass()) / CLHEP::GeV);   // conversion to GeV
    HLT::markPassing(bits, *bphysIter, trigBphysColl);
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

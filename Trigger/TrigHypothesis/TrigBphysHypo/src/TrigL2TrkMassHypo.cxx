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

#include "TrigL2TrkMassHypo.h"
#include <math.h>
#include "xAODTrigger/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "TrigBphysHelperUtilsTool.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigL2TrkMassHypo::TrigL2TrkMassHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll=true);

  declareMonitoredVariable(    "CutCounter",  m_mon_cutCounter);
  declareMonitoredVariable(    "NBphys",      m_mon_NBphys);
  declareMonitoredStdContainer("Mass",        m_mon_Mass,        AutoClear);
  //declareMonitoredStdContainer("FitMass",     m_mon_FitMass,     AutoClear);
  //declareMonitoredStdContainer("Chi2",        m_mon_Chi2,        AutoClear);
  //declareMonitoredStdContainer("Chi2Prob",    m_mon_Chi2Prob,    AutoClear);

}

TrigL2TrkMassHypo::~TrigL2TrkMassHypo()
{ }

HLT::ErrorCode TrigL2TrkMassHypo::hltInitialize()
{
  ATH_MSG_DEBUG("AcceptAll            = " << (m_acceptAll==true ? "True" : "False") );

  m_lastEvent = -1;
  m_lastEventPassed = -1;
  m_countTotalEvents =0;
  m_countTotalRoI =0;
  m_countPassedEvents =0;
  m_countPassedRoIs =0;
    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't find TrigBphysHelperUtilsTool" );
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("TrigBphysHelperUtilsTool found" );
    }

  return HLT::OK;
}

HLT::ErrorCode TrigL2TrkMassHypo::hltFinalize()
{
  ATH_MSG_INFO("in finalize()" );
  ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigL2TrkMassHypo -------------|" );
  ATH_MSG_INFO("Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI );
  ATH_MSG_INFO("Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs );

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigL2TrkMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  bool result = false;
  m_mon_cutCounter = -1;

    // Retrieve event info
    //    int IdRun   = 0;
    int IdEvent = 0;
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        ATH_MSG_ERROR("Error retriving EventInfo" );
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
  ATH_MSG_DEBUG("AcceptAll is set to : " << (m_acceptAll ? "True, taking all events " : "False, applying selection" ));

//  create vector for TrigL2Bphys particles
    const xAOD::TrigBphysContainer* trigBphysColl = 0;

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "L2TrackMass");

  if ( status != HLT::OK ) {
    ATH_MSG_WARNING("Failed to get TrigBphysics collection" );
    return HLT::OK;
  }

  ATH_MSG_DEBUG(" Retrieved Bphys collection  trigBphysColl = " << trigBphysColl );
  // if no Bphys particles were found, just leave TrigBphysColl and leave
  if ( trigBphysColl == 0 ) {
    ATH_MSG_DEBUG("No Bphys particles to analyse, leaving!" );

    m_mon_NBphys=0;
    return HLT::OK;
  }
  m_mon_cutCounter = 0;  // this is to separate bad input TE and true behaviour of the first cut

  m_mon_NBphys=trigBphysColl->size();

  ATH_MSG_DEBUG("Got TrigBphys collection with " << trigBphysColl->size() << " TrigBphys particles " );

  //TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::TrigBphysContainer>(trigBphysColl);

  // now loop over Bphys particles to see if one passes hypo cuts- not ready, just monitoring
  xAOD::TrigBphysContainer::const_iterator bphysIter = trigBphysColl->begin();
  for ( ; bphysIter != trigBphysColl->end(); ++bphysIter) {
    m_mon_Mass.push_back(((*bphysIter)->mass()) / CLHEP::GeV);   // conversion to GeV
    //HLT::markPassing(bits, *bphysIter, trigBphysColl);
    xBits->markPassing((*bphysIter),trigBphysColl,true);
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
  //  ATH_MSG_ERROR("Problem attaching TrigPassBits! " );
  //}
  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");

  return HLT::OK;
}

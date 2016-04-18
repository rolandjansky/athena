// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFMultiMuHypo.cxx
 **
 **   Description: EF hypothesis algorithms for B0_s -> mu+ mu-
 **                Dummy algo for now
 **
 **   Author: J.Kirk
 **
 **************************************************************************/

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigEFMultiMuHypo.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
//#include "xAODTrigBphys/TrigBphysAuxContainer.h"

class ISvcLocator;

TrigEFMultiMuHypo::TrigEFMultiMuHypo(const std::string & name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator)
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
,m_countPassedBsMass(0)
,m_countPassedChi2Cut(0)
{

  // Read cuts

  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("LowerMassCut", m_lowerMassCut=4000.0);
  declareProperty("UpperMassCut", m_upperMassCut=6000.0);
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
  declareProperty("ApplyChi2Cut", m_ApplyChi2Cut=true);
  declareProperty("Chi2VtxCut", m_Chi2VtxCut=20.0);

  declareMonitoredVariable("CutCounter",   mon_cutCounter);
  declareMonitoredVariable("MuMumass",     mon_MuMumass  );
  declareMonitoredVariable("FitChi2",      mon_FitChi2  );


}

TrigEFMultiMuHypo::~TrigEFMultiMuHypo()
{ }

HLT::ErrorCode TrigEFMultiMuHypo::hltInitialize()
{

  if(msgLvl() <= MSG::DEBUG) {

    msg() << MSG::DEBUG << "AcceptAll            = "
        << (m_acceptAll==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "OppositeCharge       = "
        << (m_oppositeCharge==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
    msg() << MSG::DEBUG << "ApplyUpperMassCut    = " << m_ApplyupperMassCut << endreq;
    msg() << MSG::DEBUG << "ApplyChi2Cut         = " << m_ApplyChi2Cut << endreq;
    msg() << MSG::DEBUG << "Chi2VtxCut           = " << m_Chi2VtxCut << endreq;


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

HLT::ErrorCode TrigEFMultiMuHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFMultiMuHypo -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed BsMass: "  << m_countPassedBsMass << endreq;
  msg() << MSG::INFO << "RoIs Passed Chi2 cut: "  << m_countPassedChi2Cut << endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFMultiMuHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  bool PassedBsMass=false;
  bool PassedChi2Cut=false;
  bool result = false;
  mon_cutCounter = -1;
  mon_FitChi2 = -1;
  mon_MuMumass = -1;
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

  //  create vector for TrigEFBphys particles
    //const TrigEFBphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer* trigBphysColl = 0;

//  const TrigEFBContainer* trigBphysColl = 0;
//  const VxContainer* VertexColl;

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "EFMultiMuFex");

  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigBphysics collection" << endreq;
    }

    return HLT::OK;
  }

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Retrieved Bphys collection  trigBphysColl = " << trigBphysColl << endreq;
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
    //    for (TrigEFBphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {
    for (xAOD::TrigBphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {
        
        if ((*bphysIter)->particleType() == xAOD::TrigBphys::MULTIMU ) {
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Got Bphys particle with mass " << (*bphysIter)->mass() << " chi2 :  " << (*bphysIter)->fitchi2() << endreq;
            
            float BsMass = (*bphysIter)->mass();
            bool thisPassedBsMass = (m_lowerMassCut < BsMass && ((BsMass < m_upperMassCut) || (!m_ApplyupperMassCut) ));
            PassedBsMass |= thisPassedBsMass;
            bool thisPassedChi2Cut = ((!m_ApplyChi2Cut) || ((*bphysIter)->fitchi2() < m_Chi2VtxCut && (*bphysIter)->fitchi2() != -99) );
            PassedChi2Cut |= thisPassedChi2Cut;
            if(thisPassedBsMass)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Passed mass cut " << BsMass <<" GeV" << endreq;
            mon_MuMumass = ((BsMass*0.001));
            if(thisPassedChi2Cut)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Apply chi2 cut : " << m_ApplyChi2Cut << " chi2 :  " << (*bphysIter)->fitchi2() << " Passed Chi2 cut < "<< m_Chi2VtxCut << endreq;
	    mon_FitChi2 = (*bphysIter)->fitchi2();
            if(!thisPassedBsMass && !thisPassedChi2Cut)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Did not pass mass & chi2 cuts < "<< endreq;
            
            if( thisPassedBsMass && thisPassedChi2Cut )
            {
                HLT::markPassing(bits, *bphysIter, trigBphysColl);
            }
            
        }


    // JK check tracks, for debugging only
    /*
    const ElementLinkVector<Rec::TrackParticleContainer> trackVector = (*bphysIter)->trackVector();
    if (trackVector.size() != 0) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " got track vector size: " << trackVector.size() << endreq;
  } else {
    if(msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " no track vector!!! "  << endreq;
  }
    ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
    const Trk::MeasuredPerigee* trackPerigee=(*(*trkIt))->measuredPerigee();
    if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
    double phi = trackPerigee->parameters()[Trk::phi];
    double theta = trackPerigee->parameters()[Trk::theta];
    double px = trackPerigee->momentum()[Trk::px];
    double py = trackPerigee->momentum()[Trk::py];
    double pt = sqrt(px*px + py*py);
    double eta = -std::log(tan(theta/2));

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track " << itrk << " pt phi eta " << pt << " " <<
    phi << " " << eta << endreq;
  }
    */
    // end JK debug printout
  }




  if (PassedBsMass)  { m_countPassedBsMass++; mon_cutCounter++; }
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

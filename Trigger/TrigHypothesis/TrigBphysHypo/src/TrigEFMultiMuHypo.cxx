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
//#include "TrigSteeringEvent/TrigPassBits.h"
#include "xAODTrigger/TrigPassBits.h"
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
  declareProperty("bphysCollectionKey", m_bphysCollectionKey  = "EFMultiMuFex" );
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("LowerMassCut", m_lowerMassCut=4000.0);
  declareProperty("UpperMassCut", m_upperMassCut=6000.0);
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
  declareProperty("ApplyChi2Cut", m_ApplyChi2Cut=true);
  declareProperty("Chi2VtxCut", m_Chi2VtxCut=20.0);

  declareMonitoredVariable("CutCounter",   m_mon_cutCounter);
  declareMonitoredVariable("MuMumass",     m_mon_MuMumass  );
  declareMonitoredVariable("FitChi2",      m_mon_FitChi2  );


}

TrigEFMultiMuHypo::~TrigEFMultiMuHypo()
{ }

HLT::ErrorCode TrigEFMultiMuHypo::hltInitialize()
{

  ATH_MSG_DEBUG("AcceptAll            = " << (m_acceptAll==true ? "True" : "False") );
  ATH_MSG_DEBUG("TrigBphys collection "   << m_bphysCollectionKey );
  ATH_MSG_DEBUG("OppositeCharge       = " << (m_oppositeCharge==true ? "True" : "False") );
  ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut );
  ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut );
  ATH_MSG_DEBUG("ApplyUpperMassCut    = " << m_ApplyupperMassCut );
  ATH_MSG_DEBUG("ApplyChi2Cut         = " << m_ApplyChi2Cut );
  ATH_MSG_DEBUG("Chi2VtxCut           = " << m_Chi2VtxCut );

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
  ATH_MSG_INFO("in finalize()" );
  ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigEFMultiMuHypo -------------|" );
  ATH_MSG_INFO("Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI );
  ATH_MSG_INFO("Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs );
  ATH_MSG_INFO("RoIs Passed BsMass: "  << m_countPassedBsMass );
  ATH_MSG_INFO("RoIs Passed Chi2 cut: "  << m_countPassedChi2Cut );

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFMultiMuHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  bool PassedBsMass=false;
  bool PassedChi2Cut=false;
  bool result = false;
  m_mon_cutCounter = -1;
  m_mon_FitChi2 = -1;
  m_mon_MuMumass = -1;
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

  //  create vector for TrigEFBphys particles
    //const TrigEFBphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer* trigBphysColl = 0;

//  const TrigEFBContainer* trigBphysColl = 0;
//  const VxContainer* VertexColl;

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, m_bphysCollectionKey );

  if ( status != HLT::OK ) {
    ATH_MSG_WARNING("Failed to get TrigBphysics collection" );
    return HLT::OK;
  }

  ATH_MSG_DEBUG(" Retrieved Bphys collection  trigBphysColl = " << trigBphysColl );
  if ( trigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
    ATH_MSG_DEBUG("No Bphys particles to analyse, leaving!" );

    return HLT::OK;
  }

  ATH_MSG_DEBUG("Got TrigBphys collection with " << trigBphysColl->size() << " TrigBphys particles " );

  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( trigBphysColl->size() == 0 ) {
    ATH_MSG_DEBUG("No Bphys particles to analyse, leaving!" );

    return HLT::OK;
  }

  m_mon_cutCounter = 0;
  //TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::TrigBphysContainer>(trigBphysColl);

  // now loop over Bphys particles to see if one passes cuts
    //    for (TrigEFBphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {
    for (xAOD::TrigBphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {
        
        if ((*bphysIter)->particleType() == xAOD::TrigBphys::MULTIMU ) {
            ATH_MSG_DEBUG("Got Bphys particle with mass " << (*bphysIter)->mass() << " chi2 :  " << (*bphysIter)->fitchi2() );
            
            float BsMass = (*bphysIter)->mass();
            bool thisPassedBsMass = (m_lowerMassCut < BsMass && ((BsMass < m_upperMassCut) || (!m_ApplyupperMassCut) ));
            // PassedBsMass |= thisPassedBsMass;
            bool thisPassedChi2Cut = ((!m_ApplyChi2Cut) || ((*bphysIter)->fitchi2() < m_Chi2VtxCut && (*bphysIter)->fitchi2() >= -1e-10) );
            // PassedChi2Cut |= thisPassedChi2Cut;
            if(thisPassedBsMass){ ATH_MSG_DEBUG("Passed mass cut " << BsMass <<" GeV" );}
            m_mon_MuMumass = ((BsMass*0.001));
            if(thisPassedChi2Cut){
                ATH_MSG_DEBUG("Apply chi2 cut : " << m_ApplyChi2Cut << " chi2 :  " << (*bphysIter)->fitchi2() << " Passed Chi2 cut < "<< m_Chi2VtxCut );
            }
            m_mon_FitChi2 = (*bphysIter)->fitchi2();
            if(!thisPassedBsMass && !thisPassedChi2Cut){
                ATH_MSG_DEBUG("Did not pass mass & chi2 cuts < ");
            }
	    if( thisPassedBsMass ) PassedBsMass = true;
            if( thisPassedBsMass && thisPassedChi2Cut )
            {
              PassedChi2Cut = true;
                //HLT::markPassing(bits, *bphysIter, trigBphysColl);
              xBits->markPassing((*bphysIter),trigBphysColl,true);
            }
            
        }


    // JK check tracks, for debugging only
    /*
    const ElementLinkVector<Rec::TrackParticleContainer> trackVector = (*bphysIter)->trackVector();
    if (trackVector.size() != 0) {
    ATH_MSG_DEBUG(" got track vector size: " << trackVector.size() );
  } else {
    if(msgLvl() <= MSG::DEBUG)ATH_MSG_DEBUG(" no track vector!!! "  );
  }
    ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
    const Trk::MeasuredPerigee* trackPerigee=(*(*trkIt))->measuredPerigee();
    if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) );
    double phi = trackPerigee->parameters()[Trk::phi];
    double theta = trackPerigee->parameters()[Trk::theta];
    double px = trackPerigee->momentum()[Trk::px];
    double py = trackPerigee->momentum()[Trk::py];
    double pt = sqrt(px*px + py*py);
    double eta = -std::log(tan(theta/2));

    ATH_MSG_DEBUG("track " << itrk << " pt phi eta " << pt << " " <<
    phi << " " << eta );
  }
    */
    // end JK debug printout
  }




  if (PassedBsMass)  { m_countPassedBsMass++; m_mon_cutCounter++; }
  if (PassedChi2Cut) { m_countPassedChi2Cut++; m_mon_cutCounter++; }
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
    ATH_MSG_DEBUG(" accepting event" );
  }

  // store result
  //if ( attachBits(outputTE, bits) != HLT::OK ) {
  //  ATH_MSG_ERROR("Problem attaching TrigPassBits! " );
  //}
  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");

  return HLT::OK;
}

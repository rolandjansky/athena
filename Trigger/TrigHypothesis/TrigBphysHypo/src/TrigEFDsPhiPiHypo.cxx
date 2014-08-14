/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFDsPhiPiHypo.cxx
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

#include "TrigBphysHypo/TrigEFDsPhiPiHypo.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"


class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigEFDsPhiPiHypo::TrigEFDsPhiPiHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
/*------------------------------------------------------------------------------------*/
{

  // Read cuts 
  declareProperty("AcceptAll", m_acceptAll  = false );
  declareProperty("EtaBins", m_Etabin);
  declareProperty("PhiMasslow_cut", m_PhiMasslow_cut);
  declareProperty("PhiMasshigh_cut", m_PhiMasshigh_cut);
  declareProperty("DsMasslow_cut", m_DsMasslow_cut);
  declareProperty("DsMasshigh_cut", m_DsMasshigh_cut);
  declareProperty("useVertexFit", m_useVertexFit=true);

  declareMonitoredVariable("CutCounter", mon_cutCounter    );

  // zero counters
  m_lastEvent = 999;
  m_lastEventPassed = 999;
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedTrackPair=0;
  m_countPassedPhiMass=0;
  m_countPassedDsMass=0;

}

/*-------------------------------------------*/
TrigEFDsPhiPiHypo::~TrigEFDsPhiPiHypo()
/*-------------------------------------------*/
{
}

/*-------------------------------------------*/
HLT::ErrorCode TrigEFDsPhiPiHypo::hltInitialize()
/*-------------------------------------------*/
{

  // print out properties, cuts


  msg() << MSG::INFO 
      << "Phi mass cuts: " << m_PhiMasslow_cut << " < Mass(phi) < " << m_PhiMasshigh_cut 
      << endreq;
  msg() << MSG::INFO 
      << "Ds mass cuts: " << m_DsMasslow_cut << " < Mass(Ds) < " << m_DsMasshigh_cut 
      << endreq;
  msg() << MSG::INFO 
      << "Initialization completed successfully" 
      << endreq;

  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigEFDsPhiPiHypo::hltFinalize()
/*-------------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Run on events/RoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/RoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed track pair: "  << m_countPassedTrackPair << endreq;
  msg() << MSG::INFO << "RoIs Passed PhiMass: "  << m_countPassedPhiMass << endreq;
  msg() << MSG::INFO << "RoIs Passed DsMass: "  << m_countPassedDsMass << endreq;
  
  return HLT::OK;
}

/*--------------------------------------------------------------*/
HLT::ErrorCode TrigEFDsPhiPiHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
/*--------------------------------------------------------------*/
{
  bool PassedPhiMass=false;
  bool PassedDsMass=false;
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
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endreq;
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



//  create vector for TrigL2Bphys particles
  const TrigEFBphysContainer* trigBphysColl = 0;
  //  const TrigVertexCollection* VertexColl = 0;


  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "EFDsPhiPiFex");

  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigBphysics collection" << endreq;
    }

    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Retrieved Bphys collection  trigBphysColl = " << trigBphysColl << endreq;
  if ( trigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;
 
    return HLT::OK;
  }

  mon_cutCounter = 0; // this is to separate bad input TE and true behaviour of the first cut


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
  

  // now loop over Bphys particles to see if one passes cuts
  for (TrigEFBphysContainer::const_iterator bphysIter = trigBphysColl->begin();
       bphysIter !=  trigBphysColl->end(); ++bphysIter) {    
 
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Bphys particle type: " << (*bphysIter)->particleType() << " with mass "
	  << (*bphysIter)->mass() << endreq;

    // check eta of tracks
    int etaBin=0;
    for (int iBin = 0; iBin < 3; iBin++)
        if ( fabs((*bphysIter)->eta()) > m_Etabin[iBin] ) etaBin = iBin; 
    if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << " bphys eta: " << (*bphysIter)->eta() << " etaBin " << etaBin << endl;

    bool BphysPassedPhiMass=false;
    bool BphysPassedDsMass=false;

    // JK check tracks, for debugging only
    const ElementLinkVector<Rec::TrackParticleContainer> trackVector = (*bphysIter)->trackVector();
    if ( msgLvl() <= MSG::DEBUG ){
      if (trackVector.size() != 0) {
	msg() << MSG::DEBUG << " got track vector size: " << trackVector.size() << endreq;
      } else {
        msg() << MSG::DEBUG << " no track vector!!! "  << endreq;
      }
    }
    ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
        // JW EDM        const Trk::MeasuredPerigee* trackPerigee=(*(*trkIt))->measuredPerigee();
        const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
      if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
      double phi = trackPerigee->parameters()[Trk::phi];
      double theta = trackPerigee->parameters()[Trk::theta];
      double px = trackPerigee->momentum()[Trk::px];
      double py = trackPerigee->momentum()[Trk::py];
      double pt = sqrt(px*px + py*py);
      double eta = -std::log(tan(theta/2));

      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "track " << itrk << " pt phi eta " << pt << " " <<
	phi << " " << eta << endreq;
    }


    if ((*bphysIter)->particleType() == TrigEFBphys::DSPHIPI ) {
      float DsMass = (*bphysIter)->mass();
      if (DsMass > m_DsMasslow_cut[etaBin] && DsMass < m_DsMasshigh_cut[etaBin]) {
         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " mass =  " << DsMass  << " EVENT PASSES!! " << endreq;
         PassedDsMass=true;
         BphysPassedDsMass=true;
      }
      float PhiMass = (*bphysIter)->pSecondDecay()->mass();
      if (PhiMass > m_PhiMasslow_cut[etaBin] && PhiMass < m_PhiMasshigh_cut[etaBin]) {
         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Phi mass =  " << PhiMass  << " PASSES!! " << endreq;
         PassedPhiMass=true;
         BphysPassedPhiMass=true;
      } else {
         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Phi mass =  " << PhiMass  << " FAILS!! " << endreq;
      }

      if (BphysPassedDsMass && BphysPassedPhiMass)  result=true;


    }
  }

  if (PassedPhiMass) {
    mon_cutCounter++;
    if (PassedDsMass) mon_cutCounter++;
  }

  if (PassedPhiMass) m_countPassedPhiMass++;
  if (PassedDsMass) m_countPassedDsMass++;
    if (result) {
     m_countPassedRoIs++;
     if (IdEvent!= (int) m_lastEventPassed) {
        m_countPassedEvents++;
        m_lastEventPassed=IdEvent;
     }
  }              
  // Reach this point successfully
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;

  if (result) pass=true;
  
  return HLT::OK;
}


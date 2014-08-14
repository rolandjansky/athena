/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DsPhiPiHypo.cxx
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

#include "TrigBphysHypo/TrigL2DsPhiPiHypo.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"


class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigL2DsPhiPiHypo::TrigL2DsPhiPiHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
/*------------------------------------------------------------------------------------*/
{

  // Read cuts
  declareProperty( "AcceptAll", m_acceptAll  = false );
  declareProperty("EtaBins", m_Etabin);
  declareProperty("PhiMasslow_cut", m_PhiMasslow_cut);
  declareProperty("PhiMasshigh_cut", m_PhiMasshigh_cut);
  declareProperty("DsMasslow_cut", m_DsMasslow_cut);
  declareProperty("DsMasshigh_cut", m_DsMasshigh_cut);
  declareProperty("useVertexFit", m_useVertexFit=false);
  declareProperty("PhiChi2_cut", m_PhiChi2_cut=-1.);
  declareProperty("DsChi2_cut" , m_DsChi2_cut=-1.);

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
TrigL2DsPhiPiHypo::~TrigL2DsPhiPiHypo()
/*-------------------------------------------*/
{
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2DsPhiPiHypo::hltInitialize()
/*-------------------------------------------*/
{

  // print out properties, cuts
  msg() << MSG::INFO << "Phi mass cuts: " << m_PhiMasslow_cut << " < Mass(phi) < " << m_PhiMasshigh_cut << endreq;
  msg() << MSG::INFO << "Ds mass cuts: " << m_DsMasslow_cut << " < Mass(Ds) < " << m_DsMasshigh_cut  << endreq;
  msg() << MSG::INFO << "Chi2 Cuts: Chi2(Phi) > " << m_PhiChi2_cut  << "; Chi2(Ds) >  " << m_DsChi2_cut  << endreq;
  msg() << MSG::INFO << "Initialization completed successfully" << endreq;


  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigL2DsPhiPiHypo::hltFinalize()
/*-------------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Run on events (RoIs)   : " << m_countTotalEvents  << " ( " << m_countTotalRoI   << " ) " << endreq;
  msg() << MSG::INFO << "Passed events (RoIs)   : " << m_countPassedEvents << " ( " << m_countPassedRoIs << " ) " << endreq;
  msg() << MSG::INFO << "RoIs Passed track pair : " << m_countPassedTrackPair << endreq;
  msg() << MSG::INFO << "RoIs Passed PhiMass    : " << m_countPassedPhiMass << endreq;
  msg() << MSG::INFO << "RoIs Passed DsMass     : " << m_countPassedDsMass << endreq;

  return HLT::OK;
}

/*--------------------------------------------------------------*/
HLT::ErrorCode TrigL2DsPhiPiHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
/*--------------------------------------------------------------*/
{
  bool PassedPhiMass=false;
  bool PassedDsMass=false;
  bool result = false;

  // Retrieve event info
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
  const TrigL2BphysContainer* trigBphysColl = 0;


  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "L2DsPhiPiFexDs");

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


  // now loop over Bphys particles to see if one passes cuts
  for (TrigL2BphysContainer::const_iterator bphysIter = trigBphysColl->begin();
       bphysIter !=  trigBphysColl->end(); ++bphysIter) {

    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bphys particle type: " << (*bphysIter)->particleType() << " with mass "
	  << (*bphysIter)->mass() << endreq;

    // check eta of tracks
    int etaBin=0;

    const ElementLinkVector<TrigInDetTrackCollection> trackVector = (*bphysIter)->trackVector();
    if (trackVector.size() != 0) {
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " got track vector size: " << trackVector.size() << endreq;
    } else {
      if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " no track vector!!! "  << endreq;
    }
    ElementLinkVector<TrigInDetTrackCollection>::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
      if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track " << itrk << " pt phi eta " << (*(*trkIt))->param()->pT() << " " <<
	(*(*trkIt))->param()->phi0() << " " << (*(*trkIt))->param()->eta() << endreq;
      if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track eta " <<(*(*trkIt))->param()->eta() << " now loop over eta bins " << endreq;
      for (int iBin = 0; iBin < 3; iBin++)
           if ( fabs((*(*trkIt))->param()->eta()) > m_Etabin[iBin] ) etaBin = iBin;
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " trck eta: " << (*(*trkIt))->param()->eta() << " etaBin " << etaBin << endreq;
    }


      if ((*bphysIter)->particleType() == TrigL2Bphys::DSPHIPI ) {

      bool BphysPassedPhiMass=false;
      bool BphysPassedDsMass=false;
      float DsMass = (*bphysIter)->mass();
      if (DsMass > m_DsMasslow_cut[etaBin] && DsMass < m_DsMasshigh_cut[etaBin]) {
	if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " mass , fitmass, chi2 =  " << DsMass  << " " << (*bphysIter)->fitmass() << " " << (*bphysIter)->fitchi2() << " DS MASS PASSES!! " << endreq;
         PassedDsMass=true;
         BphysPassedDsMass=true;
      }
      double chi2prob=1.0-Genfun::CumulativeChiSquare((*bphysIter)->fitndof())((*bphysIter)->fitchi2());
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Chi2prob(Ds ) =  " << chi2prob  <<  endreq;
      if (chi2prob<m_DsChi2_cut) msg() << MSG::INFO << "Ds Candidate Chi2 probability lower than cut value" << endreq;
      chi2prob=1.0-Genfun::CumulativeChiSquare((*bphysIter)->pSecondDecay()->fitndof())((*bphysIter)->pSecondDecay()->fitchi2());
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Chi2prob(Phi) =  " << chi2prob << endreq;
      if (chi2prob<m_PhiChi2_cut) msg() << MSG::INFO << "Phi Candidate Chi2 probability lower than cut value" << endreq;

      float PhiMass = (*bphysIter)->pSecondDecay()->mass();
         if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi mass =  " << PhiMass  << endreq;
      if (PhiMass > m_PhiMasslow_cut[etaBin] && PhiMass < m_PhiMasshigh_cut[etaBin]) {
         if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi mass =  " << PhiMass  << " PHI MASS PASSES!! " << endreq;
         PassedPhiMass=true;
         BphysPassedPhiMass=true;
      }
      // phi tracks

    const ElementLinkVector<TrigInDetTrackCollection> trackVectorPhi = (*bphysIter)->pSecondDecay()->trackVector();
    if (trackVectorPhi.size() != 0) {
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " got track vector size: " << trackVectorPhi.size() << endreq;
    } else {
      if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " no track vector!!! "  << endreq;
    }
    ElementLinkVector<TrigInDetTrackCollection>::const_iterator trkIt=trackVectorPhi.begin();
    for (int itrk=0 ; trkIt!= trackVectorPhi.end(); ++itrk, ++trkIt) {
      if ( msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
	msg() << MSG::DEBUG << "track " << itrk << " pt phi eta " << (*(*trkIt))->param()->pT() << " " <<
	(*(*trkIt))->param()->phi0() << " " << (*(*trkIt))->param()->eta() << endreq;
      }
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
  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;


  if (!m_acceptAll) pass = result;

  return HLT::OK;
}


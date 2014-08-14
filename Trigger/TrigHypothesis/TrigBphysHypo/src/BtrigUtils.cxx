/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysHypo/BtrigUtils.h"

#include <math.h>
#include <vector>

#include "GaudiKernel/MsgStream.h"

#include "Particle/TrackParticle.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"

#include <exception>

//--------------------------------------------------------------------
// Base invariant mass calculation using px,py,pz and mass-hypothesis.
//--------------------------------------------------------------------

double InvMass(const std::vector<double> & px, const std::vector<double> & py,
               const std::vector<double> & pz, const std::vector<double> & massHypo) {

  double px_sum = 0;
  double py_sum = 0;
  double pz_sum = 0;
  double e_sum  = 0;

  for ( unsigned int i=0; i<massHypo.size(); i++ ) {
    px_sum += px[i];
    py_sum += py[i];
    pz_sum += pz[i];
    e_sum  += sqrt(massHypo[i]*massHypo[i] + px[i]*px[i] + py[i]*py[i] + pz[i]*pz[i]);
  }

  double m2 = e_sum*e_sum - px_sum*px_sum - py_sum*py_sum - pz_sum*pz_sum;

  if ( m2 < 0 ) return 0;
  else          return sqrt(m2);
}



//--------------------------------------------------
// Invariant mass calculations using TrigInDetTrack.
//--------------------------------------------------

double InvMass(const std::vector<const TrigInDetTrack*> &tracks, const std::vector<double> & massHypo) {

  std::vector<const TrigInDetTrack*>::const_iterator trkItr    = tracks.begin();
  std::vector<const TrigInDetTrack*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    double theta = 2. * atan(exp(-1.0*((*trkItr)->param()->eta())));
    px.push_back( fabs((*trkItr)->param()->pT()) * cos((*trkItr)->param()->phi0()) );
    py.push_back( fabs((*trkItr)->param()->pT()) * sin((*trkItr)->param()->phi0()) );
    pz.push_back( fabs((*trkItr)->param()->pT()) / tan(theta) );
  }

  return InvMass(px, py, pz, massHypo);
}



//--------------------------------------------------------
// Invariant mass calculations using TrigInDetTrackFitPar.
//--------------------------------------------------------

double InvMass(const std::vector<const TrigInDetTrackFitPar*> & tracks, const std::vector<double> & massHypo) {

  std::vector<const TrigInDetTrackFitPar*>::const_iterator trkItr    = tracks.begin();
  std::vector<const TrigInDetTrackFitPar*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    double theta = 2. * atan(exp(-1.0*((*trkItr)->eta())));
    px.push_back( fabs((*trkItr)->pT()) * cos((*trkItr)->phi0()) );
    py.push_back( fabs((*trkItr)->pT()) * sin((*trkItr)->phi0()) );
    pz.push_back( fabs((*trkItr)->pT()) / tan(theta) );
  }

  return InvMass(px, py, pz, massHypo);
}



//--------------------------------------------------------
// Invariant mass calculations using Trk::MeasuredPerigee.
//--------------------------------------------------------

double InvMass(const std::vector<const Trk::TrackParameters*> & tracks, const std::vector<double> & massHypo) {

  std::vector<const Trk::TrackParameters*>::const_iterator trkItr    = tracks.begin();
  std::vector<const Trk::TrackParameters*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    px.push_back( (*trkItr)->momentum()[Trk::px] );
    py.push_back( (*trkItr)->momentum()[Trk::py] );
    pz.push_back( (*trkItr)->momentum()[Trk::pz] );
  }

  return InvMass(px, py, pz, massHypo);
}

//--------------------------------------------------------
// Invariant mass calculations using Trk::Perigee.
//--------------------------------------------------------

double InvMass(const std::vector<const Trk::Perigee*> & tracks, const std::vector<double> & massHypo) {
    auto trkItr    = tracks.begin();
    auto trkItrEnd = tracks.end();

    std::vector<double> px, py, pz;
    
    for ( ; trkItr!=trkItrEnd; trkItr++ ) {
        px.push_back( (*trkItr)->momentum()[Trk::px] );
        py.push_back( (*trkItr)->momentum()[Trk::py] );
        pz.push_back( (*trkItr)->momentum()[Trk::pz] );
    }
    
    return InvMass(px, py, pz, massHypo);
}


//----------------------------------------------
// Invariant mass calculations using Trk::Track.
//----------------------------------------------

double InvMass(const std::vector<const Trk::Track*> & tracks, const std::vector<double> & massHypo) {

  std::vector<const Trk::Track*>::const_iterator trkItr    = tracks.begin();
  std::vector<const Trk::Track*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
      // JW EDM
      //    px.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * ((*trkItr)->perigeeParameters()->cosPhi()) );
      //    py.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * ((*trkItr)->perigeeParameters()->sinPhi()) );
      //    pz.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * ((*trkItr)->perigeeParameters()->cotTheta()) );
      px.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * cos((*trkItr)->perigeeParameters()->parameters()[Trk::phi0]));
      py.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * sin((*trkItr)->perigeeParameters()->parameters()[Trk::phi0]));
      pz.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * 1./tan((*trkItr)->perigeeParameters()->parameters()[Trk::theta]));

  }

  return InvMass(px, py, pz, massHypo);
}

//-------------------------------------------------------------------
// Obtain list of Inner Detector tracks from xAOD::Muon.
//-------------------------------------------------------------------
HLT::ErrorCode GetxAODMuonTracks(const xAOD::MuonContainer* trigMuon, std::vector<const Trk::Track*>& indetTracks, MsgStream& msg) {
    bool debug = msg.level() <= MSG::DEBUG;
    if ( !trigMuon ) {
        msg << MSG::ERROR << "Retrieval of MuonContainer from vector failed" << endreq;
        return HLT::NAV_ERROR;
    } else {
        if (debug ) msg << MSG::DEBUG << "MuonContainer OK with size " << trigMuon->size() << endreq;
    }

    xAOD::MuonContainer::const_iterator MuonItr    = trigMuon->begin();
    xAOD::MuonContainer::const_iterator MuonItrEnd = trigMuon->end();

    for ( int i=0; MuonItr!=MuonItrEnd; MuonItr++, i++ ) { // loops over muons within the RoI
        if ( debug ) msg << MSG::DEBUG << "Looking at TrigMuonEFInfo " << i << endreq;
        const xAOD::Muon* muonInfo = *MuonItr;
        if ( !muonInfo ) {
            if ( debug ) msg << MSG::DEBUG << "No xAOD::Muon found" << endreq;
            continue;
        }
        if ( debug ) {
            msg << MSG::DEBUG << "Have xAOD::Muon with ptr: " <<    muonInfo << endreq;

            const ElementLink< xAOD::TrackParticleContainer >& mstp = muonInfo->muonSpectrometerTrackParticleLink();
            msg << MSG::DEBUG << "  mstp: " << mstp << " " << mstp.isValid() << endreq;
            if ( mstp.isValid() ) {
                const xAOD::TrackParticle * tp = *mstp;
                msg << MSG::DEBUG << "    tp: " <<   tp << " " << tp->charge() << " " << tp->pt() << "  " << tp->eta() << "  " << tp->phi() << "  " << tp->track() <<endreq;
            }
 
            const ElementLink< xAOD::TrackParticleContainer >& cbtp = muonInfo->combinedTrackParticleLink();
            msg << MSG::DEBUG << "  cbtp: " << cbtp << " " << cbtp.isValid() << endreq;
            if ( cbtp.isValid() ) {
                const xAOD::TrackParticle * tp = *cbtp;
                msg << MSG::DEBUG << "    tp: " <<   tp << " " << tp->charge() << " " << tp->pt() << "  " << tp->eta() << "  " << tp->phi() << "  " << tp->track()<<endreq;
            }
            const ElementLink< xAOD::TrackParticleContainer >& idtp = muonInfo->inDetTrackParticleLink();
            msg << MSG::DEBUG << "  idtp: " << idtp << " " << idtp.isValid() << endreq;
            if ( idtp.isValid() ) {
                const xAOD::TrackParticle * tp = *idtp;
                msg << MSG::DEBUG << "    tp: " <<   tp << " " << tp->charge() << " " << tp->pt() << "  " << tp->eta() << "  " << tp->phi() << "  " << tp->track() <<endreq;
            }

        } // end debug
        //xAOD::Muon::MuonType muonType = muonInfo->muonType();
        const xAOD::TrackParticle* tpCombinedMuon(0);
        const xAOD::TrackParticle* indetTrackParticle(0);
        
        if (muonInfo->combinedTrackParticleLink().isValid()) tpCombinedMuon  = *(muonInfo->combinedTrackParticleLink());
        if (muonInfo->inDetTrackParticleLink().isValid()) indetTrackParticle = *(muonInfo->inDetTrackParticleLink());

        //check for combined muon
        //const xAOD::TrackParticle* tpCombinedMuon = muonInfo->trackParticle( xAOD::Muon::CombinedTrackParticle);
        if (!tpCombinedMuon) {
            if ( debug ) msg << MSG::DEBUG << "No combined muon TrackParticle found" << endreq;
            continue;
        }
        // now search for the ID track associated to it
        //const xAOD::TrackParticle* indetTrackParticle  = muonInfo->trackParticle( xAOD::Muon::InnerDetectorTrackParticle);
        if (!indetTrackParticle) {
            if ( debug ) msg << MSG::DEBUG << "No innerdetector muon TrackParticle found" << endreq;
            continue;
        }

        const Trk::Track* indetTrack = indetTrackParticle->track();
        if ( !indetTrack ) {
            if ( debug ) msg << MSG::DEBUG << "No id muon id Trk::Track found" << endreq;
            continue;
        }
        indetTracks.push_back(indetTrack);

        
    } // loop over muons in RoI
    return HLT::OK;

} //GetxAODMuonTracks



//-------------------------------------------------------------------
// Obtain list of Inner Detector tracks from TrigMuonEFInfoContainer.
//-------------------------------------------------------------------

HLT::ErrorCode GetTrigMuonEFInfoTracks(const TrigMuonEFInfoContainer* trigMuon, std::vector<const Trk::Track*> &indetTracks, MsgStream &msg) {

  bool debug = msg.level() <= MSG::DEBUG;

  if ( !trigMuon ) {
    msg << MSG::ERROR << "Retrieval of TrigMuonEFInfoContainer from vector failed" << endreq;
    return HLT::NAV_ERROR;
  } else {
    if (debug ) msg << MSG::DEBUG << "TrigMuonEFInfoContainer OK with size " << trigMuon->size() << endreq;
  }

  TrigMuonEFInfoContainer::const_iterator MuonItr    = trigMuon->begin();
  TrigMuonEFInfoContainer::const_iterator MuonItrEnd = trigMuon->end();

  for ( int i=0; MuonItr!=MuonItrEnd; MuonItr++, i++ ) { // loops over muons within the RoI
    if ( debug ) msg << MSG::DEBUG << "Looking at TrigMuonEFInfo " << i << endreq;
    TrigMuonEFInfo* muonInfo = (*MuonItr);
    if ( !muonInfo ) {
      if ( debug ) msg << MSG::DEBUG << "No TrigMuonEFInfo found" << endreq;
      continue;
    }
    if ( muonInfo->hasTrack() ) { // was there a muon in this RoI ?
      TrigMuonEFInfoTrackContainer *muonTracks = muonInfo->TrackContainer(); // get the muon TrackContainer, there might be more then one muon in this RoI
      TrigMuonEFInfoTrackContainer::const_iterator TrackItr    = muonTracks->begin();
      TrigMuonEFInfoTrackContainer::const_iterator TrackItrEnd = muonTracks->end();
      for ( ; TrackItr!=TrackItrEnd; TrackItr++ ) { // loop over muon tracks container content
        TrigMuonEFInfoTrack* muonInfoTrack = (*TrackItr);
        if ( debug ) msg << MSG::DEBUG << "Muon info track type: " << muonInfoTrack->MuonType() << endreq;
        if ( !muonInfoTrack->hasCombinedTrack() ) {
          if ( debug ) msg << MSG::DEBUG << "Combined track not initialized" << endreq;
          continue;
        }
        TrigMuonEFCbTrack* combTrack = muonInfoTrack->CombinedTrack();
        if ( !combTrack ) {
          if ( debug ) msg << MSG::DEBUG << "No TrigMuonEFCbTrack found" << endreq;
          continue;
        }
        if ( debug ) msg << MSG::DEBUG << "Retrieved combined track" << endreq;
        const Rec::TrackParticle* indetTrackParticle = combTrack->getIDTrackParticle();
        if ( !indetTrackParticle ) {
          if ( debug ) msg << MSG::DEBUG << "No combined muon Rec::TrackParticle found" << endreq;
          continue;
        }
        const Trk::Track* indetTrack = indetTrackParticle->originalTrack();
        if ( !indetTrack ) {
          if ( debug ) msg << MSG::DEBUG << "No combined muon Trk::Track found" << endreq;
          continue;
        }
        indetTracks.push_back(indetTrack);
      }
    }
  }

  return HLT::OK;
}


double fabsDeltaPhi(double phi1, double phi2) {
    double dPhi=fabs(phi1 - phi2);
    if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
    return dPhi;
}

double fabsDeltaEta(double eta1, double eta2) {
    return fabs( eta1 - eta2);
}


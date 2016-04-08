/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// SingleTrackDiffAlg.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "TrkValAlgs/SingleTrackDiffAlg.h"
// Gaudi 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
// Trk
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
//#include "TrkParameters/Perigee.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"

#include "TrkValInterfaces/ITrackDiff.h"

Trk::SingleTrackDiffAlg::SingleTrackDiffAlg(const std::string& name, ISvcLocator* pSvcLocator):
        AthAlgorithm(name,pSvcLocator),
        m_trackDiffTool("Trk::TrackDiff/TrackDiff"),
        m_referenceTrackCollection("simulatedTracks"),
        m_comparedTrackCollection("Tracks")

{
    declareProperty("ReferenceTrackCollection", m_referenceTrackCollection, "Name of the reference track collection");
    declareProperty("ComparedTrackCollection",  m_comparedTrackCollection,  "Name of the track collection, which tracks will be compared");
    declareProperty("TrackDiffTool",            m_trackDiffTool,            "Tool to compare two tracks");

}

Trk::SingleTrackDiffAlg::~SingleTrackDiffAlg() {}

StatusCode Trk::SingleTrackDiffAlg::initialize() {
    msg(MSG::INFO) <<"SingleTrackDiffAlg initialize()" << endreq;


    // Get TrackDiff Tool
    StatusCode sc = m_trackDiffTool.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve "<< m_trackDiffTool <<" (to compare tracks) "<< endreq;
        return sc;
    }

    return StatusCode::SUCCESS;
}

StatusCode Trk::SingleTrackDiffAlg::execute() {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)    << "SingleTrackDiffAlg execute() start" << endreq;

    StatusCode sc = StatusCode::SUCCESS;

    const TrackCollection* referenceTracks = 0;
    const TrackCollection* comparedTracks = 0;
    // get reference collection
    if (m_referenceTrackCollection != "") {
        sc = evtStore()->retrieve(referenceTracks, m_referenceTrackCollection);
        if (sc.isFailure()) {
            msg(MSG::ERROR) <<"Reference tracks not found:  " << m_referenceTrackCollection << endreq;
            return StatusCode::FAILURE;
        } else {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"Reference tracks found: " << m_referenceTrackCollection <<endreq;
        }
    }else{
        msg(MSG::ERROR) <<"No reference Track collection given!" <<endreq;
        return StatusCode::FAILURE;
    }
    // get collection for comparison
    if (m_comparedTrackCollection != "") {
        sc = evtStore()->retrieve(comparedTracks, m_comparedTrackCollection);
        if (sc.isFailure()) {
            msg(MSG::ERROR) <<"Tracks for comparison not found:  " << m_comparedTrackCollection << endreq;
            return StatusCode::FAILURE;
        } else {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"Tracks for comparison found: " << m_comparedTrackCollection <<endreq;
        }
    }else{
        msg(MSG::ERROR) <<"No Track collection for comparison given!" <<endreq;
        return StatusCode::FAILURE;
    }
    // just compare the first tracks of both collections
    TrackCollection::const_iterator refTrackIterator = referenceTracks->begin();
    TrackCollection::const_iterator compTrackIterator = comparedTracks->begin();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << referenceTracks->size() <<" reference track(s) and " << comparedTracks->size() <<" comparison track(s)" <<endreq;
    if ( refTrackIterator == referenceTracks->end() ) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"reference collection containes no tracks." <<endreq;
        return StatusCode::SUCCESS;
    }
    if ( compTrackIterator == comparedTracks->end() ) {
        msg(MSG::INFO) <<"collection for comparison containes no tracks in contrast to the reference collection" <<endreq;
        return StatusCode::SUCCESS;
    }
    if (!(*refTrackIterator)) {
        msg(MSG::WARNING) <<"reference track collection containes a NULL pointer" <<endreq;
        return StatusCode::SUCCESS;
    }
    // find track in compared collection which fits best to track in reference collection
    const Perigee* refPerigee = (*refTrackIterator)->perigeeParameters();
    const Track* compTrack = 0; // later for-loop with large min chi2 will assign a sane track 
    if (refPerigee) {
        //double minDeltaqOverP = 1.e20;
        double minChi2 = 1.e30;
        for (; compTrackIterator != comparedTracks->end(); compTrackIterator++) {
            if (!(*compTrackIterator)) {
                msg(MSG::WARNING) <<"track collection for comparison containes a NULL pointer" <<endreq;
                continue;
            }
            const Perigee* compPerigee = (*compTrackIterator)->perigeeParameters();
            if (compPerigee) {
                double chi2 = -1.;
                if (compPerigee->covariance()) {
		  AmgSymMatrix(5) weight = compPerigee->covariance()->inverse();
		  AmgVector(5) paramdiff = refPerigee->parameters() - compPerigee->parameters();
		  chi2 = paramdiff.transpose() * weight * paramdiff;
                    if (chi2 < minChi2) {
                        minChi2 = chi2;
                        compTrack = (*compTrackIterator);
                    }
                } /* else {
                    double deltaqOverP = fabs(refPerigee->pT() - compPerigee->pT());
                    if (deltaqOverP < minDeltaqOverP) {
                        minDeltaqOverP = deltaqOverP;
                        compTrack = (*compTrackIterator);
                    }
                }*/
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"pTRef: " << refPerigee->pT() << " pT: " << compPerigee->pT() <<" ThetaRef: " << refPerigee->parameters()[Trk::theta] << " Theta: " << compPerigee->parameters()[Trk::theta]<<" PhiRef: " << refPerigee->parameters()[Trk::phi0] << " Phi: " << compPerigee->parameters()[Trk::phi0]<<" chi2: " << chi2 <<endreq;

            }
        }
        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"minimum Delta pT: " << minDeltaqOverP <<endreq;
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"minimum chi2: " << minChi2 <<endreq;
    }
    if (compTrack) {
        sc = m_trackDiffTool->diff( *(*refTrackIterator), *(compTrack) );
    } else {
        return StatusCode::SUCCESS;
    }
//     refTrackIterator = (*referenceTracks).end();
//     compTrackIterator = (*comparedTracks).end();
    return sc;
    //return StatusCode::SUCCESS;
}

StatusCode Trk::SingleTrackDiffAlg::finalize() {
    msg(MSG::INFO)  << "SingleTrackDiffAlg finalize()" << endreq;

    return StatusCode::SUCCESS;
}



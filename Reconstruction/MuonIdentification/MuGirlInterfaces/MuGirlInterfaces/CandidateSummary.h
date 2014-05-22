/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSCANDIDATESUMMARY_H
#define MUGIRLNSCANDIDATESUMMARY_H

#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticle.h"

class TrigMuonEFTrack;
class MuonFeature;

namespace Muon
{
class MuonSegment;
}
namespace Trk
{
class TrackSurfaceIntersection;
class Track;
}


namespace MuGirlNS
{
//typedef std::vector<const Muon::MuonSegment*> MuonSegmentList;
class Intersection;

struct CandidateSummary
{
    bool startFromMF;
    bool startFromEF;
    bool startFromBetaRefit;
    bool startFromMuonEntry;

    Trk::TrackSurfaceIntersection*  pIDTrkIsect;
    const MuonFeature* pMuonFeature;
    const TrigMuonEFTrack* pMuonEFTrack;

    std::vector<Amg::Vector3D> innerIsectPositions;
    std::vector<const Muon::MuonSegment*> muonSegmentList;

    const xAOD::TrackParticle* pTrackParticle;
    const xAOD::TrackParticle* pLowBetaTrack;
    const xAOD::TrackParticle* pLowBetaExtrpTrack;
    const xAOD::TrackParticle* pRefittedTrack;
    const xAOD::TrackParticle* pMSRefittedTrack;

    const Trk::Track* pTrkTrack;
    const Trk::Track* pTrkLowBeta;
    const Trk::Track* pTrkLowBetaExtr;
    const Trk::Track* pTrkRefitted;
    const Trk::Track* pTrkMSRefitted;

    int numMdtHits;
    int numRpcPhiHits;
    int numRpcEtaHits;
    int numCscPhiHits;
    int numCscEtaHits;
    int numMdtSegs;
    int numRpcSegs;
    int numMdtHitsInSeg;
    int numRpcHitsInSeg;
    int numTgcRpcPhiHits;
    int numTgcRpcEtaHits;
    int numTgcRpcSegs;
    int numTgcRpcHitsInSeg;
    int numTgcPhiHits;
    int numTgcEtaHits;
    int numTgcSegs;
    int numTgcHitsInSeg;
    int numCscSegs;
    double innAngle;
    double innDelta;
    double midAngle;
    double midDelta;
    double nnBarrel;
    double nnEndCap;
    double PassNNsel;
    double beta;
    double qOverP;
    double lhr;
    int saveAnn;
    int saveStau;
    StauRpcHitsLists* rpcHitsLists;

    CandidateSummary():
        rpcHitsLists(NULL)
    {}
    ~CandidateSummary()
    {
        if(NULL!=rpcHitsLists)
        {
            for(StauRpcHitsLists::iterator it = rpcHitsLists->begin(); it != rpcHitsLists->end(); it++)
            {
                StauRpcHitsList* pRpcHits = *it;
                for(StauRpcHitsList::iterator it1 = pRpcHits->begin(); it1 != pRpcHits->end(); it1++)
                {
                    delete *it1;
                }

                pRpcHits->clear();
                delete pRpcHits;
            }

            rpcHitsLists->clear();
        }
        delete rpcHitsLists;
        startFromMF = false;
        startFromEF = false;
        innerIsectPositions.clear();
    }

};

typedef std::vector<const CandidateSummary*> CandidateSummaryList; // sofia: vector of CandidateSummary
}

#endif // MUGIRLNSCANDIDATESUMMARY_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "MuonCombinedEvent/RpcSegmentInfo.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticle.h"
#include <algorithm>

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

    CandidateSummary()
        : rpcHitsLists(NULL)
    {}
    ~CandidateSummary()
    {
        if (rpcHitsLists != NULL)
        {
            for (auto pRpcHits : *rpcHitsLists)
            {
                for (auto pHit : *pRpcHits)
                    delete pHit;

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

enum StauHitTechnology { RPC_STAU_HIT=0, RPCETA_STAU_HIT, MDTT_STAU_HIT, CALO_STAU_HIT };

inline std::string hitTechnologyName(StauHitTechnology eTech)
{
    switch (eTech)
    {
        case RPC_STAU_HIT: return "RPC";
        case MDTT_STAU_HIT: return "MDTT";
        case CALO_STAU_HIT: return "CALO";
        default: return std::string();
    }
}

struct StauHit
{
    StauHitTechnology eTech;
    double mToF;
    double x;
    double y;
    double z;
    Identifier id;
    double e;
    double error;
    double shift;
    bool measuresEta;
    double propagationTime;
    
    StauHit() :
            mToF(-1), x(0), y(0), z(0), e(-1), error(-1), shift(0), measuresEta(false), propagationTime(0)
    {
    }
    StauHit(StauHitTechnology tech, double tof, double ix, double iy, double iz, Identifier iid,
            double ie, double er = -1, double sh = 0, bool isEta = false, double propTime = 0) :
                    eTech(tech),
                    mToF(tof),
                    x(ix),
                    y(iy),
                    z(iz),
                    id(iid),
                    e(ie),
                    error(er),
                    shift(sh),
                    measuresEta(isEta),
                    propagationTime(propTime)
    {
    }
};
typedef std::vector<StauHit> StauHits;

struct StauExtras
{
    double ann;
    double betaAll, betaAllt;
    int numRpcHitsInSeg, numCaloCells;
    double rpcBetaAvg, rpcBetaRms, rpcBetaChi2; int rpcBetaDof;
    double mdtBetaAvg, mdtBetaRms, mdtBetaChi2; int mdtBetaDof;
    double caloBetaAvg, caloBetaRms, caloBetaChi2; int caloBetaDof;
    StauHits hits;

    void addHits(StauHits stauHits)
    {
        std::copy(stauHits.begin(), stauHits.end(), std::back_inserter(hits));
    }        
};

struct RHExtras
{
    int numCaloCells;
    double caloBetaAvg, caloBetaRms;
    StauHits hits;

    void addHits(StauHits stauHits)
    {
        std::copy(stauHits.begin(), stauHits.end(), std::back_inserter(hits));
    }        
};

} // namespace Muon

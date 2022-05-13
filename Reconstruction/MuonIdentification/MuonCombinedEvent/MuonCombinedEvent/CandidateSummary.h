/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include <algorithm>

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCombinedEvent/RpcSegmentInfo.h"
#include "xAODTracking/TrackParticle.h"

class TrigMuonEFTrack;
class MuonFeature;

namespace Muon {
    class MuonSegment;
}
namespace Trk {
    class TrackSurfaceIntersection;
    class Track;
}  // namespace Trk

namespace MuGirlNS {
    // typedef std::vector<const Muon::MuonSegment*> MuonSegmentList;
    class Intersection;

    class CandidateSummary {
    public:
        bool startFromMF;
        bool startFromEF;
        bool startFromBetaRefit;
        bool startFromMuonEntry;

        Trk::TrackSurfaceIntersection* pIDTrkIsect;
        const MuonFeature* pMuonFeature;
        const TrigMuonEFTrack* pMuonEFTrack;

        std::vector<Amg::Vector3D> innerIsectPositions;
        std::vector<const Muon::MuonSegment*> muonSegmentList;

        const xAOD::TrackParticle* pTrackParticle;
        const xAOD::TrackParticle* pLowBetaTrack;
        const xAOD::TrackParticle* pLowBetaExtrpTrack;
        const xAOD::TrackParticle* pRefittedTrack;
        std::unique_ptr<const xAOD::TrackParticle> pMSRefittedTrack;

        const Trk::Track* pTrkTrack;
        const Trk::Track* pTrkLowBeta;
        //  const Trk::Track* pTrkLowBetaExtr;
        const Trk::Track* pTrkRefitted;
        std::unique_ptr<const Trk::Track> pTrkMSRefitted;

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
        bool isSiliconAssociated;
        StauRpcHitsLists* rpcHitsLists;

        CandidateSummary() :
            startFromMF(false),
            startFromEF(false),
            startFromBetaRefit(false),
            startFromMuonEntry(false),
            pIDTrkIsect(nullptr),
            pMuonFeature(nullptr),
            pMuonEFTrack(nullptr),
            pTrackParticle(nullptr),
            pLowBetaTrack(nullptr),
            pLowBetaExtrpTrack(nullptr),
            pRefittedTrack(nullptr),
            pMSRefittedTrack(nullptr),
            pTrkTrack(nullptr),
            pTrkLowBeta(nullptr),
            pTrkRefitted(nullptr),
            pTrkMSRefitted(nullptr),
            numMdtHits(-1),
            numRpcPhiHits(-1),
            numRpcEtaHits(-1),
            numCscPhiHits(-1),
            numCscEtaHits(-1),
            numMdtSegs(-1),
            numRpcSegs(-1),
            numMdtHitsInSeg(-1),
            numRpcHitsInSeg(-1),
            numTgcRpcPhiHits(-1),
            numTgcRpcEtaHits(-1),
            numTgcRpcSegs(-1),
            numTgcRpcHitsInSeg(-1),
            numTgcPhiHits(-1),
            numTgcEtaHits(-1),
            numTgcSegs(-1),
            numTgcHitsInSeg(-1),
            numCscSegs(-1),
            innAngle(-999.),
            innDelta(-999.),
            midAngle(-999.),
            midDelta(-999.),
            nnBarrel(-999.),
            nnEndCap(-999.),
            PassNNsel(-999.),
            beta(-999.),
            qOverP(-999.),
            lhr(-999.),
            saveAnn(-1),
            saveStau(-1),
            isSiliconAssociated(false),
            rpcHitsLists(NULL) {}

        ~CandidateSummary() {
            if (rpcHitsLists != NULL) {
                for (auto pRpcHits : *rpcHitsLists) {
                    for (auto pHit : *pRpcHits) delete pHit;

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

        void clear() {
            startFromMF = false;
            startFromEF = false;
            startFromBetaRefit = false;
            startFromMuonEntry = false;

            pIDTrkIsect = nullptr;
            pMuonFeature = nullptr;
            pMuonEFTrack = nullptr;

            innerIsectPositions.clear();
            muonSegmentList.clear();

            pTrackParticle = nullptr;
            pLowBetaTrack = nullptr;
            pLowBetaExtrpTrack = nullptr;
            pRefittedTrack = nullptr;
            pMSRefittedTrack.reset();

            pTrkTrack = nullptr;
            pTrkLowBeta = nullptr;
            pTrkRefitted = nullptr;
            pTrkMSRefitted.reset();

            numMdtHits = 0;
            numRpcPhiHits = 0;
            numRpcEtaHits = 0;
            numCscPhiHits = 0;
            numCscEtaHits = 0;
            numMdtSegs = 0;
            numRpcSegs = 0;
            numMdtHitsInSeg = 0;
            numRpcHitsInSeg = 0;
            numTgcRpcPhiHits = 0;
            numTgcRpcEtaHits = 0;
            numTgcRpcSegs = 0;
            numTgcRpcHitsInSeg = 0;
            numTgcPhiHits = 0;
            numTgcEtaHits = 0;
            numTgcSegs = 0;
            numTgcHitsInSeg = 0;
            numCscSegs = 0;
            innAngle = 0;
            innDelta = 0;
            midAngle = 0;
            midDelta = 0;
            nnBarrel = 0;
            nnEndCap = 0;
            PassNNsel = 0;
            beta = 0;
            qOverP = 0;
            lhr = 0;
            saveAnn = 0;
            saveStau = 0;
            isSiliconAssociated = false;
            rpcHitsLists = nullptr;
        }
    };

    typedef std::vector<const CandidateSummary*> CandidateSummaryList;  // sofia: vector of CandidateSummary

    enum StauHitTechnology { RPC_STAU_HIT = 0, RPCETA_STAU_HIT, MDTT_STAU_HIT, CALO_STAU_HIT, CSC_STAU_HIT };

    inline std::string hitTechnologyName(StauHitTechnology eTech) {
        switch (eTech) {
            case RPC_STAU_HIT: return "RPC";
            case RPCETA_STAU_HIT: return "RPCETA";
            case MDTT_STAU_HIT: return "MDTT";
            case CALO_STAU_HIT: return "CALO";
            case CSC_STAU_HIT: return "CSC";
            default: return std::string();
        }
    }

    struct StauHit {
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

        StauHit() : mToF(-1), x(0), y(0), z(0), e(-1), error(-1), shift(0), measuresEta(false), propagationTime(0) {}
        StauHit(StauHitTechnology tech, double tof, double ix, double iy, double iz, Identifier iid, double ie, double er = -1,
                double sh = 0, bool isEta = false, double propTime = 0) :
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
            propagationTime(propTime) {}
    };
    typedef std::vector<StauHit> StauHits;

    struct StauExtras {
        double ann;
        double betaAll, betaAllt;
        int numRpcHitsInSeg, numCaloCells;
        double rpcBetaAvg, rpcBetaRms, rpcBetaChi2;
        int rpcBetaDof;
        double mdtBetaAvg, mdtBetaRms, mdtBetaChi2;
        int mdtBetaDof;
        double caloBetaAvg, caloBetaRms, caloBetaChi2;
        int caloBetaDof;
        StauHits hits;

        void addHits(StauHits stauHits) { std::copy(stauHits.begin(), stauHits.end(), std::back_inserter(hits)); }
    };

    struct RHExtras {
        int numCaloCells;
        double caloBetaAvg, caloBetaRms;
        StauHits hits;

        void addHits(StauHits stauHits) { std::copy(stauHits.begin(), stauHits.end(), std::back_inserter(hits)); }
    };

}  // namespace MuGirlNS

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBTRACKSUMMARY_H
#define MUONCALIBTRACKSUMMARY_H

#include <set>
#include <string>
#include <vector>

#include "CxxUtils/checker_macros.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    class ATLAS_NOT_THREAD_SAFE MuonCalibTrackSummary {
    public:
        struct ChamberHitSummary {
            struct Projection {
                int nhits{0};
                int nholes{0};
                int noutliers{0};
                int ndeltas{0};
            };

            ChamberHitSummary(MuonFixedId chID) : chId{chID} {}

            MuonFixedId chId{};
            bool isMdt() const { return chId.is_mdt(); }

            int nhits() const { return m_first.nhits + m_second.nhits; }
            int nholes() const { return m_first.nholes + m_second.nholes; }
            int noutliers() const { return m_first.noutliers + m_second.noutliers; }
            int ndeltas() const { return m_first.ndeltas + m_second.ndeltas; }

            int netaHits() const { return isMdt() ? nhits() : m_first.nhits; }
            int nphiHits() const { return isMdt() ? 0 : m_second.nhits; }

            int nMdtHitsMl1() const { return isMdt() ? m_first.nhits : 0; }
            int nMdtHitsMl2() const { return isMdt() ? m_second.nhits : 0; }

            // before using make sure isMdt == false!
            Projection& etaProjection() { return m_first; }
            Projection& phiProjection() { return m_second; }
            const Projection& etaProjection() const { return m_first; }
            const Projection& phiProjection() const { return m_second; }

        private:
            Projection m_first;   //<! eta projections for cluster chambers, first multi layer for mdt chambers
            Projection m_second;  //<! phi projections for cluster chambers, first multi layer for mdt chambers
        };

        unsigned int nhits{0};
        unsigned int npixel{0};
        unsigned int nsct{0};
        unsigned int ntrtBarrel{0};
        unsigned int ntrtEndcap{0};
        unsigned int nhitsUpperHemisphere{0};
        unsigned int nhitsLowerHemisphere{0};
        unsigned int nscatters{0};
        unsigned int nholes{0};
        unsigned int noutliers{0};
        unsigned int npseudo{0};
        unsigned int ntubeHits{0};
        unsigned int nmdtHits{0};
        unsigned int nmdtHitsBelowADCCut{0};
        unsigned int nrpcPhiHits{0};
        unsigned int nrpcEtaHits{0};
        unsigned int ntgcPhiHits{0};
        unsigned int ntgcEtaHits{0};
        unsigned int ncscPhiHits{0};
        unsigned int ncscEtaHits{0};

        bool hasEndcapA{false};
        bool hasEndcapC{false};
        bool hasBarrel{false};

        bool hasEndcapLayersWithTGC{false};
        bool hasBarrelLayersWithRPC{false};

        bool isTrack{false};

        /** access to number of chambers per technology on the track */
        unsigned int nmdtCh() const;
        unsigned int nrpcCh(bool phi) const;
        unsigned int ntgcCh(bool phi) const;
        unsigned int ncscCh(bool phi) const;

        unsigned int nrpcEtaPhiCh() const;
        unsigned int ntgcEtaPhiCh() const;
        unsigned int ncscEtaPhiCh() const;

        unsigned int ntrt() const { return ntrtBarrel + ntrtEndcap; }

        /** access to number of eta/phi trigger hits */
        unsigned int nphiTrigHits() const { return nrpcPhiHits + ntgcPhiHits; }
        unsigned int netaTrigHits() const { return nrpcEtaHits + ntgcEtaHits; }

        /** access to number of MDT hits per ml (ml=1,2) */
        unsigned int nmdtHitsPerMl(int ml) const;

        bool hasEndcap() const { return hasEndcapA || hasEndcapC; }
        bool hasMuon() const { return nmdtHits + nrpcEtaHits + nrpcPhiHits + ntgcEtaHits + ntgcPhiHits + ncscEtaHits + ncscPhiHits > 0; }
        bool hasId() const { return npixel + nsct + ntrt() > 0; }

        std::vector<ChamberHitSummary> chamberHitSummary;

        /** set of phi layers on the track*/
        std::set<MuonFixedIdManipulator::PhiStationIndex> phiStationLayers;

        /** set of precision layers on the track */
        std::set<MuonFixedIdManipulator::StationIndex> precisionStationLayers;

        /** set of phi layers which also have eta hits on the track*/
        std::set<MuonFixedIdManipulator::PhiStationIndex> phiEtaStationLayers;

        /** set of precision layers with (MDT: two multi layers, CSC: at least 2 eta and phi hits) on the track */
        std::set<MuonFixedIdManipulator::StationIndex> goodPrecisionStationLayers;

        /** dump counts to string */
        std::string dump() const;
    };
}  // namespace MuonCalib

#endif

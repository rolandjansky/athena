/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONHITSUMMARYTOOL_H
#define MUON_IMUONHITSUMMARYTOOL_H

#include <iostream>
#include <map>
#include <set>

#include "GaudiKernel/IAlgTool.h"
#include "MuonStationIndex/MuonStationIndex.h"

namespace Trk {
    class Track;
    class TrackSummary;
    class MuonTrackSummary;
    class MeasurementBase;
}  // namespace Trk

static const InterfaceID IID_IMuonHitSummaryTool("Muon::IMuonHitSummaryTool", 1, 0);

namespace Muon {
    class MuonSegment;

    /** Interface for tools calculating hit count summaries for track */
    class IMuonHitSummaryTool : virtual public IAlgTool {
    public:
        struct HitSummary {
            HitSummary() :
                nprecisionHits(0),
                nprecisionGoodHits(0),
                netaTriggerLayers(0),
                nphiLayers(0),
                netaPhiLayers(0),
                nprecisionHoles(0),
                netaTriggerHoleLayers(0),
                nphiHoleLayers(0),
                nprecisionOutliers(0),
                nprecisionCloseHits(0),
                noutBoundsHits(0),
                isEndcap(false),
                isSmall(false) {}

            /** hit counts */
            unsigned int nprecisionHits;     /** number of precision hits */
            unsigned int nprecisionGoodHits; /** number of precision hits that are not deweighted */
            unsigned int netaTriggerLayers;  /** number of eta trigger layers */
            unsigned int nphiLayers;         /** number of phi layers */
            unsigned int netaPhiLayers;      /** number of paired eta/phi trigger layers */

            /** hole counts */
            unsigned int nprecisionHoles;       /** number of precision holes */
            unsigned int netaTriggerHoleLayers; /** number of eta trigger layer holes */
            unsigned int nphiHoleLayers;        /** number of phi layer holes */

            /** outliers and close hits counts */
            unsigned int nprecisionOutliers;  /** number of outliers hits */
            unsigned int nprecisionCloseHits; /** number of close hits hits */
            unsigned int noutBoundsHits;      /** number of out of bounds hits */

            // some basic info
            bool isEndcap;
            bool isSmall;

            std::string dump() const;
        };

        struct CompactSummary {
            CompactSummary() :
                mainSector(-1),
                nprecisionLayers(0),
                nphiLayers(0),
                ntrigEtaLayers(0),
                nprecisionHoleLayers(0),
                ntrigEtaHoleLayers(0),
                nphiHoleLayers(0),
                nprecisionGoodLayers(0),
                isEndcap(false),
                isSmall(false) {}

            int mainSector;
            unsigned int nprecisionLayers;     /** number of precision layers */
            unsigned int nphiLayers;           /** number of phi layers */
            unsigned int ntrigEtaLayers;       /** number of eta trigger layers */
            unsigned int nprecisionHoleLayers; /** number of precision holes */
            unsigned int ntrigEtaHoleLayers;   /** number of eta trigger layer holes */
            unsigned int nphiHoleLayers;       /** number of phi layer holes */
            unsigned int nprecisionGoodLayers; /** number of precision layers not deweighted */
            bool isEndcap;                     /** true if non-deweighted chambers are in endcap */
            bool isSmall;                      /** true if non-deweighted chambers are small */

            std::set<int> sectors;
            std::map<MuonStationIndex::StIndex, HitSummary> stationLayers;
            std::set<MuonStationIndex::PhiIndex> phiLayers;

            std::string dump() const;
        };

    public:
        /** IAlgTool interface */
        static const InterfaceID& interfaceID();

        /** @brief Calculate compact summary
            @param track input track
            @return CompactSummary
        */
        virtual CompactSummary summary(const Trk::Track& track) const = 0;

        /** @brief Calculate compact summary
            @param summary TrackSummary of input track
            @return CompactSummary
        */
        virtual CompactSummary summary(const Trk::TrackSummary& summary) const = 0;

        /** @brief Calculate compact summary. Can fail if summary does not have a MuonTrackSummary.
            @param summary MuonTrackSummary of input track
            @return CompactSummary
        */
        virtual CompactSummary summary(const Trk::MuonTrackSummary& summary) const = 0;

        /** @brief Calculate compact summary
            @param segment input MuonSegment
            @return CompactSummary
        */
        virtual CompactSummary summary(const Muon::MuonSegment& segment) const = 0;

        /** @brief Calculate compact summary
            @param segments input vector of MuonSegments
            @return CompactSummary
        */
        virtual CompactSummary summary(const std::vector<const Muon::MuonSegment*>& segments) const = 0;

        /** @brief Calculate compact summary
            @param measurements input vector of MeasurementBase
            @return CompactSummary
        */
        virtual CompactSummary summary(const std::vector<const Trk::MeasurementBase*>& measurements) const = 0;
    };

    inline const InterfaceID& IMuonHitSummaryTool::interfaceID() { return IID_IMuonHitSummaryTool; }

    inline std::string IMuonHitSummaryTool::CompactSummary::dump() const {
        std::ostringstream sout;
        sout << " IMuonHitSummaryTool::CompactSummary: sectors " << sectors.size() << std::endl << "  sector indices: ";
        for (std::set<int>::const_iterator it = sectors.begin(); it != sectors.end(); ++it) sout << " " << *it;

        sout << std::endl << " StationLayers " << stationLayers.size();
        for (std::map<MuonStationIndex::StIndex, HitSummary>::const_iterator it = stationLayers.begin(); it != stationLayers.end(); ++it) {
            sout << std::endl << " " << MuonStationIndex::stName(it->first) << " " << it->second.dump();
        }
        sout << std::endl << " Phi layers " << phiLayers.size();
        for (std::set<MuonStationIndex::PhiIndex>::const_iterator it = phiLayers.begin(); it != phiLayers.end(); ++it)
            sout << " " << MuonStationIndex::phiName(*it);
        return sout.str();
    }

    inline std::string IMuonHitSummaryTool::HitSummary::dump() const {
        std::ostringstream sout;
        sout << " precHits " << nprecisionHits << " etaTrigLay " << netaTriggerLayers << " phiLay " << nphiLayers << " etaPhiLay "
             << netaPhiLayers << " precHoles " << nprecisionHoles << " etaTrigHoleLay " << netaTriggerHoleLayers << " phiHoleLay "
             << nphiHoleLayers << " isEndcap " << isEndcap << " isSmall " << isSmall;
        return sout.str();
    }

}  // namespace Muon

#endif  // IMuonHitSummaryTool_H

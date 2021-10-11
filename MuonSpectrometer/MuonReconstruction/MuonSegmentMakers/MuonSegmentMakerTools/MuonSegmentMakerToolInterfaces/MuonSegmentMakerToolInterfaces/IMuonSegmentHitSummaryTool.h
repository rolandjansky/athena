/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTHITSUMMARYTOOL_H
#define MUON_IMUONSEGMENTHITSUMMARYTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

namespace Muon {
    class MuonSegment;

    /** Interface for tools providing segment hit summary information*/
    class IMuonSegmentHitSummaryTool : virtual public IAlgTool {
    public:
        // struct to allow us to count the number of trigger hits per gas gap in both projections
        struct EtaPhiHitCount {
            EtaPhiHitCount() = default;
            int nphiHits{0};
            int netaHits{0};
        };
        typedef std::map<Identifier, EtaPhiHitCount> HitCountsPerLayer;  // map with hits per gas gap

        struct MdtLayerIntersect {
            MdtLayerIntersect() = default;
            int nhits{0};
            int nholes{0};
            double distFromTubeEnd{0.};
            double tubeLength{0.};
        };
        typedef std::map<int, MdtLayerIntersect> MdtLayerIntersectMap;

        struct HitCounts {
            HitCounts() = default;
            unsigned int nmdtHighADCHits{0};         // number of mdt hits with an ADC count above cut
            unsigned int nmdtHitsMl1{0};             // number of mdt hits in the first multilayer
            unsigned int nmdtHitsMl2{0};             // number of mdt hits in the second multilayer
            unsigned int nmdtHoles{0};               // number of mdt holes
            unsigned int nmdtHolesInChamber{0};      // number of mdt holes within the chamber bounds
            unsigned int nmdtEnclosedHoles{0};       // number of mdt holes enclosed within hits
            unsigned int ncscHitsPhi{0};             // number of csc phi hits
            unsigned int ncscHitsEta{0};             // number of csc eta hits
            unsigned int nphiTrigHitLayers{0};       // number of trigger hit layers with phi hits
            unsigned int netaTrigHitLayers{0};       // number of trigger hit layers with eta hits
            unsigned int npairedTrigHitLayers{0};    // number of trigger layers with hits in both projections
            unsigned int nexpectedTrigHitLayers{0};  // number of expected trigger layers
            bool closeToChamberEdge{false};
            double segmentQuality{0};  // segment quality number
            int adcMax{-999};          // maximum ADC on segment

            unsigned int nmdtHits() const { return nmdtHitsMl1 + nmdtHitsMl2; }
            unsigned int ncscHits() const { return ncscHitsPhi + ncscHitsEta; }

            double goodADCFraction() const { return (double)nmdtHighADCHits / (double)nmdtHits(); }

            // count number of hits and holes per layer
            MdtLayerIntersectMap mdtHitHolePerLayerCounts;

            // hit counts per layer
            HitCountsPerLayer hitCountsPerLayer;

            /** dump content to a string */
            std::string print() const;
        };

        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentHitSummaryTool("Muon::IMuonSegmentHitSummaryTool", 1, 0);

            return IID_IMuonSegmentHitSummaryTool;
        }

        /** @brief get hits counts for a segment
            @return provides pointer to fitter, ownsership not passed to client
        */
        virtual HitCounts getHitCounts(const MuonSegment& seg) const = 0;
    };

    inline std::string IMuonSegmentHitSummaryTool::HitCounts::print() const {
        std::ostringstream sout;
        sout << "HitCounts: ";
        if (nmdtHits() != 0) sout << " MDT ml1 " << std::setw(3) << nmdtHitsMl1 << "  ml2 " << std::setw(3) << nmdtHitsMl2;
        if (nmdtHoles != 0)
            sout << "  holes " << std::setw(3) << nmdtHoles << " in chamber " << std::setw(3) << nmdtHolesInChamber << "  enclosed "
                 << std::setw(3) << nmdtEnclosedHoles << " quality " << segmentQuality;
        if (ncscHits() != 0) sout << "  CSC eta " << std::setw(3) << ncscHitsEta << "  phi " << std::setw(3) << ncscHitsPhi;
        if (netaTrigHitLayers != 0) sout << "  Trig eta " << std::setw(3) << netaTrigHitLayers;
        if (nphiTrigHitLayers != 0) sout << "  Trig phi " << std::setw(3) << nphiTrigHitLayers;
        if (npairedTrigHitLayers != 0) sout << "  Trig pairs " << std::setw(3) << npairedTrigHitLayers;
        if (closeToChamberEdge) sout << " on edge";
        return sout.str();
    }

}  // namespace Muon

#endif  // IMuonSegmentHitSummaryTool

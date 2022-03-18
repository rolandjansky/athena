/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
            uint8_t nphiHits{0};
            uint8_t netaHits{0};
            uint8_t operator()() const {return nphiHits + netaHits; }  
            bool hasEtaAndPhi() const {return nphiHits > 0 && netaHits > 0; }          
        };

        using HitCountsPerLayer = std::map<Identifier, EtaPhiHitCount>;  // map with hits per gas gap

        struct MdtLayerIntersect {
            MdtLayerIntersect() = default;
            uint8_t nhits{0};
            uint8_t nholes{0};
            double distFromTubeEnd{0.};
            double tubeLength{0.};
        };
        using MdtLayerIntersectMap = std::map<int, MdtLayerIntersect>;

        struct HitCounts {
            HitCounts() = default;
            uint8_t nmdtHighADCHits{0};         // number of mdt hits with an ADC count above cut
            uint8_t nmdtHitsMl1{0};             // number of mdt hits in the first multilayer
            uint8_t nmdtHitsMl2{0};             // number of mdt hits in the second multilayer
            uint8_t nmdtHoles{0};               // number of mdt holes
            uint8_t nmdtHolesInChamber{0};      // number of mdt holes within the chamber bounds
            uint8_t nmdtEnclosedHoles{0};       // number of mdt holes enclosed within hits
            EtaPhiHitCount ncscHits{};               // Number of CSC hits
            EtaPhiHitCount nstgcHits{};              // Number of sTGC hits

            uint8_t nmmEtaHits{0};              /// Number of eta micromega hits
            uint8_t nmmStereoHits{0};           /// Number of eta stereo hits
           
            uint8_t nphiTrigHitLayers{0};       // number of trigger hit layers with phi hits
            uint8_t netaTrigHitLayers{0};       // number of trigger hit layers with eta hits
            uint8_t npairedTrigHitLayers{0};    // number of trigger layers with hits in both projections
            uint8_t nexpectedTrigHitLayers{0};  // number of expected trigger layers
            bool closeToChamberEdge{false};
            double segmentQuality{0};  // segment quality number
            int adcMax{-999};          // maximum ADC on segment

            /// Returns the number of hits in both MDT layers
            uint8_t nmdtHits() const { return nmdtHitsMl1 + nmdtHitsMl2; }
            /// Returns the number of hits in the micromegas
            uint8_t nmmHits() const {return nmmEtaHits + nmmStereoHits; }
            /// Returns the number of hits in the NSW
            uint8_t nnswHits() const {return nmmHits() + nstgcHits();}          
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
        if (nmdtHits()) sout << " MDT ml1 " << std::setw(3) <<(int) nmdtHitsMl1 << "  ml2 " << std::setw(3) <<(int) nmdtHitsMl2;
        if (nmdtHoles )
            sout << "  holes " << std::setw(3) << nmdtHoles << " in chamber " << std::setw(3) << nmdtHolesInChamber << "  enclosed "
                 << std::setw(3) << nmdtEnclosedHoles << " quality " << segmentQuality;
        if (ncscHits()) sout << "  CSC eta " << std::setw(3) << (int) ncscHits.netaHits << "  phi " << std::setw(3) << (int) ncscHits.nphiHits;
        if (nmmHits()) sout << "  Micromega eta " << std::setw(3) << (int) nmmEtaHits << "  stereo " << std::setw(3) << (int) nmmStereoHits;
        if (nstgcHits()) sout << "  sTGC eta " << std::setw(3) << (int) nstgcHits.netaHits << "  phi " << std::setw(3) << (int) nstgcHits.netaHits;
     
        if (netaTrigHitLayers != 0) sout << "  Trig eta " << std::setw(3) << (int)netaTrigHitLayers;
        if (nphiTrigHitLayers != 0) sout << "  Trig phi " << std::setw(3) << (int)nphiTrigHitLayers;
        if (npairedTrigHitLayers != 0) sout << "  Trig pairs " << std::setw(3) << (int)npairedTrigHitLayers;
        if (closeToChamberEdge) sout << " on edge";
        return sout.str();
    }

}  // namespace Muon

#endif  // IMuonSegmentHitSummaryTool

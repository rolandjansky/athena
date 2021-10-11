/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/MuonStationIndexHelpers.h"

namespace Muon {

    std::vector<MuonStationIndex::TechnologyIndex> MuonStationIndexHelpers::technologiesInStation(MuonStationIndex::StIndex stIndex) {
        static const std::vector<MuonStationIndex::TechnologyIndex> mmm_empty = {};
        static const std::vector<MuonStationIndex::TechnologyIndex> mmm_barrelInner = {MuonStationIndex::MDT};
        static const std::vector<MuonStationIndex::TechnologyIndex> mmm_barrelMiddleOuter = {MuonStationIndex::MDT, MuonStationIndex::RPC};
        static const std::vector<MuonStationIndex::TechnologyIndex> mmm_barrelExtended = {MuonStationIndex::MDT};
        static const std::vector<MuonStationIndex::TechnologyIndex> mmm_endcapInner = {
            MuonStationIndex::MDT, MuonStationIndex::CSCI, MuonStationIndex::TGC, MuonStationIndex::STGC, MuonStationIndex::MM};
        static const std::vector<MuonStationIndex::TechnologyIndex> mmm_endcapMiddle = {MuonStationIndex::MDT, MuonStationIndex::TGC};
        static const std::vector<MuonStationIndex::TechnologyIndex> mmm_endcapOuterExtended = {MuonStationIndex::MDT};
        static const std::vector<std::vector<MuonStationIndex::TechnologyIndex> > mmm_all_technologies = {
            mmm_barrelInner, mmm_barrelMiddleOuter, mmm_barrelMiddleOuter,   mmm_barrelExtended,
            mmm_endcapInner, mmm_endcapMiddle,      mmm_endcapOuterExtended, mmm_endcapOuterExtended};

        if (stIndex == MuonStationIndex::StUnknown || stIndex >= MuonStationIndex::StIndexMax) return mmm_empty;
        return mmm_all_technologies[stIndex];
    }

    bool MuonStationIndexHelpers::isSmall(int sector) { return sector % 2 == 0; }

}  // namespace Muon

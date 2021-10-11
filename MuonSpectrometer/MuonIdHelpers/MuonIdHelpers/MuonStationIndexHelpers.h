/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONIDHELPERS_MUONSTATIONINDEXHELPERS_H
#define MUONIDHELPERS_MUONSTATIONINDEXHELPERS_H

#include <vector>

#include "MuonStationIndex/MuonStationIndex.h"

namespace Muon {
    class MuonStationIndexHelpers {
    public:
        /** get techonolgies in */
        static std::vector<MuonStationIndex::TechnologyIndex> technologiesInStation(MuonStationIndex::StIndex stIndex);

        /** returns whether a sector is small */
        static bool isSmall(int sector);
    };
}  // namespace Muon

#endif

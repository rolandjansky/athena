/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigEFMissingET/PUSplitGrid.h"

namespace HLT
{
  namespace MET
  {
    PUSplitGrid::PUSplitGrid(
        double maxEta,
        std::size_t nEtaTowers,
        std::size_t nPhiTowers,
        bool displaceEta,
        bool displacePhi) : PUSplitGrid(GridParameters{maxEta, nEtaTowers, nPhiTowers, displaceEta, displacePhi})
    {
    }

    PUSplitGrid::PUSplitGrid(const GridParameters &parameters) : PufitMultiGrid<3>(parameters)
    {
    }

    PUSplitGrid::PUSplitGrid(const PUSplitGrid &other) : PufitMultiGrid<3>(other)
    {
    }
  } // namespace MET
} // namespace HLT
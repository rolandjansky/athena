/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/MdtMultiChamberGeometry.h"

#include <algorithm>
#include <iostream>
#include <iterator>

#include "TrkDriftCircleMath/SortDcsByY.h"

namespace TrkDriftCircleMath {

    MdtMultiChamberGeometry::MdtMultiChamberGeometry(const std::vector<MdtChamberGeometry>& chambers) : m_chambers(chambers) {
        m_validGeometry = std::find_if(m_chambers.begin(), m_chambers.end(),
                                       [](const MdtChamberGeometry& geo) { return !geo.validGeometry(); }) == m_chambers.end();        
    }

    MdtMultiChamberGeometry::~MdtMultiChamberGeometry() = default;
    DCVec MdtMultiChamberGeometry::tubesPassedByLine(const Line& line, int ml) const {
        DCVec crossedTubes;
        crossedTubes.reserve(60);
        if (!m_validGeometry) {
            std::cout << " >>>>> invalid geometry <<<<< " << std::endl;
            return crossedTubes;
        }

        for (const MdtChamberGeometry& it : m_chambers) { it.tubesPassedByLine(line, ml, crossedTubes); }

        std::stable_sort(crossedTubes.begin(), crossedTubes.end(), SortDcsByY());

        return crossedTubes;
    }

    void MdtMultiChamberGeometry::print(MsgStream& msg) const {
        for (const MdtChamberGeometry& it : m_chambers) { it.print(msg); }
    }

    unsigned int MdtMultiChamberGeometry::nlay() const { return m_chambers.empty() ? 0 : m_chambers[0].nlay(); }

    LocVec2D MdtMultiChamberGeometry::tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const {
        std::vector<MdtChamberGeometry>::const_iterator it = m_chambers.begin();
        if (it->validId(ml, lay, tube))
            return it->tubePosition(ml, lay, tube);
        else
            return it->tubePosition(1, 0, 0);
    }
}  // namespace TrkDriftCircleMath

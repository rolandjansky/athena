/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/MuonLayerHoughSelector.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>

namespace MuonHough {

    using valPair = std::pair<int, float>;

    MuonLayerHoughSelector::MuonLayerHoughSelector(std::vector<std::pair<int, float>> cutValues) {
        m_cutValues = std::move(cutValues);

        auto comp = [](const valPair& a, const valPair& b) -> bool { return a.first < b.first; };
        std::sort(m_cutValues.begin(), m_cutValues.end(), comp);

        if (getMinCutValue() < 0) std::cout << std::endl << "MuonLayerHoughSelector: Negative cuts found!!!" << std::endl;
    }

    MuonLayerHoughSelector::~MuonLayerHoughSelector() {}

    float MuonLayerHoughSelector::getCutValue(float position) const {
        const float pos = std::abs(position);
        for (const auto& cut : m_cutValues) {
            if (cut.first < pos) return cut.second;  // notice the array is already sorted
        }
        return m_cutValues.back().second;
    }

    float MuonLayerHoughSelector::getMinCutValue() const {
        if (m_cutValues.empty()) {
            std::cout << "MuonLayerHoughSelector: cutValues not available, returning invalid value";
            return -999;
        }
        return m_cutValues[0].second;
    }

    bool MuonLayerHoughSelector::passesCutValue(float testValue, float position) const { return testValue > getCutValue(position); }

}  // namespace MuonHough

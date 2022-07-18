/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLAYERHOUGH_MUONLAYERHOUGHSELECTOR_H
#define MUONLAYERHOUGH_MUONLAYERHOUGHSELECTOR_H

#include <vector>
#include <utility> //for std::pair

namespace MuonHough {
    class MuonLayerHoughSelector {
    public:
        /** Default constructor */
        MuonLayerHoughSelector() = default;
        MuonLayerHoughSelector(const MuonLayerHoughSelector &) = default;
        MuonLayerHoughSelector(MuonLayerHoughSelector &&) = default;
        MuonLayerHoughSelector & operator= (MuonLayerHoughSelector &&) = default;
        MuonLayerHoughSelector & operator= (const MuonLayerHoughSelector &) = default;
        MuonLayerHoughSelector(std::vector<std::pair<int, float>> cutValues);

        /** Destructor */
        virtual ~MuonLayerHoughSelector();

        /** Getter Methods */
        float getCutValue(float pos) const;
        float getMinCutValue() const;
        bool passesCutValue(float testValue, float position) const;

    private:
        std::vector<std::pair<int, float>> m_cutValues;
    };

}  // namespace MuonHough
#endif

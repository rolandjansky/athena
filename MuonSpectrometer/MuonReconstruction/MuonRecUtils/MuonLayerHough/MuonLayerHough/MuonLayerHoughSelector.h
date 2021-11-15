/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLAYERHOUGH_MUONLAYERHOUGHSELECTOR_H
#define MUONLAYERHOUGH_MUONLAYERHOUGHSELECTOR_H

#include <vector>

namespace MuonHough {
    class MuonLayerHoughSelector {
    public:
        /** Default constructor */
        MuonLayerHoughSelector() = default;
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

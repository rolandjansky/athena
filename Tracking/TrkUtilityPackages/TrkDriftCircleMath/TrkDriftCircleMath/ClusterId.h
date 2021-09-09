/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CLUSTERID_H
#define DCMATH_CLUSTERID_H

#include <ostream>

namespace TrkDriftCircleMath {

    class ClusterId {
    public:
        static constexpr int stationOffSet = 1000000;
        static constexpr int etaOffSet = 10000;
        static constexpr int phiOffSet = 100;
        ClusterId() = default;
        ClusterId(int stName, int eta, int phi, int barrel, int measuresPhi) {
            m_id = stationOffSet * stName + etaOffSet * eta + phiOffSet * phi + 10 * barrel + measuresPhi;
        }

        int id() const { return m_id; }
        int stName() const { return m_id / stationOffSet; }
        int eta() const { return (m_id % stationOffSet) / etaOffSet; }
        int phi() const { return (m_id % etaOffSet) / phiOffSet; }
        int isTgc() const { return (m_id % phiOffSet) / 10; }
        int measuresPhi() const { return m_id % 10; }

    private:
        int m_id{0};
    };

    std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::ClusterId& id);

}  // namespace TrkDriftCircleMath

#endif

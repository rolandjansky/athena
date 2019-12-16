/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEMUIDMONITORALGORITHM_H
#define TILEMONITORING_TILEMUIDMONITORALGORITHM_H

#include "TileMonitorAlgorithm.h"
#include "TileEvent/TileMuContainer.h"

#include "AthenaMonitoring/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

/** @class TileMuIdMonitorAlgorithm
 *  @brief Class for Tile MuId based monitoring
 */

class TileMuIdMonitorAlgorithm : public TileMonitorAlgorithm {

  public:

    using TileMonitorAlgorithm::TileMonitorAlgorithm;
    virtual ~TileMuIdMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    SG::ReadHandleKey<TileMuContainer> m_muContainerKey{this,
        "TileMuContainer", "TileMuObj", "Tile MuId container name"};

    std::vector<int> m_muIdTagsGroups;
    std::vector<int> m_muIdEnergyGroups;
    std::vector<int> m_muIdEtaGroups;
    std::vector<int> m_muIdPhiGroups;
    std::vector<int> m_muIdQualityGroups;
    std::vector<int> m_muIdEtaPhiGroups;
    std::vector<int> m_muIdEneEtaGroups;
    std::vector<int> m_muIdEnePhiGroups;
    std::vector<int> m_muIdEneQualityGroups;

    std::vector<int> m_muId1TagEnergyGroups;
    std::vector<int> m_muId1TagQualityGroups;
    std::vector<int> m_muId1TagEtaPhiGroups;
    std::vector<int> m_muId1TagEneEtaGroups;
    std::vector<int> m_muId1TagEnePhiGroups;
    std::vector<int> m_muId1TagEneQualityGroups;

    std::vector<int> m_muId2TagEnergyGroups;
    std::vector<int> m_muId2TagQualityGroups;
    std::vector<int> m_muId2TagEtaPhiGroups;
    std::vector<int> m_muId2TagEneEtaGroups;
    std::vector<int> m_muId2TagEnePhiGroups;
    std::vector<int> m_muId2TagEneQualityGroups;
};


#endif // TILEMONITORING_TILEMUIDMONITORALGORITHM_H

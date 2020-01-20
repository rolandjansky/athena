/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILETOWERMONITORALGORITHM_H
#define TILEMONITORING_TILETOWERMONITORALGORITHM_H

#include "TileMonitorAlgorithm.h"

#include "CaloEvent/CaloTowerContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileID;

/** @class TileTowerMonitorAlgorithm
 *  @brief Class for Tile Tower based monitoring
 */

class TileTowerMonitorAlgorithm : public TileMonitorAlgorithm {

  public:

    using TileMonitorAlgorithm::TileMonitorAlgorithm;
    virtual ~TileTowerMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    SG::ReadHandleKey<CaloTowerContainer> m_caloTowerContainerKey{this,
        "CaloCellContainer", "TileTower", "Calo tower container name"};


    std::vector<int> m_towerEtaPhiGroups;
    std::vector<int> m_towerEtaPhiDiffGroups;
    std::vector<std::vector<int>> m_towerEtGroups;
    std::vector<std::vector<int>> m_towerEnergyGroups;

    const TileID* m_tileID{nullptr};
};


#endif // TILEMONITORING_TILETOWERMONITORALGORITHM_H

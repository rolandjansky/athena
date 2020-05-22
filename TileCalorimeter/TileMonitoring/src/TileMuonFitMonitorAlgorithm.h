/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEMUONFITMONITORALGORITHM_H
#define TILEMONITORING_TILEMUONFITMONITORALGORITHM_H

#include "TileMonitorAlgorithm.h"

#include "TileEvent/TileCosmicMuonContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileID;

/** @class TileMuonFitMonitorAlgorithm
 *  @brief Class for Tile cosmic muon based monitoring
 */

class TileMuonFitMonitorAlgorithm : public TileMonitorAlgorithm {

  public:

    using TileMonitorAlgorithm::TileMonitorAlgorithm;
    virtual ~TileMuonFitMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    SG::ReadHandleKey<TileCosmicMuonContainer> m_cosmicMuonContainerKey{this,
        "TileCosmicMuonContainer", "TileCosmicMuonHT", "Tile cosmic muon container name"};

    std::vector<int> m_nMuGroups;
    std::vector<int> m_muEneGroups;
    std::vector<int> m_muTimeGroups;
    std::vector<int> m_muPathGroups;
    std::vector<int> m_muNCellsGroups;
    std::vector<int> m_muPositionGroups;
    std::vector<int> m_muPositionTimeGroups;
    std::vector<int> m_muEneDensityGroups;
    std::vector<int> m_muDirectionPhiGroups;
    std::vector<int> m_muDirectionThetaGroups;
    std::vector<int> m_muPosDirectionThetaGroups;
    std::vector<int> m_muPartitionTimeGroups;

    const TileID* m_tileID{nullptr};
};


#endif // TILEMONITORING_TILEMUONFITMONITORALGORITHM_H

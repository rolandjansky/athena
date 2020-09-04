/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILECELLMONITORALGORITHM_H
#define TILEMONITORING_TILECELLMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileBadChannels.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileMonitorAlgorithm.h"

#include "CaloEvent/CaloCellContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

class TileID;
class TileHWID;
class TileCablingService;


/** @class TileCellMonitorAlgorithm
 *  @brief Class for Tile Cell based monitoring
 */

class TileCellMonitorAlgorithm : public TileMonitorAlgorithm {

  public:

    using TileMonitorAlgorithm::TileMonitorAlgorithm;
    virtual ~TileCellMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    int getDigitizer(int channel) const;
    void fillMaskedInDB(const TileBadChannels* badChannels) const;
    void fillSynchronization(const std::vector<const CaloCell*>& cells, const std::vector<int>& l1TriggersIndices) const;

    Gaudi::Property<float> m_energyThreshold{this,
        "EnergyThreshold", 300.0F, "Energy threshold in MeV"};

    Gaudi::Property<float> m_negativeEnergyThreshold{this,
        "NegativeEnergyThreshold", -2000.0F, "Negative energy threshold in MeV"};

    Gaudi::Property<float> m_energyThresholdForTime{this,
        "EnergyThresholdForTime", 500.0F, "Energy threshold for timing in MeV"};

    Gaudi::Property<float> m_energyBalanceThreshold{this,
        "EnergyBalanceThreshold", 3.0F, "Energy ratio threshold"};

    Gaudi::Property<float> m_timeBalanceThreshold{this,
        "TimeBalanceThreshold", 25.0F, "Time threshold in ns"};

    Gaudi::Property<bool> m_fillTimeHistograms{this,
        "fillTimeHistograms", false, "Force filling timing histograms"};

    Gaudi::Property<bool> m_fillTimeAndEnergyDiffHistograms{this, "fillTimeAndEnergyDiffHistograms", true,
         "Fill histograms with time and energy difference between two PMTs of the same Cell"};

    Gaudi::Property<std::vector<float>> m_energyRangeForMuon{this,
         "EnergyRangeForMuon", {300.0F, 2000.F}, "Cell energy range for muon in MeV"};

    Gaudi::Property<std::vector<float>> m_timeRangeForMuon{this,
         "TimeRangeForMuon", {-60.0F, 60.F}, "Cell time range for muon in ns"};

    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "Tile cabling service"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

    /**
     * @brief Name of TileBadChannels in condition store
     */
    SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this,
        "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};

    SG::ReadHandleKey<CaloCellContainer> m_caloCellContainerKey{this,
        "CaloCellContainer", "AllCalo", "Calo cell container name"};

    const TileID* m_tileID{nullptr};
    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

    std::vector<int> m_energyBalModPartGroups;
    std::vector<int> m_timeBalModPartGroups;
    std::vector<int> m_cellSynchGroups;
    std::vector<int> m_maskedOnFlyGroups;
    std::vector<int> m_maskedDueDQGroups;
    std::vector<int> m_maskedCellsDueDQGroups;
    std::vector<int> m_maskedOnFlyLBGroups;
    std::vector<int> m_maskedCellsLBGroups;
    std::vector<std::vector<int>> m_maskedGroups;

    std::vector<std::vector<int>> m_energySampEGroups;
    std::vector<std::vector<int>> m_moduleCorrGroups;
    std::vector<std::vector<int>> m_chanTimeGroups;
    std::vector<std::vector<int>> m_digiTimeGroups;
    std::vector<std::vector<int>> m_nCellsGroups;
    std::vector<std::vector<int>> m_nCellsOverThrGroups;
    std::vector<std::vector<int>> m_detailOccupGroups;
    std::vector<std::vector<int>> m_overThrOccupGroups;
    std::vector<std::vector<int>> m_overThr30GeVOccupGroups;
    std::vector<std::vector<int>> m_overThr300GeVOccupGroups;
    std::vector<std::vector<std::vector<int>>> m_overThrOccupGainGroups;

    std::vector<std::vector<std::vector<int>>> m_eneDiffSampGroups;
    std::vector<std::vector<std::vector<int>>> m_timeDiffSampGroups;

    std::vector<std::vector<int>> m_eneEtaPhiGroups;
    std::vector<std::vector<int>> m_overThrEtaPhiGroups;

    std::vector<int> m_negOccupGroups;
    std::vector<int> m_timeBalGroups;
    std::vector<int> m_energyBalGroups;

};


#endif // TILEMONITORING_TILECELLMONITORALGORITHM_H

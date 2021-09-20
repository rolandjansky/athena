/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEMBTSMONITORALGORITHM_H
#define TILEMONITORING_TILEMBTSMONITORALGORITHM_H

#include "TileEvent/TileContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDQstatus.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigConfData/L1Menu.h"
#include "TrigT1Result/CTP_RDO.h"

class TileTBID;
class TileHWID;
class TileCablingService;

/** @class TileMBTSMonitorAlgorithm
 *  @brief Class for Tile MBTS based monitoring
 */

class TileMBTSMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    using AthMonitorAlgorithm::AthMonitorAlgorithm;

    virtual ~TileMBTSMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
    virtual StatusCode start() override;

  private:

    using Tile = TileCalibUtils;

    enum TileDigiErrors {GENERAL, GLOBAL_CRC, ROD_CRC, FRONTEND_CRC, BCID, HEADER_FORMAT,
                         HEADER_PARITY, SAMPLE_FORMAT, SAMPLE_PARITY, MEMORY_PARITY};

    void setDigiError(std::vector<int>& counters, std::vector<int>& errors, int counter, int error) const;

    int getMBTSCounter(Identifier mbtsID) const;

    Gaudi::Property<bool> m_fillHistogramsPerMBTS{this,
        "FillHistogramsPerMBTS", false, "Switch for using per MBTS histograms"};

    Gaudi::Property<bool> m_useTrigger{this,
        "useTrigger", false, "Switch for using trigger information"};

    Gaudi::Property<std::vector<float>> m_energyCut{this,
        "EnergyCuts",
         {//MBTSA00   MBTSA01   MBTSA02   MBTSA03   MBTSA04   MBTSA05   MBTSA06   MBTSA07
           60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222,
          //MBTSA08   MBTSA09   MBTSA10   MBTSA11   MBTSA12   MBTSA13   MBTSA14   MBTSA15
           60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222,
          //MBTSC00   MBTSC01   MBTSC02   MBTSC03   MBTSC04   MBTSC05   MBTSC06   MBTSC07
           60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222,
          //MBTSC08   MBTSC09   MBTSC10   MBTSC11   MBTSC12   MBTSC13   MBTSC14   MBTSC15
           60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222, 60. /222},
        "Energy cuts for 32 MBTS counters (MBTSA00, .., MBTSC15)"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status"};

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{ this,
        "TileDigitsContainer", "TileDigitsCnt", "Tile digits container" };

    SG::ReadHandleKey<TileCellContainer> m_mbtsCellContainerKey{this,
        "MBTSContainer", "MBTSContainer", "Tile MBTS container" };

    SG::ReadHandleKey<CTP_RDO> m_ctpRdoKey{this,
        "CTP_RDO", "CTP_RDO", "Trigger CTP RDO name" };

    Gaudi::Property<std::vector<int>> m_ctpID{this,
        "CTP_ID", {-1}, "Force CTP IDs for 32 MBTS counters (MBTSA00, .., MBTSC15)"};

    Gaudi::Property<std::vector<std::string>> m_l1Triggers{this,
        "L1_MBTS",
        {
         "L1_MBTSA0", "L1_MBTSA1", "L1_MBTSA2",  "L1_MBTSA3",  "L1_MBTSA4",  "L1_MBTSA5",  "L1_MBTSA6",  "L1_MBTSA7",
         "L1_MBTSA8", "L1_MBTSA9", "L1_MBTSA10", "L1_MBTSA11", "L1_MBTSA12", "L1_MBTSA13", "L1_MBTSA14", "L1_MBTSA15",
         "L1_MBTSC0", "L1_MBTSC1", "L1_MBTSC2",  "L1_MBTSC3",  "L1_MBTSC4",  "L1_MBTSC5",  "L1_MBTSC6",  "L1_MBTSC7",
         "L1_MBTSC8", "L1_MBTSC9", "L1_MBTSC10", "L1_MBTSC11", "L1_MBTSC12", "L1_MBTSC13", "L1_MBTSC14", "L1_MBTSC15",
        }, "Names of 32 L1 MBTS triggers (e.g.: L1_MBTSA0, .., L1_MBTSC15)"};

    SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey{this,
        "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service" };

    const TileCablingService* m_cabling{nullptr};

    std::vector<int> m_energyGroups;
    std::vector<int> m_energyLBGroups;
    std::vector<int> m_energyTrigGroups;
    std::vector<int> m_energyTrigLBGroups;

    std::vector<int> m_timeGroups;
    std::vector<int> m_timeTrigGroups;

    std::vector<int> m_hfnGroups;
    std::vector<int> m_pedestalGroups;

    std::vector<int> m_pulseGroups;
    std::vector<int> m_pulseTrigGroups;

    std::vector<int> m_effTrigGroups;

    std::vector<int> m_trigGroups;
    std::vector<int> m_trigInWinGroups;
    std::vector<int> m_trigSumGroups;
    std::vector<int> m_coinTrigGroups;
    std::vector<int> m_deltaBCIDSumGroups;
    std::vector<std::vector<int>> m_deltaBCIDGroups;

    const TileTBID* m_tileTBID{nullptr};
    const TileHWID* m_tileHWID{nullptr};

    int m_MBTSchannels[Tile::MAX_ROS - 3][Tile::MAX_DRAWER];
    int m_MBTScounters[Tile::MAX_ROS - 3][Tile::MAX_DRAWER];

    static const unsigned int MAX_MBTS_COUNTER{32};
    static const unsigned int MBTS_DMU{0};
};

#endif // TILEMONITORING_TILEMBTSMONITORALGORITHM_H

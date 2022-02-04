/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILETMDBMONITORALGORITHM_H
#define TILEMONITORING_TILETMDBMONITORALGORITHM_H

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/ITileCondToolTMDB.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TileTMDBMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include "math.h"

class TileHWID;
class TileCablingService;

/** @class TileTMDBMonitorAlgorithm
 *  @brief Class for Tile TMDB based monitoring
 */

class TileTMDBMonitorAlgorithm: public AthMonitorAlgorithm {

  public:

    using  AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileTMDBMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;


    void fillNoiseHistograms(const TileDigitsCollection* muRcvDigitsCollection, const int drawer, const int partition) const;

  private:

    Gaudi::Property<std::vector<float>> m_pulseEnergyRange{this,
        "PulseEnergyRange", {0.5F, 5000.F}, "Energy [MeV] range for pulse shape"};

    Gaudi::Property<std::vector<float>> m_energyRange{this,
        "EnergyRange", {100.0F, 10000.F}, "Energy [MeV] range for pulse shape"};

    Gaudi::Property<bool> m_fillDetailedHistograms{this,
        "fillDetailedHistograms", false, "Fill monitoring histograms per TMDB channel"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "TileRawChannelCnt", "Input Tile raw channel container key"};

    SG::ReadHandleKey<TileRawChannelContainer> m_muRcvRawChannelContainerKey{this,
        "MuonReceiverRawChannelContainer", "MuRcvRawChCnt", "Input Tile muon receiver raw channel container key"};

    SG::ReadHandleKey<TileDigitsContainer> m_muRcvDigitsContainerKey{this,
        "MuonReceiverDigitsContainer", "MuRcvDigitsCnt", "Input Tile muon receiver digits container key"};

    ToolHandle<ITileCondToolTMDB> m_tileCondToolTMDB{this,
        "TileCondToolTMDB", "TileCondToolTMDB", "Tile TMDB conditions tool"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

    std::vector<int> m_peakGroups;
    std::vector<int> m_energyGroups;

    std::vector<std::vector<std::vector<std::vector<float>>>> m_PedestalVector;

    std::vector<std::vector<std::vector<int>>> m_chanEnergyGroups;
    std::vector<std::vector<std::vector<std::vector<int>>>> m_chanChannelNoiseGroups;
    std::vector<std::vector<std::vector<int>>> m_chanPeakPosGroups;
    std::vector<std::vector<std::vector<int>>> m_calibErrorGroups;
    std::vector<std::vector<std::vector<int>>> m_pulseGroups;

    struct corrmap {
        double **accCorrelation;
        double *accMean;
    };

    std::vector<std::vector<std::vector<float *>>> m_accCorrelation;

    using Tile = TileCalibUtils;

    // Mapping from Tile raw channels to TMDB in LB for odd modules,
    // in even modules TMDB channels are shifted by 1
    const std::map<int, int> m_cellTMDB_LB{{0, 0 /*D0*/},
                                           {13, 1 /*D1L*/}, {14, 2 /*D1R*/},
                                           {24, 3 /*D2R*/}, {25, 4 /*D2L*/},
                                           {41, 5 /*D3L*/}, {44, 6 /*D3R*/}};

    // Mapping from Tile raw channels to TMDB in EB
    const std::map<int, int> m_cellTMDB_EB{{16, 0 /*D5R*/}, {17, 1 /*D5L*/},
                                           {37, 2 /*D6L*/}, {38, 3 /*D6R*/}};

    std::reference_wrapper<const std::map<int, int>> m_cellTMDB[Tile::MAX_ROS - 1] = {m_cellTMDB_LB,
                                                                                      m_cellTMDB_LB,
                                                                                      m_cellTMDB_EB,
                                                                                      m_cellTMDB_EB};
};


#endif // TILEMONITORING_TILETMDBMONITORALGORITHM_H

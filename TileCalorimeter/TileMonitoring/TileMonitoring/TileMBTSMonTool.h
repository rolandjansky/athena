/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileMBTSMonTool.h
// PACKAGE:  TileMonitoring  
//
// AUTHORS:   Base Structure - Luca Fiorini (Luca.Fiorini@cern.ch) 
//            Specific Alg - Brian Martin (Brian.Thomas.Martin@cern.ch)
//
// ********************************************************************
#ifndef TILEMBTSMONTOOL_H
#define TILEMBTSMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "StoreGate/ReadHandleKey.h"

class CTP_RDO;
class CTP_RIO;
class TileCell;
class TileTBID;

namespace TrigConf {
  class ILVL1ConfigSvc;
}

/** @class TileMBTSMonTool
 *  @brief Class for Tile MBTS based monitoring
 */

class TileMBTSMonTool: public TileFatherMonTool {
  public:
    TileMBTSMonTool(const std::string & type, const std::string & name, const IInterface* parent);
    virtual ~TileMBTSMonTool();

    virtual StatusCode initialize() override;
    virtual StatusCode checkHists(bool fromFinalize) override;
    virtual StatusCode bookHistograms() override;
    virtual StatusCode procHistograms() override;
    virtual StatusCode fillHistograms() override;

    void setBinLabels(TH1*, int);
    void setBinLabels(TH2*, int, bool);

  private:
    // Digit level histograms
    TH1F* m_h_pedestal[32];
    TH1F* m_h_hfNoise[32];
    //TH2F* m_h_sumPed;
    //TH2F* m_h_sumHFNoise;
    //TH2F* m_h_sumLFNoise;
    TProfile* m_h_pulseAvgTrig[32];
    TProfile* m_h_pulseAvgNoTrig[32];

    // Cell level histograms
    TH1F* m_h_energy[32];
    TProfile* m_h_energy_lb[32];
    TH1F* m_h_time[32];
    TH1F* m_h_energy_wTBP[32];
    TH1F* m_h_time_wTBP[32];
    TH1F* m_h_occupancy;
    TProfile* m_h_efficiency[32];
    TProfile* m_h_sumEnergy;
    TProfile* m_h_sumEnergy_wTBP;
    TProfile* m_h_sumTime;
    TH1F* m_h_timeA;
    TH1F* m_h_timeC;
    TH1F* m_h_timeDiff;
    TH2F* m_h_timeDiffLumi;
    TH2S* m_h_coinEnergyHits;
    //TH2F* m_h_corrEnergy;
    //TH2F* m_h_corrEnergyTot;
    TProfile* m_h_bcidEnergyA;
    TProfile* m_h_bcidEnergyC;

    // Trigger histograms
    TH1S* m_h_ctpPIT;
    TH1S* m_h_ctpTBP;
    TH1S* m_h_ctpTAP;
    TH1S* m_h_ctpTAV;
    TH1S* m_h_ctpPITwin;
    TH1S* m_h_ctpTBPwin;
    TH1S* m_h_ctpTAPwin;
    TH1S* m_h_ctpTAVwin;
    TH1S* m_h_sumPIT;
    TH1S* m_h_sumTBP;
    TH1S* m_h_sumTAP;
    TH1S* m_h_sumTAV;
    TH1S* m_h_bcidPIT[32];
    TH1S* m_h_bcidTBP[32];
    TH1S* m_h_bcidTAP[32];
    TH1S* m_h_bcidTAV[32];
    TProfile* m_h_bcidPITsum;
    TProfile* m_h_bcidTBPsum;
    TProfile* m_h_bcidTAPsum;
    TProfile* m_h_bcidTAVsum;
    TH2S* m_h_multiMapPITA;
    TH2S* m_h_multiMapPITC;
    TH2S* m_h_multiPerSide;
    TH2S* m_h_coinPIT;
    TH2S* m_h_coinTBP;
    TH2S* m_h_coinTAP;
    TH2S* m_h_coinTAV;
    TProfile* m_h_bcidTriggerA;
    TProfile* m_h_bcidTriggerC;

    // Readout Error histograms
    TH2S* m_h_tileRdOutErr;

    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1ConfigSvc;

    // container names
    SG::ReadHandleKey<TileDigitsContainer> m_TileDigitsContainerID
    { this, "TileDigitsContainerName", "TileDigitsCnt", "" };
    SG::ReadHandleKey<TileCellContainer>   m_MBTSCellContainerID
    { this, "MBTSContainerName", "MBTSContainer", "" };

    int m_numEvents; // event counter
    bool m_isOnline;
    bool m_readTrigger;
    // Quantities for counter correlation calc
    double m_energy_ij[32][32];
    double m_energy_sum[32];
    double m_energy_sq[32];
    // Quantities for area correlation calc
    double m_energy_ij_tot[6][6];
    double m_energy_sum_tot[6];
    double m_energy_sq_tot[6];

    bool m_hasEnergyHit[32]; // Stores if counter recorded hit in cell info
    bool m_hasPIT[32]; // Stores if counter fired PIT
    bool m_hasTBP[32]; // Stores if counter fired TBP
    bool m_hasTAP[32]; // Stores if counter fired TAP
    bool m_hasTAV[32]; // Stores if counter fired TAV

    static const int m_tileDrawer[32]; // stores which tile drawers have MBTS
    static const int m_isInner[32]; // stores which MBTS are on the innner ring
    std::vector<std::string> m_counterNames;  // array of module names
    std::vector<std::string> m_counterLabel; // array of counter names
    std::vector<int> m_pitID; // stores MBTS pit IDs
    std::vector<int> m_ctpID; // stores MBTS ctp IDs
    unsigned int m_pitMultBitsA[3];
    unsigned int m_pitMultBitsC[3];

    double m_energyCut[32];  // Controls entries in m_hasHit
    std::string m_CTPcabling;  // Controls CTP mapping to use

    int m_MBTSchannels[2][64];
    int m_MBTScounters[2][64];
    std::vector<bool> m_counterExist;

    bool m_useTrigger;
    bool m_fillHistogramsPerMBTS;

    int32_t m_old_lumiblock;
    int m_nLumiblocks;
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey;
};

#endif

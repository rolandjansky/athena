/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigiNoiseMonTool.h
// PACKAGE:  
//
// AUTHOR:   Christophe Clement
//
//
// ********************************************************************
#ifndef TILEDIGINOISEMONTOOL_H
#define TILEDIGINOISEMONTOOL_H

#include "TileFatherMonTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileConditions/ITileDCSTool.h"
#include "StoreGate/ReadHandleKey.h"

class ITileBadChanTool;
class TileDQstatus;
class TileCondToolNoiseSample;

/** @class TileDigitsNoiseMonTool
 *  @brief Class for TileCal noise monitoring at digits level
 */

class TileDigiNoiseMonTool : public TileFatherMonTool {

  public:

    TileDigiNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    virtual ~TileDigiNoiseMonTool() override;

    virtual StatusCode initialize() override;

    //pure virtual methods
    virtual StatusCode bookHistograms() override;
    virtual StatusCode fillHistograms() override;
    virtual StatusCode procHistograms() override;

  private:

    StatusCode bookNoiseHistograms();
    StatusCode updateSummaryHistograms();

    bool isChanDCSgood (int ros, int drawer, int channel) const;

    std::string m_digitsContainerName;

    ToolHandle<ITileBadChanTool> m_tileBadChanTool;
    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provided noise values

    const TileDQstatus* m_DQstatus{};

    bool m_bigain{};

    //int m_nSamples;
    double m_sumPed1[5][64][48][2] = {{{{0}}}};
    double m_sumPed2[5][64][48][2]= {{{{0}}}};
    double m_sumRms1[5][64][48][2]= {{{{0}}}};

    int m_nPedEvents[5][64][48][2]= {{{{0}}}};
    int m_nRmsEvents[5][64][48][2]= {{{{0}}}};

    // histogram to store 4 maps of module vs channel
    TH2F* m_finalNoiseMap[5][2][3]= {{{0}}};

    int m_summaryUpdateFrequency{};
    int m_nEventsProcessed{};
    bool m_fillEmtyFromDB{};
    bool m_fillPedestalDifference{};
    std::vector<uint32_t> m_triggerTypes;
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{};
    ToolHandle<ITileDCSTool> m_tileDCS{this, "TileDCSTool", "TileDCSTool", "Tile DCS tool"};
    bool m_checkDCS{};
    bool m_histogramsNotBooked{};
};

#endif

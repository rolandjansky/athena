/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTBMonTool.h
// PACKAGE:  
//
// AUTHOR:   
//
//
// ********************************************************************
#ifndef TILEMONITORING_TILETBMONTOOL_H
#define TILEMONITORING_TILETBMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class TileDQstatus;

#include <vector>
#include <string>

/** @class TileTBMonTool
 *  @brief Class for TileCal TB monitoring
 */

class TileTBMonTool : public TileFatherMonTool {

  public:

    TileTBMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileTBMonTool();

    virtual StatusCode initialize();

    //pure virtual methods
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:

    void fillHitMap(int side, int section, int module, int tower, int sample, double energy);
    void initFirstEvent(void);

    std::string m_cellContainer; 


    TH1F* m_tileTotalEventEnergy;

    TProfile2D* m_tileTBHitMapLBC01;
    TProfile2D* m_tileTBHitMapEBC02;
    TH2F* m_tileEventEnergyVsCellsNumber;

    bool m_isFirstEvent;
    unsigned char m_maskedChannels[276][48]; // max drawerIdx = 276 and max channel = 48
    std::vector<std::string> m_masked;

    double m_maxTotalEnergy;
    double m_cellEnergyThreshold;
};

#endif

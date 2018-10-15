/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileRawChannelToTTL1.h
// Author   : Pedro Amaral, based on TileHitToTTL1
// Created  : Feb 2005
//
// DESCRIPTION
// 
// Creates TileTTL1 Digits from TileRawChannel
//
// Properties (JobOption Parameters):
//
//    TileRawChannelContainer          string   Name of container with TileRawChannel to read
//    TileTTL1Container         string   Name of container with TileTTL1Digits to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILERECALGS_TILERAWCHANNELTOTTL1_H
#define TILERECALGS_TILERAWCHANNELTOTTL1_H

// Tile includes
//#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileTTL1Container.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"


class TileID;
class TileHWID;
class TileInfo;
class CaloLVL1_ID;
class TileCablingService;
class ITileBadChanTool;
class TileCondToolEmscale;
class TileRawChannelContainer;

#include <string>

class TileRawChannelToTTL1: public AthAlgorithm {
  public:
    // Constructor
    TileRawChannelToTTL1(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileRawChannelToTTL1();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,"TileRawChannelContainer",
                                                                        "TileRawChannelCnt", 
                                                                        "Input Tile raw channel container key"};

    SG::WriteHandleKey<TileTTL1Container> m_ttl1ContainerKey{this,"TileTTL1Container",
                                                             "TileTTL1Container","Output Tile TTL1 container key"};

    std::string m_infoName;        // name of TileInfo object in TES
    bool m_constantTTL1shape; // true by default, so only gets ttl1 shape once at initialization, with phase=0 for all towers.

    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
    const TileInfo* m_tileInfo;
    const CaloLVL1_ID* m_TT_ID;

    std::vector<double> m_TTL1Shape;
    double m_phase;
    int m_nSamp;
    int m_iTrig;

    bool m_maskBadChannels;      //!< if true=> bad channels are masked

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
};

#endif // TILERECALGS_TILERAWCHANNELTOTTL1_H

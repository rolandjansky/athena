/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TilePulseForTileMuonReceiver.h
// Author   : Joao Gentil Mendes Saraiva (jmendes@cern.ch)
// Created  : October 2013
//
// DESCRIPTION
// 
// Creates TileMuonReceiver digits and raw channel from TileHit
//
// Properties (JobOption Parameters):
//
//    TileHitContainer                "TileHitCnt" 
//    MuonReceiverDigitsContainer     "MuRcvDigitsCnt" 
//    MuonReceiverRawChannelContainer "MuRcvRawChCnt" 
//    TileInfoName                    "TileInfo" 
//    IntegerDigits                   "Round digits (default=false)"                 
//    MaskBadChannels                 "Remove channels tagged bad (default=false)" 
//    UseCoolPulseShapes              "Pulse shapes from database (default=false)" 
//    UseCoolNoise                    "Noise from database (default=false)"
//    UseCoolPedestal                 "Pedestal from database (default=false)"
//    RndmSvc                         "Random Number Service used in TilePulseForTileMuonReceiver" 
//    TileRawChannelBuilderMF         "The tool by default is the Matched Filter"
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILEPULSEFORTILEMUONRECEIVER_H
#define TILESIMALGS_TILEPULSEFORTILEMUONRECEIVER_H

// Tile includes
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileRecUtils/TileRawChannelBuilderMF.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class IAthRNGSvc;
class PileUpMergeSvc;
class HWIdentifier;

class TileID;
class TileHWID;
class TileInfo;
class TileCablingService;
class TileEvent;
class TileDQstatus;

#include <string>
#include <vector>

class TilePulseForTileMuonReceiver: public AthAlgorithm {
  public:
    // constructor
    TilePulseForTileMuonReceiver(std::string name, ISvcLocator* pSvcLocator);
    // destructor
    virtual ~TilePulseForTileMuonReceiver();

    //Gaudi Hooks
    virtual StatusCode initialize() override; //!< initialize method
    virtual StatusCode execute() override;    //!< execute method
    virtual StatusCode finalize() override;   //!< finalize method

  private:

    SG::ReadHandleKey<TileHitContainer> m_hitContainerKey{this,
        "TileHitContainer","TileHitCnt", "Input Tile hit container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_muRcvDigitsContainerKey{this,
        "MuonReceiverDigitsContainer", "MuRcvDigitsCnt", "Output Tile muon receiver digits container key"};

    SG::WriteHandleKey<TileRawChannelContainer> m_muRcvRawChannelContainerKey{this,
        "MuonReceiverRawChannelContainer", "MuRcvRawChCnt", "Output Tile muon receiver raw channel container key"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""}; //!< Random number service to use
    /// Random Stream Name
    Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "Tile_PulseForTileMuonReceiver", ""};

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

    ToolHandle<TileCondToolPulseShape> m_tileToolPulseShape{this,
        "TileCondToolPulseShape", "TileCondToolPulseShape", "Tile pulse shape tool"};

    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};

    ToolHandle<TileRawChannelBuilderMF> m_MuRcvBuildTool{this,
        "TileRawChannelBuilderMF", "TileRawChannelBuilderMF", "Reconstruction tool, default: the Matched Filter"};

    Gaudi::Property<std::string> m_infoName{this, "TileInfoName", "TileInfo", "TileInfo object name"};
    Gaudi::Property<bool> m_integerDigits{this, "IntegerDigits", false, "Round digits (default=false)"};
    Gaudi::Property<bool> m_maskBadChannels{this, "MaskBadChannels", false, "Remove channels tagged bad (default=false)"};
    Gaudi::Property<bool> m_useCoolPulseShapes{this, "UseCoolPulseShapes", false, "Pulse shapes from database (default=false)"};
    Gaudi::Property<bool> m_tileNoise{this, "UseCoolNoise", false, "Noise from database (default=false)"};
    Gaudi::Property<bool> m_tilePedestal{this, "UseCoolPedestal", false, "Pedestal from database (default=false)"};

    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
    const TileInfo* m_tileInfo;
    const TileCablingService* m_cablingService; //!< TileCabling instance

    int m_nSamples;          //!< Number of time slices for each channel
    int m_iTrig;          //!< Index of the triggering time slice
    int m_adcMax;         //!< ADC saturation value
    double m_tileThresh;  //!< Actual threshold value

    // This is used when loading a pulse from db
    //
    int m_nShape;       //!< Number of bins in pulse shape
    int m_nBinsPerX;    //!< Number of bins per bunch crossing in pulse shape
    int m_binTime0;     //!< Index of time=0 bin for pulse shape
    double m_timeStep;  //!< Time step in pulse shape: 25.0 / nBinsPerX

    // vector container for the pulse shape
    //
    std::vector<double> m_shapeMuonReceiver;//!< Muon receiver pulse shape

    
    bool m_run2;
};

#endif

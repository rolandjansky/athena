/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RandomEngine.h"

class IAtRndmGenSvc;
class PileUpMergeSvc;
class HWIdentifier;

class TileID;
class TileHWID;
class TileInfo;
class TileCablingService;
class TileEvent;
class TileDQstatus;
class TileRawChannelBuilderMF;

#include <string>
#include <vector>

class TilePulseForTileMuonReceiver: public AthAlgorithm {
  public:
    // constructor
    TilePulseForTileMuonReceiver(std::string name, ISvcLocator* pSvcLocator);
    // destructor
    virtual ~TilePulseForTileMuonReceiver();
    //Gaudi Hooks
    StatusCode initialize(); //!< initialize method
    StatusCode execute();    //!< execute method
    StatusCode finalize();   //!< finalize method

  private:
    std::string m_hitContainer;         //!< Name of the tile hit container
    std::string m_MuRcvDigitsContainer; //!< Name of the algorithm digits container
    std::string m_MuRcvRawChContainer;  //!< Name of the algorithm raw channel container

    SG::ReadHandleKey<TileHitContainer> m_hitContainerKey{this,"TileHitContainer","TileHitCnt",
                                                          "input Tile hit container key"};


    SG::WriteHandleKey<TileDigitsContainer> m_muRcvDigitsContainerKey{this,"MuonReceiverDigitsContainer",
                                                                      "MuRcvDigitsCnt",
                                                                      "Output Tile muon receiver digits container key"};

    SG::WriteHandleKey<TileRawChannelContainer> m_muRcvRawChannelContainerKey{this,"MuonReceiverRawChannelContainer",
                                                                              "MuRcvRawChCnt",
                                                                              "Output Tile muon receiver raw channel container key"};


    std::string m_infoName;             //!< Name of tile info object in TES
    bool m_integerDigits;               //!< If true => round digits to integer
    bool m_useCoolPulseShapes;          //!< If true => use of pulse shapes from db
    bool m_maskBadChannels;             //!< If true => mask bad channels
    bool m_tilePedestal;                //!< If true => generate pedestal (amp) in digits from  db
    bool m_tileNoise;                   //!< If true => generate noise (rms) in digits from  db

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

    CLHEP::HepRandomEngine* m_pHRengine;    //!< Random number generator engine to use
    ServiceHandle<IAtRndmGenSvc> m_rndmSvc; //!< Random number service to use

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

    ToolHandle<TileCondToolPulseShape> m_tileToolPulseShape{this,
        "TileCondToolPulseShape", "TileCondToolPulseShape", "Tile pulse shape tool"};

    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};
    ToolHandle<TileRawChannelBuilderMF> m_MuRcvBuildTool;      //!< tool to set up the reconstruction algorithm
    
    bool m_run2;
};

#endif

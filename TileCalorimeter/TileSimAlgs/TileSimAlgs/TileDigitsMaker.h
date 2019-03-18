/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileDigitsMaker.h
// Author   : UC-ATLAS TileCal group
// Created  : Feb 2006 from TileHitToDigits algorithm
//
// DESCRIPTION
// 
// Creates TileDigits from TileHit
//
// Properties (JobOption Parameters):
//
//    TileHitContainer          string   Name of container with TileHit to read
//    TileDigitsContainer       string   Name of container with TileDigits to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILEDIGITSMAKER_H
#define TILESIMALGS_TILEDIGITSMAKER_H

// Tile includes
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileRecUtils/ITileDQstatusTool.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gauid includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RandomEngine.h"
#include "TileEvent/TileHitContainer.h"


class IAthRNGSvc;
class PileUpMergeSvc;
class TileID;
class TileTBID;
class TileHWID;
class TileInfo;
class HWIdentifier;
class TileCablingService;
class TileDQstatus;

#include <string>
#include <vector>

/** 
 @class TileDigitsMaker
 @brief This algorithm performs digitization in TileCal, obtaining TileDigits from TileHits.

 Hits from the PMTs in the cell are merged and calibration constants applied to obtained cell energy, time and quality. Starting from hits, pulse shapes are applied and the resulting pulses are digitized. Depending on the options selected, electronic noise and photoelectron statistical fluctuations are simulated in this procedure.
 */
class TileDigitsMaker: public AthAlgorithm {
  public:
    // Constructor
    TileDigitsMaker(std::string name, ISvcLocator* pSvcLocator);


    //Destructor 
    virtual ~TileDigitsMaker();

    //Gaudi Hooks
    StatusCode initialize(); //!< initialize method
    StatusCode execute();    //!< execute method
    StatusCode finalize();   //!< finalize method

  private:
    StatusCode FillDigitCollection(TileHitContainer::const_iterator hitContItr, std::vector<double *> &drawerBufferLo, std::vector<double *> &drawerBufferHi, int igain[], int overgain[], double ech_int[], std::vector<bool> &signal_in_channel) const;

    SG::ReadHandleKey<TileHitContainer> m_hitContainerKey{this,"TileHitContainer","TileHitCnt",
                                                          "input Tile hit container key"};

    SG::ReadHandleKey<TileHitContainer> m_hitContainer_DigiHSTruthKey{this,"TileHitContainer_DigiHSTruth","TileHitCnt_DigiHSTruth",
                                                          "input Tile hit container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_digitsContainerKey{this,"TileDigitsContainer",
                                                                 "TileDigitsCnt",
                                                                 "Output Tile digits container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_digitsContainer_DigiHSTruthKey{this,"TileDigitsContainer_DigiHSTruth",
                                                                 "TileDigitsCnt_DigiHSTruth",
                                                                 "Output DigiHSTruth Tile digits container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_filteredDigitsContainerKey{this,"TileFilteredContainer",
                                                                         "TileDigitsFlt",
                                                                         "Output filtered Tile digits container key"};

    std::string m_infoName;        //!< Name of TileInfo object in TES
    double m_filterThreshold;      //!< theshold on hit energy to store digit in filtered container
    double m_filterThresholdMBTS; //!< theshold on MBTS hit energy to store digit in filtered container
    bool m_integerDigits;          //!< If true -> round digits to integer
    bool m_calibRun;               //!< If true -> both high and low gain saved
    bool m_rndmEvtOverlay;       //!< If true -> overlay with random event (zero-luminosity pile-up)
    bool m_useCoolPulseShapes;
    bool m_maskBadChannels;
    bool m_doDigiTruth;

    PileUpMergeSvc* m_mergeSvc; //!< Pointer to PileUpMergeService

    const TileID* m_tileID;
    const TileTBID* m_tileTBID;
    const TileHWID* m_tileHWID;
    const TileInfo* m_tileInfo;
    const TileCablingService* m_cabling; //!< TileCabling instance
    const TileDQstatus* m_DQstatus;

    std::vector<HWIdentifier *> m_all_ids;
    std::vector<double *> m_drawerBufferHi; //!< Vector used to store pointers to digits for a single drawer (high gain)
    std::vector<double *> m_drawerBufferLo; //!< Vector used to store pointers to digits for a single drawer (low gain)
    std::vector<double *> m_drawerBufferHi_DigiHSTruth; //!< Vector used to store pointers to digits for a single drawer (high gain)
    std::vector<double *> m_drawerBufferLo_DigiHSTruth; //!< Vector used to store pointers to digits for a single drawer (low gain)

    int m_nSamples;           //!< Number of time slices for each channel
    int m_iTrig;           //!< Index of the triggering time slice
    float m_adcMax;        //!< ADC saturation value 
    float m_adcMaxHG;      //!< ADC saturation value - 0.5 
    bool m_tileNoise;      //!< If true => generate noise in TileDigits
    bool m_tileCoherNoise; //!< If true => generate coherent noise in TileDigits
    bool m_tileThresh;     //!< If true => apply threshold to Digits
    double m_tileThreshHi; //!< Actual threshold value for high gain
    double m_tileThreshLo; //!< Actual threshold value for low gain
    int m_allChannels;     //!< If set to 1 => always create all channels in output container
                           //!< If set to 2 => always add noise to all missing channels in overlay

    std::vector<double> m_digitShapeHi; //!< High gain pulse shape
    int m_nShapeHi;                     //!< Number of bins in high gain pulse shape 
    int m_nBinsPerXHi;                //!< Number of bins per bunch crossing in high gain pulse shape
    int m_binTime0Hi;                   //!< Index of time=0 bin for high gain pulse shape 
    double m_timeStepHi;                //!< Time step in high gain pulse shape: 25.0 / nBinsPerXHi

    std::vector<double> m_digitShapeLo; //!< Low gain pulse shape
    int m_nShapeLo;                     //!< Number of bins in low gain pulse shape 
    int m_nBinsPerXLo;                  //!< Number of bins per bunch crossing in low gain pulse shape
    int m_binTime0Lo;                   //!< Index of time=0 bin for low gain pulse shape
    double m_timeStepLo;                //!< Time step in low gain pulse shape: 25.0 / nBinsPerXLo

    ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service to use

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

    ToolHandle<TileCondToolPulseShape> m_tileToolPulseShape{this,
        "TileCondToolPulseShape", "TileCondToolPulseShape", "Tile pulse shape tool"};

    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};

    ToolHandle<ITileDQstatusTool> m_DQstatusTool { this,
        "TileDQstatusTool", "TileDQstatusTool", "Tool to create TileDQstatus" };

    SG::WriteHandleKey<TileDQstatus> m_DQstatusKey {this,
        "TileDQstatus", "TileDQstatus", "Output TileDQstatus key" };
};

#endif // TILESIMALGS_TILEDIGITSMAKER_H

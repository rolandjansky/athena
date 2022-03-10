/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "TileEvent/TileDQstatus.h"
#include "TileConditions/TilePulse.h"
#include "TileConditions/TileSampleNoise.h"
#include "TileConditions/TileEMScale.h"
#include "TileConditions/TileBadChannels.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileSamplingFraction.h"

// Atlas includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/Units.h"

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
#include <memory>

/** 
 @class TileDigitsMaker
 @brief This algorithm performs digitization in TileCal, obtaining TileDigits from TileHits.

 Hits from the PMTs in the cell are merged and calibration constants applied to obtained cell energy, time and quality.
 Starting from hits, pulse shapes are applied and the resulting pulses are digitized.
 Depending on the options selected, electronic noise and photoelectron statistical fluctuations are simulated in this procedure.
 */
class TileDigitsMaker: public AthReentrantAlgorithm {
  public:
    // Constructor
    using AthReentrantAlgorithm::AthReentrantAlgorithm;

    //Destructor
    virtual ~TileDigitsMaker() = default;

    //Gaudi Hooks
    virtual StatusCode initialize() override; //!< initialize method
    virtual StatusCode execute(const EventContext &ctx) const override;    //!< execute method
    virtual StatusCode finalize() override;   //!< finalize method

  private:
    StatusCode overlayBackgroundDigits(const TileDigitsCollection* bkgDigitCollection,
                                       const TileHitCollection* hitCollection,
                                       std::vector<std::vector<double>>& drawerBufferLo,
                                       std::vector<std::vector<double>>& drawerBufferHi,
                                       std::vector<int>& igain, int ros, int drawer, int drawerIdx,
                                       std::vector<int>& over_gain, const TileEMScale* emScale,
                                       const TileSampleNoise& sampleNoise, const TileDQstatus* dqStatus,
                                       const TileBadChannels* badChannels) const;

    StatusCode fillDigitCollection(const TileHitCollection* hitCollection,
                                   std::vector<std::vector<double>>& drawerBufferLo,
                                   std::vector<std::vector<double>>& drawerBufferHi,
                                   std::vector<int>& igain, std::vector<int>& overgain, std::vector<double>& ech_int,
                                   std::vector<bool> &signal_in_channel, const TileEMScale* emScale,
                                   const TileSamplingFraction* samplingFraction, const TilePulse& pulse) const;

    SG::ReadHandleKey<TileHitContainer> m_hitContainerKey{this,
         "TileHitContainer", "TileHitCnt", "input Tile hit container key"};

    SG::ReadHandleKey<TileHitContainer> m_hitContainer_DigiHSTruthKey{this,
         "TileHitContainer_DigiHSTruth","TileHitCnt_DigiHSTruth", "input Tile hit container key"};

    Gaudi::Property<bool> m_onlyUseContainerName{this,
        "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};

    SG::ReadHandleKey<TileDigitsContainer> m_inputDigitContainerKey{this, "InputTileDigitContainer","",""};
    std::string m_inputDigitContainerName{""};

    SG::WriteHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Output Tile digits container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_digitsContainer_DigiHSTruthKey{this,
        "TileDigitsContainer_DigiHSTruth", "TileDigitsCnt_DigiHSTruth", "Output DigiHSTruth Tile digits container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_filteredDigitsContainerKey{this,
         "TileFilteredContainer", "TileDigitsFlt", "Output filtered Tile digits container key"};

    Gaudi::Property<double> m_filterThreshold{this,
         "FilterThreshold", 100.0 * Athena::Units::MeV, "Threshold on filtered digits (default - 100 MeV)"};

    Gaudi::Property<double> m_filterThresholdMBTS{this,
         "FilterThresholdMBTS", 0.0 * Athena::Units::MeV, "Threshold on filtered digits of MBTS (default - 0 MeV)"};

    Gaudi::Property<std::string> m_infoName{this,
         "TileInfoName", "TileInfo", "TileInfo object name"};

    Gaudi::Property<bool> m_integerDigits{this,
         "IntegerDigits", true, "Round digits to integer"};

    Gaudi::Property<bool> m_calibRun{this,
         "CalibrationRun", false, "If true -> both high and low gain saved"};

    Gaudi::Property<bool> m_rndmEvtOverlay{this,
         "RndmEvtOverlay", false, "Pileup and/or noise added by overlaying random events (default=false)"};

    Gaudi::Property<bool> m_useCoolPulseShapes{this,
         "UseCoolPulseShapes", true, "Pulse shapes from database (default=true)"};

    Gaudi::Property<bool> m_maskBadChannels{this,
         "MaskBadChannels", false, "Remove channels tagged bad (default=false)"};

    Gaudi::Property<bool> m_doDigiTruth{this,
         "DoHSTruthReconstruction", false, ""};

    Gaudi::Property<int> m_allChannels{this,
         "AllChannels", -1, "Create all channels, use 0 or 1 or 2 (default=-1 - unset)"};

    PileUpMergeSvc* m_mergeSvc{nullptr}; //!< Pointer to PileUpMergeService

    const TileID* m_tileID{nullptr};
    const TileTBID* m_tileTBID{nullptr};
    const TileHWID* m_tileHWID{nullptr};
    const TileInfo* m_tileInfo{nullptr};
    const TileCablingService* m_cabling{nullptr}; //!< TileCabling instance

    std::vector<std::unique_ptr<HWIdentifier[]> > m_all_ids;

    int m_nSamples{0};            //!< Number of time slices for each channel
    int m_iTrig{0};               //!< Index of the triggering time slice
    int m_i_ADCmax{0};            //!< ADC saturation value
    float m_f_ADCmax{0.0F};       //!< ADC saturation value
    float m_f_ADCmaxHG{0.0F};     //!< ADC saturation value - 0.5
    float m_ADCmaxMinusEps{0.0F}; //!< ADC saturation value - 0.01 or something small
    float m_ADCmaxPlusEps{0.0F};  //!< ADC saturation value + 0.01 or something small
    float m_f_ADCmaskValue{0.0F}; //!< indicates channels which were masked in background dataset
    bool m_tileNoise{false};      //!< If true => generate noise in TileDigits
    bool m_tileCoherNoise{false}; //!< If true => generate coherent noise in TileDigits
    bool m_tileThresh{false};     //!< If true => apply threshold to Digits
    double m_tileThreshHi{0.0};   //!< Actual threshold value for high gain
    double m_tileThreshLo{0.0};   //!< Actual threshold value for low gain

    std::vector<double> m_digitShapeHi; //!< High gain pulse shape
    int m_nShapeHi{0};                  //!< Number of bins in high gain pulse shape
    int m_nBinsPerXHi{0};               //!< Number of bins per bunch crossing in high gain pulse shape
    int m_binTime0Hi{0};                //!< Index of time=0 bin for high gain pulse shape
    double m_timeStepHi{0.};            //!< Time step in high gain pulse shape: 25.0 / nBinsPerXHi

    std::vector<double> m_digitShapeLo; //!< Low gain pulse shape
    int m_nShapeLo{0};                  //!< Number of bins in low gain pulse shape
    int m_nBinsPerXLo{0};               //!< Number of bins per bunch crossing in low gain pulse shape
    int m_binTime0Lo{0};                //!< Index of time=0 bin for low gain pulse shape
    double m_timeStepLo{0.};            //!< Time step in low gain pulse shape: 25.0 / nBinsPerXLo

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service to use
    /// Random Stream Name
    Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "Tile_DigitsMaker", ""};

    /**
     * @brief Name of TileSampleNoise in condition store
     */
    SG::ReadCondHandleKey<TileCalibDataFlt> m_sampleNoiseKey{this,
        "TileSampleNoise", "TileSampleNoise", "Input Tile sample noise"};

    /**
     * @brief Name of TileEMScale in condition store
     */
    SG::ReadCondHandleKey<TileEMScale> m_emScaleKey{this,
        "TileEMScale", "TileEMScale", "Input Tile EMS calibration constants"};

    /**
     * @brief Name of TilePulseShape in condition store
     */
    SG::ReadCondHandleKey<TileCalibDataFlt> m_pulseShapeKey{this,
        "TilePulseShape", "TilePulseShape", "Input Tile pulse shape"};

    /**
     * @brief Name of TileSamplingFraction in condition store
     */
    SG::ReadCondHandleKey<TileSamplingFraction> m_samplingFractionKey{this,
        "TileSamplingFraction", "TileSamplingFraction", "Input Tile sampling fraction"};

    /**
     * @brief Name of TileBadChannels in condition store
     */
    SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this,
        "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey {this,
        "TileDQstatus", "", "Input TileDQstatus key" };

};

#endif // TILESIMALGS_TILEDIGITSMAKER_H

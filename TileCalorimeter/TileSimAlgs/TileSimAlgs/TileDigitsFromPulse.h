/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileDigitsFromPulse.h
// Author   : Simon Molander <simon.molander@cern.ch>
// Created  : Feb 2013
//
// DESCRIPTION
// 
// Create TileDigits from pulse simulator
//
// Properties (JobOption Parameters):
//
//    OutputDigitsContainer        string   Name of container with TileDigits to write
//    ImperfectionMean             double   Mean value of pulse shape broadening
//    ImperfectionRMS              double   RMS of pulse shape broadening
//    InTimeAmp                    double   Amplitude of in-time pulse
//    OutOfTimeAmp                 double   Amplitude of out-of-time pulse
//    InTimeOffset                 double   In-time pulse offset from nominal time
//    OutOfTimeOffset              double   Out-of-time pulse offset from nominal time
//    UseGaussNoise                bool     Set to TRUE in order to create noise from double gaussian
//    GaussNoiseAmpOne             double   Amplitude of first gaussian of double gaussian noise.
//    GaussNoiseSigmaOne           double   Standard deviation of first gaussian of double gaussian noise.
//    GaussNoiseAmpTwo             double   Amplitude of second gaussian of double gaussian noise.
//    GaussNoiseSigmaTwo           double   Standard deviation of second gaussian of double gaussian noise.    
//    UseInTimeAmpDist             bool     Set to TRUE in order to use a distribution for the in-time amplitude instead of a const.
//    UseOutOfTimeAmpDist          bool     Set to TRUE in order to use a distribution for the out-of-time amplitude instead of a const.
//    InTimeAmpDistFileName        string   Filename of file to use for amplitude distribution of in-time pulses
//    OutOfTimeAmpDistFileName     string   Filename of file to use for amplitude distribution of out-of-time pulses
//    PileUpFraction               float    Probability that an out-of-time component will be added
//    GaussianC2CPhaseVariation    float    RMS for the in-time pulse offset (channel-to-channel phase variation)
//    ChannelSpecificPedestal      bool     Set to TRUE in order to use a channel specific value for the pedestal
//    ChannelSpecificNoise         bool     Set to TRUE in order to add channel specific noise
//    OutOfTimeOffsetHistogramFile string   Filename of file containing histogram of pile-up timing distribution
//    OutOfTimeOffsetHistogramName string   name of the histogram to use for pile-up timing distribution
//    AmpDistLowerLimit            int      Set all bins lower than this to zero. Default = 135.
//    InTimeAmpDistHistogramName   string   Name of the histogram to use for in-time amplitude distribution
//    OutOfTimeAmpDistHistogramName   string   Name of the histogram to use for out-of-time amplitude distribution
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILEDIGITSFROMPULSE_H
#define TILESIMALGS_TILEDIGITSFROMPULSE_H

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCondToolNoiseSample.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CLHEP/Random/RandomEngine.h"

#include <string>

//Simulation includes
class TileSampleGenerator;
class TileSampleBuffer;
class TilePulseShape;

class TileHWID;
class TileInfo;
class IAthRNGSvc;

class TH1F;
class TFile;

/** 
@class TileDigitsFromPulse
@brief This algorithm creates TileDigits from pulses provided by pulse simulator
 */
class TileDigitsFromPulse : public AthAlgorithm {
public:
    // Constructor
    TileDigitsFromPulse(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileDigitsFromPulse();                         
    
    //Gaudi Hooks
    StatusCode initialize(); //!< initialize method
    StatusCode execute();    //!< execute method
    StatusCode finalize();   //!< finalize method

private:
    std::string m_outputContainer; //!< Name of the output TileDigitsContainer
    
    const TileHWID* m_tileHWID;
    const TileInfo* m_tileInfo;      
    
    TileRawChannelUnit::UNIT m_rChUnit; //!< Units used for the TileRawChannels (ADC, pCb, etc.)(see TileInfo.h)
    TileFragHash::TYPE m_rChType; //!< Type of TileRawChannels (Digitizar, OF1, OF2, Fit, etc.)(see TileFragHash.h)
    
    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

    SG::WriteHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Output Tile digits container key"};

    SG::WriteHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "TrueAmp", "Output Tile raw channel container key"};
    
    ServiceHandle <IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service to use

    
    //Parameters
    double m_imperfectionMean; //!< Mean value of pulse shape broadening
    double m_imperfectionRms; //!< RMS of pulse shape broadening
    double m_inTimeAmp; //!< Amplitude of in-time pulse
    double m_ootAmp; //!< Amplitude of out-of-time pulse
    double m_itOffset; //!< In-time pulse offset from nominal time
    double m_ootOffset; //!< Out-of-time pulse offset from nominal time
    bool m_gaussNoise; //!< Set to TRUE in order to create noise from double gaussian
    double m_GNAmpOne; //!< Amplitude of first gaussian of double gaussian noise.
    double m_GNSigmaOne; //!< Standard deviation of first gaussian of double gaussian noise.
    double m_GNAmpTwo; //!< Amplitude of second gaussian of double gaussian noise.
    double m_GNSigmaTwo; //!< Standard deviation of second gaussian of double gaussian noise.    
    bool m_useItADist; //!< Set to TRUE in order to use a distribution for the in-time amplitude instead of a constant value
    bool m_useOotADist; //!< Set to TRUE in order to use a distribution for the out-of-time amplitude instead of a constant value
    float m_pileUpFraction; //!< Probability that an out-of-time component will be added
    float m_gausC2C; //!< RMS for the in-time pulse offset (channel-to-channel phase variation)
    std::string m_itADistFileName; //!< Filename of file to use for amplitude distribution of in-time pulses
    std::string m_ootADistFileName; //!< Filename of file to use for amplitude distribution of out-of-time pulses
    bool m_chanPed; //!< Use channel specific pedestal value if true. Otherwise, same for all channels.
    bool m_chanNoise; //!< Add channel specific noise
    std::string m_ootOffsetFileName; //!< File name for offset timing distribution histogram
    std::string m_ootOffsetHistName; //!< Name of the histogram for timing offset distribution
    int m_AmpDistLowLim; //!< Set all bins to the left of this bin to 0 in the amplitude distribution histograms
    std::string m_itADistHistName; //!< Name of histogram for in-time amplitude distribution
    std::string m_ootADistHistName; //!< Name of  histogram for out-of-time amplitude distribution
    
    bool m_simQIE; //!<Raw PMT pulses are generated if the option is set to true. The option is intended to simulate the QIE FEB.

    int m_seed;
    int m_BunchSpacing; //!< Time between pulses in ms 25, 50 or 75
    int m_nSamples;  //!< number of read out samples
    int m_nPul;  //!< number of pileup pulses 
    int m_nPul_eff;  //Used for symetrization of PU in computation
    std::vector<float> m_PUAmp;
    
    //Members for simulator
    TilePulseShape*      m_ps[2]; //!< Class for defining pulse. One element for lo gain and one for hi.
    TileSampleBuffer*    m_buf; //!< Buffer class to hold generated pulses
    TileSampleGenerator* m_tsg; //!< Pulse generating class
    
    //Histograms for distribution
    TH1F* m_ootDist; //!< Histogram to hold the distribution of out-of-time amplitudes.
    TH1F* m_itDist; //!< Histogram to hold the distribution of in-time amplitudes.
    TFile* m_itFile; //!< File that holds the distribution of in-time amplitudes.
    TFile* m_ootFile; //!< File that holds the distribution of out-of-time amplitudes.
    TH1F* m_ootOffsetDist; //!< Histogram to hold the distribution of out-of-time timing offsets
    TFile* m_ootOffsetFile; //!< File that holds the distribution of out-of-time timing offsets
    bool m_useOffsetHisto; //!< Internally used to keep track of wether a histogram has been opened or not
    
    //Internal methods
    bool makeDist(TFile*& file, TH1F*& hist, std::string fileName, std::string histName="h_Eopt_hi"); //!< Method to read distribution from file

};

#endif // TILESIMALGS_TILEDIGITSFROMPULSE_H


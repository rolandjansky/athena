// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPrimitives.h 576255 2013-12-19 12:54:41Z emoyse $
#ifndef XAODTRACKING_XAODPRIMITIVES_H
#define XAODTRACKING_XAODPRIMITIVES_H

// EDM include(s):
#include "EventPrimitives/EventPrimitives.h"
#include "AthContainers/tools/AuxDataTraits.h"
#include "CxxUtils/checker_macros.h"

#ifndef AmgMatrixDef
#define AmgMatrixDef
#define AmgMatrix(col, row) Eigen::Matrix<double,col,row,0,col,row>
#define AmgSymMatrix(dim) Eigen::Matrix<double,dim,dim,0,dim,dim>
#endif

#ifndef AmgVectorDef
#define AmgVectorDef
#define AmgVector(col) Eigen::Matrix<double, col, 1, 0, col, 1>
#define AmgRowVector(row) Eigen::Matrix<double, 1, row, 0, 1, row>
#endif

namespace xAOD {

  //typedef ROOT::Math::SMatrix<float,5,5,ROOT::Math::MatRepSym<float,5> >     ParametersCovMatrix_t;
  //typedef ROOT::Math::SVector<float,5>                                       DefiningParameters_t;
  //typedef ROOT::Math::SVector<float,6>                                       CurvilinearParameters_t;
  typedef AmgSymMatrix(5)                                                       ParametersCovMatrix_t;
  typedef AmgVector(5)                                                          DefiningParameters_t;
  typedef AmgVector(6)                                                          CurvilinearParameters_t;
    /// Enums to identify who created this track and which properties does it have.
  enum TrackFitter
  {
      ///Track fitter not defined.
    Unknown        = 0,
      ///tracks produced by iPat
      iPatTrackFitter      = 1,
      ///tracks produced by xKalman
      xKalman        = 2,
      ///tracks produced by the Kalman Fitter
      KalmanFitter       = 3,
      ///Tracks from Gaussian Sum Filter
      GaussianSumFilter      = 4,
      ///Track's from Thijs' global chi^2 fitter
      GlobalChi2Fitter       = 5,
      ///Fast Kalman filter from HLT with simplified material effects
      DistributedKalmanFilter    = 6,
      ///Adpative track fitter with fuzzy hit assignment
      DeterministicAnnealingFilter = 7,
      /// Kalman DNA Fitter
      KalmanDNAFitter            = 8,
      /// Track produced with Muonboy's internal fitter.
      MuonboyFitter                = 9,
      ///maximum number of enums
      NumberOfTrackFitters     = 10
  };

  enum TrackProperties
  {
      /// A brem fit was performed on this track
      BremFit         = 1,
      /// A brem fit was performed on this track and this fit was successful
      BremFitSuccessful       = 2,
      /// A straight track
      StraightTrack       = 3,
      /// A slimmed track
      SlimmedTrack      = 4,
      /// A track with a kink or a hard scatter
      HardScatterOrKink       = 5,
      /// A LowPt track 
      LowPtTrack        = 6,
      ///maximum number of enums
      NumberOfTrackProperties = 7
  };

  enum TrackPatternRecoInfo
  {
    /// Tracks from SiSPSeedFinder
    SiSPSeededFinder             = 0,
    /// Tracks from SiCTBTracking 
    SiCTBTracking                = 1,
    /// Tracks with InDetAmbiguitySolver used
    InDetAmbiguitySolver         = 2,
    /// Tracks with InDetExtensionProcessor used
    InDetExtensionProcessor      = 3,
    /// Tracks from TRT Seeded Track finder
    TRTSeededTrackFinder         = 4,
    /// Tracks from MuonBoy
    Muonboy                      = 5,
    /// Tracks from MuGirl not refitted with iPat
    MuGirlUnrefitted             = 6,
    ///Tracks produced by STACO
    STACO                        = 7,
    /// Tracks produced by StacoLowpt
    StacoLowPt                   = 8,
    /// Tracks produced by MuTag
    MuTag                        = 9,
    ///tracks produced by Moore 
    MooreToTrackTool             = 10,
    ///TrigInDetTracks produced by TrigIDSCAN, running at trigger LVL2
    TrigIDSCAN                   = 11,
    ///TrigInDetTracks produced by TrigSiTrack, running at trigger LVL2
    TrigSiTrack                  = 12,
    ///TrigInDetTracks produced by TrigTRTxK, running at trigger LVL2
    TrigTRTxK                    = 13,
    ///TrigInDetTracks produced by TrigTRTLUT, running at trigger LVL2
    TrigTRTLUT                   = 14,
    /// Track from FATRAS
    Fatras                       = 15,
    /// Added because of compilation problems
    iPatLegacyCnv                = 16,
    /// Added because of compilation problems
    xKalmanLegacyCnv             = 17,
    /// Added because of compilation problems
    SimpleAmbiguityProcessorTool = 18,
    /// Added because of compilation problems
    InDetAmbiTrackSelectionTool  = 19,
    ///  TRT Standalone 
    TRTStandalone                = 20,
    ///   MuidStandalone 
    MuidStandAlone               = 21,
    /// TRTSeededSingleSpTrackFinder 
    TRTSeededSingleSpTrackFinder = 22,
    ///Back compatibility with old type of ESD 
    MooreLegacyCnv               = 23,
    ///Back compatibility with older ESD versions 
    MuidComb                     = 24,
    ///   Moore (MoMu) 
    Moore                        = 25,
    ///   MuidCombined 
    MuidCombined           = 26,
    ///   MuidVertexAssociator 
    MuidVertexAssociator         = 27,
    ///   MuGirl 
    MuGirl                       = 28,
    ///   iPatRec 
    iPatRec                      = 29,
    ///  MuGirlLowBeta 
    MuGirlLowBeta                = 30,
    ///  Fatras Simulation 
    FatrasSimulation             = 31,
    /// Track which has its hits behind it, e.g. a downward moving cosmic track which through the upper hemisphere of ATLAS will have a beamline perigee pointing
    /// down, but many/all of its hits will be above. A downward moving cosmic track which is reconstructed across all of ATLAS could also have this flag set (i.e.
    /// it does not mean that hits are ONLY behind the perigee.
    /// @warning This enum is the subject of some controversy and may be removed in the future. It would be safer to examine
    /// the track and determine this for yourself (especially as not all Track builders are currently filling this flag). EJWM.
    ReverseOrderedTrack         = 32,
    /// Track which (physically) does not intersect the ID or Calo. i.e. not a partially reconstructed IP track, but a cosmic or beam halo track.
    /// @warning This enum is the subject of some controversy and may be removed in the future. It would be safer to examine
    /// the track and determine this for yourself (especially as not all Track builders are currently filling this flag). EJWM.
    MuonNotHittingTileVolume    = 33,
    /// Entries allowing to distinguish different seed makers
    SiSpacePointsSeedMaker_Cosmic          = 34,
    SiSpacePointsSeedMaker_HeavyIon        = 35,
    SiSpacePointsSeedMaker_LowMomentum     = 36,
    SiSpacePointsSeedMaker_BeamGas         = 37,
    SiSpacePointsSeedMaker_VeryLowMomentum = 38,
    /// Muons found by the ID seeded muon recovery 
    MuidMuonRecoveryTool        = 39,
    /// Standalone muon that was obtained by refitting a combined muon using the calorimeter information of the combined and the muon measurements 
    MuidStandaloneRefit         = 40,
    /// A track in a CaloROI 
    TrackInCaloROI              = 41,
    /// Entries allowing to distinguish different seed makers
    SiSpacePointsSeedMaker_ForwardTracks   = 42,
    /// L2Star strategies 
    strategyA                              = 43,
    strategyB                              = 44,
    strategyC                              = 45,
    ///   for tracks from FTK
    FTK                                    = 46,
    ///  for tracks seeded by the FastTrackFinder
    FastTrackFinderSeed                    = 47,
    //    for tracks processed by the trigger version of the SiSPSeededFinder
    SiSPSeededFinderSimple                 = 48,
    // Large d0 for displaced vertex searches
    SiSpacePointsSeedMaker_LargeD0         = 49,
    // SLHCConversion Track flag
    SiSpacePointsSeedMaker_SLHCConversionTracks = 50,
    // Pseudotracking
    Pseudotracking			   = 51,
     ///maximum number of enums
    NumberOfTrackRecoInfo                  = 52
  };

  enum ParticleHypothesis { nonInteracting  = 0,
    geantino        = 0,
    electron        = 1,
    muon            = 2,
    pion            = 3,
    kaon            = 4,
    proton          = 5,
    photon          = 6,  /// for Fatras usage
    neutron         = 7,  /// for Fatras usage
    pi0             = 8, /// for Fatras usage
    k0              = 9, /// for Fatras usage
    nonInteractingMuon = 10, /// For material collection
    noHypothesis    = 99,
    undefined       = 99
  };

  /// Enum allowing us to know where in ATLAS the parameters are defined.
  enum ParameterPosition {
    /// Parameter defined at the Vertex/Beamline.
    BeamLine,
    /// Parameter defined at the position of the 1st measurement.
    FirstMeasurement,
    /// Parameter defined at the position of the last measurement.
    LastMeasurement,
    /// Parameter defined at the entrance to the calorimeter.
    CalorimeterEntrance,
    /// Parameter defined at the exit of the calorimeter.
    CalorimeterExit,
    /// Parameter defined at the entrance to the Muon Spectrometer.
    MuonSpectrometerEntrance
  };


  /// Enumerates the different types of information stored in Summary. 
  /// Please note that the values have specific types - i.e. some are float, whilst most are uint8_t.
  /// When adding a new transient information type, please make sure to increase numberOfTrackSummaryTypes.*/
  enum SummaryType {
    // --- Inner Detector
    numberOfContribPixelLayers      =29,  //!< number of contributing layers of the pixel detector [unit8_t].
    numberOfBLayerHits              = 0,  //!< these are the hits in the first pixel layer, i.e. b-layer [unit8_t].
    numberOfBLayerOutliers          =31,  //!< number of blayer outliers [unit8_t].
    numberOfBLayerSharedHits        =16,  //!< number of Pixel b-layer hits shared by several tracks [unit8_t].
    numberOfBLayerSplitHits         =43,  //!< number of Pixel b-layer hits split by cluster splitting [unit8_t].
    expectBLayerHit                 =42,  //!< Do we expect a b-layer hit for this track? [unit8_t] (should be [bool])
    expectInnermostPixelLayerHit                 =52,  //!< Do we expect a 0th-layer hit for this track?
    numberOfInnermostPixelLayerHits              =53,  //!< these are the hits in the 0th pixel layer?
    numberOfInnermostPixelLayerOutliers          =54,  //!< number of 0th layer outliers  
    numberOfInnermostPixelLayerSharedHits        =55,  //!< number of Pixel 0th layer hits shared by several tracks.
    numberOfInnermostPixelLayerSplitHits         =56,  //!< number of Pixel 0th layer hits split by cluster splitting 
    expectNextToInnermostPixelLayerHit                 =57,  //!< Do we expect a 1st-layer hit for this track?
    numberOfNextToInnermostPixelLayerHits              = 58,  //!< these are the hits in the 1st pixel layer
    numberOfNextToInnermostPixelLayerOutliers          =59,  //!< number of 1st pixel layer outliers  
    numberOfNextToInnermostPixelLayerSharedHits        =60,  //!< number of Pixel 1st layer hits shared by several tracks.
    numberOfNextToInnermostPixelLayerSplitHits         =61,  //!< number of Pixel 1st layer hits split by cluster splitting 
    numberOfDBMHits                 = 63, //!< these are the number of DBM hits [unit8_t].
    numberOfPixelHits               = 2,  //!< these are the pixel hits, including the b-layer [unit8_t].
    numberOfPixelOutliers           =41,  //!< these are the pixel outliers, including the b-layer [unit8_t].
    numberOfPixelHoles              = 1,  //!< number of pixel layers on track with absence of hits [unit8_t].
    numberOfPixelSharedHits         =17,  //!< number of Pixel all-layer hits shared by several tracks [unit8_t].
    numberOfPixelSplitHits          =44,  //!< number of Pixel all-layer hits split by cluster splitting [unit8_t].
    numberOfGangedPixels            =14,  //!< number of pixels which have a ganged ambiguity [unit8_t].
    numberOfGangedFlaggedFakes      =32,  //!< number of Ganged Pixels flagged as fakes [unit8_t].
    numberOfPixelDeadSensors        =33,  //!< number of dead pixel sensors crossed [unit8_t].
    numberOfPixelSpoiltHits         =35,  //!< number of pixel hits with broad errors (width/sqrt(12)) [unit8_t].
    numberOfSCTHits                 = 3,  //!< number of hits in SCT [unit8_t].
    numberOfSCTOutliers             =39,  //!< number of SCT outliers [unit8_t].
    numberOfSCTHoles                = 4,  //!< number of SCT holes [unit8_t].
    numberOfSCTDoubleHoles          =28,  //!< number of Holes in both sides of a SCT module [unit8_t].
    numberOfSCTSharedHits           =18,  //!< number of SCT hits shared by several tracks [unit8_t].
    numberOfSCTDeadSensors          =34,  //!< number of dead SCT sensors crossed [unit8_t].
    numberOfSCTSpoiltHits           =36,  //!< number of SCT hits with broad errors (width/sqrt(12)) [unit8_t].
    numberOfTRTHits                 = 5,  //!< number of TRT hits [unit8_t].
    numberOfTRTOutliers             =19,  //!< number of TRT outliers [unit8_t].
    numberOfTRTHoles                =40,  //!< number of TRT holes [unit8_t].
    numberOfTRTHighThresholdHits    = 6,  //!< number of TRT hits which pass the high threshold (only xenon counted) [unit8_t].
    numberOfTRTHighThresholdHitsTotal= 64,  //!< total number of TRT hits which pass the high threshold  [unit8_t].
    //    numberOfTRTHitsUsedFordEdx      = 65, //!< number of TRT hits used for dE/dx computation
    numberOfTRTHighThresholdOutliers=20,  //!< number of TRT high threshold outliers (only xenon counted) [unit8_t].
    numberOfTRTDeadStraws           =37,  //!< number of dead TRT straws crossed [unit8_t].
    numberOfTRTTubeHits             =38,  //!< number of TRT tube hits [unit8_t].
    numberOfTRTXenonHits            =46,  //!< number of TRT hits on track in straws with xenon [unit8_t].
    numberOfTRTSharedHits           =62,  //!< number of TRT hits used by more than one track

    // --- Muon Spectrometer
    numberOfPrecisionLayers = 7,       //!< layers with at least 3 hits [unit8_t].
    numberOfPrecisionHoleLayers = 8,   //!< layers with holes AND no hits [unit8_t].
    numberOfPhiLayers = 9,             //!< layers with a trigger phi hit [unit8_t].
    numberOfPhiHoleLayers = 10,        //!< layers with trigger phi holes but no hits [unit8_t].
    numberOfTriggerEtaLayers = 11,     //!< layers with trigger eta hits [unit8_t].
    numberOfTriggerEtaHoleLayers = 12, //!< layers with trigger eta holes but no hits [unit8_t].
    numberOfGoodPrecisionLayers = 66,  //!< layers with at least 3 hits that are not deweighted [uint8_t]

    // --- all
    numberOfOutliersOnTrack         =15,  //!< number of measurements flaged as outliers in TSOS [unit8_t].
    standardDeviationOfChi2OS       = 30, //!< 100 times the standard deviation of the chi2 from the surfaces [unit8_t].
    // --- float values
    eProbabilityComb                = 47, //!< Electron probability from combining the below probabilities [float].
    eProbabilityHT                  = 48, //!< Electron probability from  High Threshold (HT) information [float].   
    // removed:   eProbabilityToT                 = 49, //!< Electron probability from Time-Over-Threshold (ToT) information [float].    
    // removed:   eProbabilityBrem                = 50, //!< Electron probability from Brem fitting (DNA) [float].  
    pixeldEdx                       = 51, //!< the dE/dx estimate, calculated using the pixel clusters [?]
    // -- numbers...
    numberOfTrackSummaryTypes       = 67
  };

  /// Enumerates the different types of information stored in Summary. 
  /// When adding a new transient information type, please make sure to increase numberOfTrackSummaryTypes.*/
  enum MuonSummaryType {

    // global hit counts
    primarySector = 0,                 //!< primary sector crossed by the muon
    secondarySector = 1,               //!< secondary sector crossed by the muon (set to the primary if the muon only passed one sector)

    innerSmallHits = 2,        //!< number of precision hits in the inner small layer
    innerLargeHits = 3,        //!< number of precision hits in the inner large layer
    middleSmallHits = 4,      //!< number of precision hits in the middle small layer
    middleLargeHits = 5,      //!< number of precision hits in the middle large layer
    outerSmallHits = 6,      //!< number of precision hits in the outer small layer
    outerLargeHits = 7,       //!< number of precision hits in the outer large layer
    extendedSmallHits = 8,    //!< number of precision hits in the extended small layer
    extendedLargeHits = 9,    //!< number of precision hits in the extended large layer

    innerSmallHoles = 10,       //!< number of precision holes in the inner small layer
    innerLargeHoles = 11,       //!< number of precision holes in the inner large layer
    middleSmallHoles = 12,      //!< number of precision holes in the middle small layer
    middleLargeHoles = 13,      //!< number of precision holes in the middle large layer
    outerSmallHoles = 14,      //!< number of precision holes in the outer small layer
    outerLargeHoles = 15,       //!< number of precision holes in the outer large layer
    extendedSmallHoles = 16,    //!< number of precision holes in the extended small layer
    extendedLargeHoles = 17,    //!< number of precision holes in the extended large layer

    phiLayer1Hits = 18,         //!< number of phi hits in the first trigger layer (BML1 ot T4)
    phiLayer2Hits = 19,         //!< number of phi hits in the second trigger layer (BML2 ot T1)
    phiLayer3Hits = 20,         //!< number of phi hits in the third trigger layer (BOL1 ot T2)
    phiLayer4Hits = 21,         //!< number of phi hits in the fourth trigger layer (T3)

    etaLayer1Hits = 22,         //!< number of eta hits in the first trigger layer (BML1 ot T4)
    etaLayer2Hits = 23,         //!< number of eta hits in the second trigger layer (BML2 ot T1)
    etaLayer3Hits = 24,         //!< number of eta hits in the third trigger layer (BOL1 ot T2)
    etaLayer4Hits = 25,         //!< number of eta hits in the fourth trigger layer (T3)

    phiLayer1Holes = 26,         //!< number of phi holes in the first trigger layer (BML1 ot T4)
    phiLayer2Holes = 27,         //!< number of phi holes in the second trigger layer (BML2 ot T1)
    phiLayer3Holes = 28,         //!< number of phi holes in the third trigger layer (BOL1 ot T2)
    phiLayer4Holes = 29,         //!< number of phi holes in the fourth trigger layer (T3)

    etaLayer1Holes = 30,         //!< number of eta holes in the first trigger layer (BML1 ot T4)
    etaLayer2Holes = 31,         //!< number of eta holes in the second trigger layer (BML2 ot T1)
    etaLayer3Holes = 32,         //!< number of eta holes in the third trigger layer (BOL1 ot T2)
    etaLayer4Holes = 33,         //!< number of eta holes in the fourth trigger layer (T3)

    innerClosePrecisionHits = 34,         //!< number of close precision hits in the inner layer
    middleClosePrecisionHits = 35,        //!< number of close precision hits in the middle layer
    outerClosePrecisionHits = 36,         //!< number of close precision hits in the outer layer
    extendedClosePrecisionHits = 37,      //!< number of close precision hits in the extended layer

    innerOutBoundsPrecisionHits = 38,     //!< number of out-of-bounds hits in the inner layer
    middleOutBoundsPrecisionHits = 39,    //!< number of out-of-bounds hits in the middle layer
    outerOutBoundsPrecisionHits = 40,     //!< number of out-of-bounds hits in the outer layer
    extendedOutBoundsPrecisionHits = 41,  //!< number of out-of-bounds hits in the extended layer
    combinedTrackOutBoundsPrecisionHits = 42, //!< total out-of-bounds hits on the combined track

    isEndcapGoodLayers = 43,                //!< if non-deweighted track chambers are in endcap
    isSmallGoodSectors = 44,                //!< if non-deweighted track chambers are small

    phiLayer1RPCHits  = 45,  //!< number of phi hits  in the first  RPC trigger layer (BML1)
    phiLayer2RPCHits  = 46,  //!< number of phi hits  in the second RPC trigger layer (BML2)
    phiLayer3RPCHits  = 47,  //!< number of phi hits  in the third  RPC trigger layer (BOL1)
    phiLayer4RPCHits  = 48,  //!< number of phi hits  in the fourth RPC trigger layer (BOL2)

    etaLayer1RPCHits  = 49,  //!< number of eta hits  in the first  RPC trigger layer (BML1)
    etaLayer2RPCHits  = 50,  //!< number of eta hits  in the second RPC trigger layer (BML2)
    etaLayer3RPCHits  = 51,  //!< number of eta hits  in the third  RPC trigger layer (BOL1)
    etaLayer4RPCHits  = 52,  //!< number of eta hits  in the fourth RPC trigger layer (BOL2)

    phiLayer1RPCHoles = 53,  //!< number of phi holes in the first  RPC trigger layer (BML1)
    phiLayer2RPCHoles = 54,  //!< number of phi holes in the second RPC trigger layer (BML2)
    phiLayer3RPCHoles = 55,  //!< number of phi holes in the third  RPC trigger layer (BOL1)
    phiLayer4RPCHoles = 56,  //!< number of phi holes in the fourth RPC trigger layer (BOL2)

    etaLayer1RPCHoles = 57,  //!< number of eta holes in the first  RPC trigger layer (BML1)
    etaLayer2RPCHoles = 58,  //!< number of eta holes in the second RPC trigger layer (BML2)
    etaLayer3RPCHoles = 59,  //!< number of eta holes in the third  RPC trigger layer (BOL1)
    etaLayer4RPCHoles = 60,  //!< number of eta holes in the fourth RPC trigger layer (BOL2)

    phiLayer1TGCHits  = 61,  //!< number of phi hits  in the first  TGC trigger layer (T4)
    phiLayer2TGCHits  = 62,  //!< number of phi hits  in the second TGC trigger layer (T1)
    phiLayer3TGCHits  = 63,  //!< number of phi hits  in the third  TGC trigger layer (T2)
    phiLayer4TGCHits  = 64,  //!< number of phi hits  in the fourth TGC trigger layer (T3)

    etaLayer1TGCHits  = 65,  //!< number of eta hits  in the first  TGC trigger layer (T4)
    etaLayer2TGCHits  = 66,  //!< number of eta hits  in the second TGC trigger layer (T1)
    etaLayer3TGCHits  = 67,  //!< number of eta hits  in the third  TGC trigger layer (T2)
    etaLayer4TGCHits  = 68,  //!< number of eta hits  in the fourth TGC trigger layer (T3)

    phiLayer1TGCHoles = 69,  //!< number of phi holes in the first  TGC trigger layer (T4)
    phiLayer2TGCHoles = 70,  //!< number of phi holes in the second TGC trigger layer (T1)
    phiLayer3TGCHoles = 71,  //!< number of phi holes in the third  TGC trigger layer (T2)
    phiLayer4TGCHoles = 72,  //!< number of phi holes in the fourth TGC trigger layer (T3)

    etaLayer1TGCHoles = 73,  //!< number of eta holes in the first  TGC trigger layer (T4)
    etaLayer2TGCHoles = 74,  //!< number of eta holes in the second TGC trigger layer (T1)
    etaLayer3TGCHoles = 75,  //!< number of eta holes in the third  TGC trigger layer (T2)
    etaLayer4TGCHoles = 76,  //!< number of eta holes in the fourth TGC trigger layer (T3)

    // New Small Wheel
    //STGC
    phiLayer1STGCHits  = 79,  //!< number of phi hits  in the first  STGC trigger layer (STGC1)
    phiLayer2STGCHits  = 80,  //!< number of phi hits  in the second STGC trigger layer (STGC2)

    etaLayer1STGCHits  = 81,  //!< number of eta hits  in the first  STGC trigger layer (STGC1)
    etaLayer2STGCHits  = 82,  //!< number of eta hits  in the second STGC trigger layer (STGC2)

    phiLayer1STGCHoles = 83,  //!< number of phi holes in the first  STGC trigger layer (STGC1)
    phiLayer2STGCHoles = 84,  //!< number of phi holes in the second STGC trigger layer (STGC2)
 
    etaLayer1STGCHoles = 85,  //!< number of eta holes in the first  STGC trigger layer (STGC1)
    etaLayer2STGCHoles = 86,  //!< number of eta holes in the second STGC trigger layer (STGC2)

    //MM
    MMHits = 87,
    MMHoles = 88,
    // Note: there is currently no MuonStationIndex for the two MM layers
    // In the future it might be needed to add them in order to have 
    // separate counters as follows
    // Layer1MMHits  = 87,  //!< number of eta hits  in the first  MM layer
    // Layer2MMHits  = 88,  //!< number of eta hits  in the second MM layer
    // Layer1MMHoles = 89,  //!< number of eta holes in the first  MM layer
    // Layer2MMHoles = 90,  //!< number of eta holes in the second MM layer


    //CSC debugging
    cscEtaHits = 77,           //!<number of CSC eta clusters on the track
    cscUnspoiledEtaHits = 78,  //!<number of unspoiled CSC eta clusters on track

    // -- numbers...
    numberOfMuonSummaryTypes       = 89
  };

  /// A convenience namespace to make the client code easier to understand
  namespace VxType {
    /// Vertex types
    ///
    /// Different vertex types that can be created in the offline
    /// reconstruction.
    ///
    enum VertexType {
      NoVtx   = 0, ///< Dummy vertex. TrackParticle was not used in vertex fit
        PriVtx  = 1, ///< Primary vertex
        SecVtx  = 2, ///< Secondary vertex
        PileUp  = 3, ///< Pile-up vertex
        ConvVtx = 4, ///< Conversion vertex
        V0Vtx   = 5, ///< Vertex from V0 decay
        KinkVtx = 6, ///< Kink vertex
        NotSpecified = -99 ///< Default value, no explicit type set
    };
  } // namespace VxType
} //  namespace xAOD 


namespace SG {


// The default Matrix constructor does not initialize the matrix contents.
// Specialize this for Matrix, so that when fill() gets used inside DataVector,
// we'll fill with a fully initialized instance.
template <typename SCALAR, int ROWS, int COLS, int OPTIONS, int MAXROWS, int MAXCOLS>
struct Zero<Eigen::Matrix<SCALAR, ROWS, COLS, OPTIONS, MAXROWS, MAXCOLS> >
{
  typedef Eigen::Matrix<SCALAR, ROWS, COLS, OPTIONS, MAXROWS, MAXCOLS> Matrix;
  static Matrix zero ATLAS_CHECK_THREAD_SAFETY ()
  {
    Matrix m;
    m.setZero();
    return m;
  }
};


} // namespace SG


#endif // XAODTRACKING_XAODPRIMITIVES_H

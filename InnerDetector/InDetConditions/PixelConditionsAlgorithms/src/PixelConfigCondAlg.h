/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file PixelConditionsAlgorithms/PixelConfigCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date December, 2019
 * @brief Store pixel module parameters in PixelModuleData.
 */

#ifndef PIXELCONFIGCONDALG
#define PIXELCONFIGCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CommissionEvent/ComTime.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

class PixelConfigCondAlg : public AthReentrantAlgorithm {
  public:
    PixelConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    // Key for basic pixel parameters
    SG::WriteCondHandleKey<PixelModuleData> m_writeKey
    {this, "WriteKey", "PixelModuleData", "Output key of pixel module data"};

    // Switch parameters
    Gaudi::Property<bool> m_useCablingConditions
    {this, "UseCablingConditions", true, "Use cabling map conditions DB"};

    // Digitization parameters
    Gaudi::Property<double> m_bunchSpace
    {this, "BunchSpace", 25.0, "Bunch space [ns]"};

    Gaudi::Property<bool> m_UseComTime
    {this, "UseComTime", false, "Use computed time used in cosmic/commissioning timing"};

    double m_ComTime{0.0};

    SG::ReadHandleKey<ComTime> m_ComTimeKey
    {this, "ComTimeKey", "ComTime", "Commissioning time for cosmic"};

    Gaudi::Property<std::vector<int>> m_BarrelNumberOfBCID
    {this, "BarrelNumberOfBCID", {1,1,1,1}, "BCID numbers for barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_EndcapNumberOfBCID
    {this, "EndcapNumberOfBCID", {1,1,1}, "BCID numbers for endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMNumberOfBCID
    {this, "DBMNumberOfBCID", {1,1,1}, "BCID numbers for DBM layers"};

    Gaudi::Property<std::vector<double>> m_BarrelTimeOffset
    {this, "BarrelTimeOffset", {5.0,5.0,5.0,5.0}, "Offset time of barrel pixel layer"};

    Gaudi::Property<std::vector<double>> m_EndcapTimeOffset
    {this, "EndcapTimeOffset", {5.0,5.0,5.0}, "Offset time of endcap pixel layer"};

    Gaudi::Property<std::vector<double>> m_DBMTimeOffset
    {this, "DBMTimeOffset", {5.0,5.0,5.0}, "Offset time of DBM layer"};

    Gaudi::Property<std::vector<double>> m_BarrelTimeJitter
    {this, "BarrelTimeJitter", {0.0,0.0,0.0,0.0}, "Time jitter of barrel pixel layer"};

    Gaudi::Property<std::vector<double>> m_EndcapTimeJitter
    {this, "EndcapTimeJitter", {0.0,0.0,0.0}, "Time jitter of endcap pixel layer"};

    Gaudi::Property<std::vector<double>> m_DBMTimeJitter
    {this, "DBMTimeJitter", {0.0,0.0,0.0}, "Time jitter of DBM layer"};

    // Dead map key. This will be replaced once new deadmap format is validated.
    SG::ReadCondHandleKey<CondAttrListCollection> m_readDeadMapKey
    {this, "ReadDeadMapKey", "/PIXEL/PixMapOverlay", "Input key of deadmap conditions folder"};

    //====================================================================================
    // Run-dependent SIMULATION(digitization) parameters:
    //
    //   So far, they are year-granularity (3 entries!), thus they may be still 
    //   controlled via job option.
    //
    // MC Project:               RUN1            RUN2 mc16a            RUN2 mc16d            RUN2 mc16e 
    //  Year:                  - 2014             2015/2016                  2017                  2018
    //  Run Number:           <222222                284500                300000                310000
    //                                 
    // Barrel:                             
    //  ToT:         [   3,   3,   3] [  -1,   5,   5,   5] [  -1,   5,   5,   5] [  -1,   3,   5,   5]
    //  Latency:     [ 256, 256, 256] [  -1, 150, 256, 256] [  -1, 150, 256, 256] [  -1, 150, 256, 256]
    //  Duplicaiton: [   T,   T,   T] [ N/A,   F,   F,   F] [ N/A,   F,   F,   F] [ N/A,   F,   F,   F]
    //  SmallHit:    [   7,   7,   7] [ N/A,   0,   0,   0] [ N/A,   0,   0,   0] [ N/A,   0,   0,   0]
    //  TimingTune:  [2009,2009,2009] [ N/A,2015,2015,2015] [ N/A,2018,2018,2018] [ N/A,2018,2018,2018]
    //  CrossTalk:   [0.06,0.06,0.06] [0.30,0.06,0.06,0.06] [0.30,0.06,0.06,0.06] [0.30,0.06,0.06,0.06]
    //  NoiseOcc.:   [5e-8,5e-8,5e-8] [5e-8,5e-8,5e-8,5e-8] [5e-8,5e-8,5e-8,5e-8] [5e-8,5e-8,5e-8,5e-8]
    //  DisalbePix:  [9e-3,9e-3,9e-3] [9e-3,9e-3,9e-3,9e-3] [9e-3,9e-3,9e-3,9e-3] [9e-3,9e-3,9e-3,9e-3]
    //  NoiseShape:  [2018,2018,2018] [2018,2018,2018,2018] [2018,2018,2018,2018] [2018,2018,2018,2018]
    //                                     
    // Endcap:                             
    //  ToT:         [   3,   3,   3]      [   5,   5,   5]      [   5,   5,   5]      [   5,   5,   5]
    //  Latency:     [ 256, 256, 256]      [ 256, 256, 256]      [ 256, 256, 256]      [ 256, 256, 256]
    //  Duplicaiton: [   T,   T,   T]      [   F,   F,   F]      [   F,   F,   F]      [   F,   F,   F]
    //  SmallHit:    [   7,   7,   7]      [   0,   0,   0]      [   0,   0,   0]      [   0,   0,   0]
    //  TimingTune:  [2009,2009,2009]      [2015,2015,2015]      [2018,2018,2018]      [2018,2018,2018]
    //  CrossTalk:   [0.06,0.06,0.06]      [0.06,0.06,0.06]      [0.06,0.06,0.06]      [0.06,0.06,0.06]
    //  NoiseOcc.:   [5e-8,5e-8,5e-8]      [5e-8,5e-8,5e-8]      [5e-8,5e-8,5e-8]      [5e-8,5e-8,5e-8]
    //  DisalbePix:  [9e-3,9e-3,9e-3]      [9e-3,9e-3,9e-3]      [9e-3,9e-3,9e-3]      [9e-3,9e-3,9e-3]
    //  NoiseShape:  [2018,2018,2018]      [2018,2018,2018]      [2018,2018,2018]      [2018,2018,2018]
    //                                
    // DBM:                           
    //  ToT:                    [N/A]      [  -1,  -1,  -1]      [  -1,  -1,  -1]      [  -1,  -1,  -1]
    //  CrossTalk:              [N/A]      [0.06,0.06,0.06]      [0.06,0.06,0.06]      [0.06,0.06,0.06]
    //  NoiseOcc.:              [N/A]      [5e-8,5e-8,5e-8]      [5e-8,5e-8,5e-8]      [5e-8,5e-8,5e-8]
    //  DisalbePix:             [N/A]      [9e-3,9e-3,9e-3]      [9e-3,9e-3,9e-3]      [9e-3,9e-3,9e-3]
    //
    // See  https://twiki.cern.ch/twiki/bin/view/Atlas/PixelConditionsRUN2
    // for further details.
    //
    //
    // Upgrade ITk-pixel
    //
    // MC Project:               RUN4 (unknown)
    //  Year:                              2027 
    //  Run Number:             [240000-250000] 
    //                               
    // Barrel:                       
    //  ToT:         [   3,   3,   3,   3,   3]
    //  CrossTalk:   [0.06,0.06,0.06,0.06,0.06]
    //  NoiseOcc.:   [5e-8,5e-8,5e-8,5e-8,5e-8]
    //  DisalbePix:  [9e-3,9e-3,9e-3,9e-3,9e-3]
    //  NoiseShape:  [2018,2018,2018,2018,2018]
    //                               
    // Endcap:                       
    //  ToT:         [   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3]
    //  CrossTalk:   [0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06]
    //  NoiseOcc.:   [5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8]
    //  DisalbePix:  [9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3]
    //  NoiseShape:  [2018,2018,2018,2018,2018,2018,2018,2018,2018,2018,2018,2018,2018,2018]
    //
    //====================================================================================

    // Variable definition is a bit ugly here...
    //====================================================================================
    // Barrel RUN2 2015/2016
    Gaudi::Property<std::vector<int>> m_BarrelToTThreshold2016
    {this, "BarrelToTThreshold2016", {-1, 5, 5, 5}, "ToT thresholds for barrel pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelLatency2016
    {this, "FEI3BarrelLatency2016", {  0,151,256,256}, "FEI3 latency for barrel pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<bool>> m_FEI3BarrelHitDuplication2016
    {this, "FEI3BarrelHitDuplication2016", {false,false,false,false}, "Use FEI3 hit-duplicaiton for barrel pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<int>>  m_FEI3BarrelSmallHitToT2016
    {this, "FEI3BarrelSmallHitToT2016", {-1,-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of barrel pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelTimingSimTune2016
    {this, "FEI3BarrelTimingSimTune2016", {-1,2015,2015,2015}, "FEI3 timing tuning set of barrel pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_BarrelCrossTalk2016
    {this, "BarrelCrossTalk2016", {0.30,0.06,0.06,0.06}, "Cross-talk fraction of barrel pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_BarrelNoiseOccupancy2016
    {this, "BarrelNoiseOccupancy2016", {5e-8,5e-8,5e-8,5e-8}, "Noise occupancy of barrel pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_BarrelDisableProbability2016
    {this, "BarrelDisableProbability2016", {9e-3,9e-3,9e-3,9e-3}, "Disable probability of barrel pixel layers in 2015/2016"};

// So far, Gaudi::Property does not support 2D vector.
//    Gaudi::Property<std::vector<std::vector<float>>> m_BarrelNoiseShape2016
//    {this, "BarrelNoiseShape", {{0.0,1.0},{0.0,1.0},{0.0,1.0},{0.0,1.0}}, "Noise shape of barrel pixel layers"};

    // This is ad-hoc solution.
    Gaudi::Property<std::vector<float>> m_IBLNoiseShape2016
    {this, "IBLNoiseShape2016", {0.0,1.0}, "Noise shape for IBL in 2015/2016"};

    Gaudi::Property<std::vector<float>> m_BLayerNoiseShape2016
    {this, "BLayerNoiseShape2016", {0.0,1.0}, "Noise shape for b-layer in 2015/2016"};

    Gaudi::Property<std::vector<float>> m_PixelNoiseShape2016
    {this, "PixelNoiseShape2016", {0.0,1.0}, "Noise shape for PIXEL in 2015/2016"};

    // Endcap RUN2 2015/2016
    Gaudi::Property<std::vector<int>> m_EndcapToTThreshold2016
    {this, "EndcapToTThreshold2016", { 5, 5, 5}, "ToT thresholds for endcap pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapLatency2016
    {this, "FEI3EndcapLatency2016", {256,256,256}, "FEI3 latency for endcap pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<bool>> m_FEI3EndcapHitDuplication2016
    {this, "FEI3EndcapHitDuplication2016", {false,false,false}, "Use FEI3 hit-duplicaiton for endcap pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<int>>  m_FEI3EndcapSmallHitToT2016
    {this, "FEI3EndcapSmallHitToT2016", {-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of endcap pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapTimingSimTune2016
    {this, "FEI3EndcapTimingSimTune2016", {2015,2015,2015}, "FEI3 timing tuning set of endcap pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_EndcapCrossTalk2016
    {this, "EndcapCrossTalk2016", {0.06,0.06,0.06}, "Cross-talk fraction of barrel endcap layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_EndcapNoiseOccupancy2016
    {this, "EndcapNoiseOccupancy2016", {5e-8,5e-8,5e-8}, "Noise occupancy of endcap pixel layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_EndcapDisableProbability2016
    {this, "EndcapDisableProbability2016", {9e-3,9e-3,9e-3}, "Disable probability of endcap pixel layers in 2015/2016"};

    // DBM RUN2 2015/2016
    Gaudi::Property<std::vector<int>> m_DBMToTThreshold2016
    {this, "DBMToTThreshold2016", {-1,-1,-1}, "ToT thresholds for DBM layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_DBMCrossTalk2016
    {this, "DBMCrossTalk2016", {0.06,0.06,0.06}, "Cross-talk fraction of barrel DBM layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_DBMNoiseOccupancy2016
    {this, "DBMNoiseOccupancy2016", {5e-8,5e-8,5e-8}, "Noise occupancy of DBM layers in 2015/2016"};

    Gaudi::Property<std::vector<double>> m_DBMDisableProbability2016
    {this, "DBMDisableProbability2016", {9e-3,9e-3,9e-3}, "Disable probability of DBM pixel layers in 2015/2016"};

    //====================================================================================
    // Barrel RUN2 2017
    Gaudi::Property<std::vector<int>> m_BarrelToTThreshold2017
    {this, "BarrelToTThreshold2017", {-1, 5, 5, 5}, "ToT thresholds for barrel pixel layers in 2017"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelLatency2017
    {this, "FEI3BarrelLatency2017", {  0,151,256,256}, "FEI3 latency for barrel pixel layers in 2017"};

    Gaudi::Property<std::vector<bool>> m_FEI3BarrelHitDuplication2017
    {this, "FEI3BarrelHitDuplication2017", {false,false,false,false}, "Use FEI3 hit-duplicaiton for barrel pixel layers in 2017"};

    Gaudi::Property<std::vector<int>>  m_FEI3BarrelSmallHitToT2017
    {this, "FEI3BarrelSmallHitToT2017", {-1,-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of barrel pixel layers in 2017"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelTimingSimTune2017
    {this, "FEI3BarrelTimingSimTune2017", {-1,2018,2018,2018}, "FEI3 timing tuning set of barrel pixel layers in 2017"};

    Gaudi::Property<std::vector<double>> m_BarrelCrossTalk2017
    {this, "BarrelCrossTalk2017", {0.30,0.06,0.06,0.06}, "Cross-talk fraction of barrel pixel layers in 2017"};

    Gaudi::Property<std::vector<double>> m_BarrelNoiseOccupancy2017
    {this, "BarrelNoiseOccupancy2017", {5e-8,5e-8,5e-8,5e-8}, "Noise occupancy of barrel pixel layers in 2017"};

    Gaudi::Property<std::vector<double>> m_BarrelDisableProbability2017
    {this, "BarrelDisableProbability2017", {9e-3,9e-3,9e-3,9e-3}, "Disable probability of barrel pixel layers in 2017"};

    // This is ad-hoc solution.
    Gaudi::Property<std::vector<float>> m_IBLNoiseShape2017
    {this, "IBLNoiseShape2017", {0.0,1.0}, "Noise shape for IBL in 2017"};

    Gaudi::Property<std::vector<float>> m_BLayerNoiseShape2017
    {this, "BLayerNoiseShape2017", {0.0,1.0}, "Noise shape for b-layer in 2017"};

    Gaudi::Property<std::vector<float>> m_PixelNoiseShape2017
    {this, "PixelNoiseShape2017", {0.0,1.0}, "Noise shape for PIXEL in 2017"};

    // Endcap RUN2 2017
    Gaudi::Property<std::vector<int>> m_EndcapToTThreshold2017
    {this, "EndcapToTThreshold2017", { 5, 5, 5}, "ToT thresholds for endcap pixel layers in 2017"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapLatency2017
    {this, "FEI3EndcapLatency2017", {256,256,256}, "FEI3 latency for endcap pixel layers in 2017"};

    Gaudi::Property<std::vector<bool>> m_FEI3EndcapHitDuplication2017
    {this, "FEI3EndcapHitDuplication2017", {false,false,false}, "Use FEI3 hit-duplicaiton for endcap pixel layers in 2017"};

    Gaudi::Property<std::vector<int>>  m_FEI3EndcapSmallHitToT2017
    {this, "FEI3EndcapSmallHitToT2017", {-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of endcap pixel layers in 2017"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapTimingSimTune2017
    {this, "FEI3EndcapTimingSimTune2017", {2018,2018,2018}, "FEI3 timing tuning set of endcap pixel layers in 2017"};

    Gaudi::Property<std::vector<double>> m_EndcapCrossTalk2017
    {this, "EndcapCrossTalk2017", {0.06,0.06,0.06}, "Cross-talk fraction of barrel endcap layers in 2017"};

    Gaudi::Property<std::vector<double>> m_EndcapNoiseOccupancy2017
    {this, "EndcapNoiseOccupancy2017", {5e-8,5e-8,5e-8}, "Noise occupancy of endcap pixel layers in 2017"};

    Gaudi::Property<std::vector<double>> m_EndcapDisableProbability2017
    {this, "EndcapDisableProbability2017", {9e-3,9e-3,9e-3}, "Disable probability of endcap pixel layers in 2017"};

    // DBM RUN2 2017
    Gaudi::Property<std::vector<int>> m_DBMToTThreshold2017
    {this, "DBMToTThreshold2017", {-1,-1,-1}, "ToT thresholds for DBM layers in 2017"};

    Gaudi::Property<std::vector<double>> m_DBMCrossTalk2017
    {this, "DBMCrossTalk2017", {0.06,0.06,0.06}, "Cross-talk fraction of barrel DBM layers in 2017"};

    Gaudi::Property<std::vector<double>> m_DBMNoiseOccupancy2017
    {this, "DBMNoiseOccupancy2017", {5e-8,5e-8,5e-8}, "Noise occupancy of DBM layers in 2017"};

    Gaudi::Property<std::vector<double>> m_DBMDisableProbability2017
    {this, "DBMDisableProbability2017", {9e-3,9e-3,9e-3}, "Disable probability of DBM pixel layers in 2017"};

    //====================================================================================
    // Barrel RUN2 2018
    Gaudi::Property<std::vector<int>> m_BarrelToTThreshold2018
    {this, "BarrelToTThreshold2018", {-1, 3, 5, 5}, "ToT thresholds for barrel pixel layers in 2018"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelLatency2018
    {this, "FEI3BarrelLatency2018", {  0,151,256,256}, "FEI3 latency for barrel pixel layers in 2018"};

    Gaudi::Property<std::vector<bool>> m_FEI3BarrelHitDuplication2018
    {this, "FEI3BarrelHitDuplication2018", {false,false,false,false}, "Use FEI3 hit-duplicaiton for barrel pixel layers in 2018"};

    Gaudi::Property<std::vector<int>>  m_FEI3BarrelSmallHitToT2018
    {this, "FEI3BarrelSmallHitToT2018", {-1,-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of barrel pixel layers in 2018"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelTimingSimTune2018
    {this, "FEI3BarrelTimingSimTune2018", {-1,2018,2018,2018}, "FEI3 timing tuning set of barrel pixel layers in 2018"};

    Gaudi::Property<std::vector<double>> m_BarrelCrossTalk2018
    {this, "BarrelCrossTalk2018", {0.30,0.06,0.06,0.06}, "Cross-talk fraction of barrel pixel layers in 2018"};

    Gaudi::Property<std::vector<double>> m_BarrelNoiseOccupancy2018
    {this, "BarrelNoiseOccupancy2018", {5e-8,5e-8,5e-8,5e-8}, "Noise occupancy of barrel pixel layers in 2018"};

    Gaudi::Property<std::vector<double>> m_BarrelDisableProbability2018
    {this, "BarrelDisableProbability2018", {9e-3,9e-3,9e-3,9e-3}, "Disable probability of barrel pixel layers in 2018"};

    // This is ad-hoc solution.
    Gaudi::Property<std::vector<float>> m_IBLNoiseShape2018
    {this, "IBLNoiseShape2018", {0.0,1.0}, "Noise shape for IBL in 2018"};

    Gaudi::Property<std::vector<float>> m_BLayerNoiseShape2018
    {this, "BLayerNoiseShape2018", {0.0,1.0}, "Noise shape for b-layer in 2018"};

    Gaudi::Property<std::vector<float>> m_PixelNoiseShape2018
    {this, "PixelNoiseShape2018", {0.0,1.0}, "Noise shape for PIXEL in 2018"};

    // Endcap RUN2 2018
    Gaudi::Property<std::vector<int>> m_EndcapToTThreshold2018
    {this, "EndcapToTThreshold2018", { 5, 5, 5}, "ToT thresholds for endcap pixel layers in 2018"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapLatency2018
    {this, "FEI3EndcapLatency2018", {256,256,256}, "FEI3 latency for endcap pixel layers in 2018"};

    Gaudi::Property<std::vector<bool>> m_FEI3EndcapHitDuplication2018
    {this, "FEI3EndcapHitDuplication2018", {false,false,false}, "Use FEI3 hit-duplicaiton for endcap pixel layers in 2018"};

    Gaudi::Property<std::vector<int>>  m_FEI3EndcapSmallHitToT2018
    {this, "FEI3EndcapSmallHitToT2018", {-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of endcap pixel layers in 2018"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapTimingSimTune2018
    {this, "FEI3EndcapTimingSimTune2018", {2018,2018,2018}, "FEI3 timing tuning set of endcap pixel layers in 2018"};

    Gaudi::Property<std::vector<double>> m_EndcapCrossTalk2018
    {this, "EndcapCrossTalk2018", {0.06,0.06,0.06}, "Cross-talk fraction of barrel endcap layers in 2018"};

    Gaudi::Property<std::vector<double>> m_EndcapNoiseOccupancy2018
    {this, "EndcapNoiseOccupancy2018", {5e-8,5e-8,5e-8}, "Noise occupancy of endcap pixel layers in 2018"};

    Gaudi::Property<std::vector<double>> m_EndcapDisableProbability2018
    {this, "EndcapDisableProbability2018", {9e-3,9e-3,9e-3}, "Disable probability of endcap pixel layers in 2018"};

    // DBM RUN2 2018
    Gaudi::Property<std::vector<int>> m_DBMToTThreshold2018
    {this, "DBMToTThreshold2018", {-1,-1,-1}, "ToT thresholds for DBM layers in 2018"};

    Gaudi::Property<std::vector<double>> m_DBMCrossTalk2018
    {this, "DBMCrossTalk2018", {0.06,0.06,0.06}, "Cross-talk fraction of barrel DBM layers in 2018"};

    Gaudi::Property<std::vector<double>> m_DBMNoiseOccupancy2018
    {this, "DBMNoiseOccupancy2018", {5e-8,5e-8,5e-8}, "Noise occupancy of DBM layers in 2018"};

    Gaudi::Property<std::vector<double>> m_DBMDisableProbability2018
    {this, "DBMDisableProbability2018", {9e-3,9e-3,9e-3}, "Disable probability of DBM pixel layers in 2018"};

    //====================================================================================
    // Barrel RUN1
    Gaudi::Property<std::vector<int>> m_BarrelToTThresholdRUN1
    {this, "BarrelToTThresholdRUN1", {3, 3, 3}, "ToT thresholds for barrel pixel layers in RUN1"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelLatencyRUN1
    {this, "FEI3BarrelLatencyRUN1", {256,256,256}, "FEI3 latency for barrel pixel layers in RUN1"};

    Gaudi::Property<std::vector<bool>> m_FEI3BarrelHitDuplicationRUN1
    {this, "FEI3BarrelHitDuplicationRUN1", {true,true,true}, "Use FEI3 hit-duplicaiton for barrel pixel layers in RUN1"};

    Gaudi::Property<std::vector<int>>  m_FEI3BarrelSmallHitToTRUN1
    {this, "FEI3BarrelSmallHitToTRUN1", {7,7,7}, "Small hit ToT thresold for FEI3 hit-duplication of barrel pixel layers in RUN1"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelTimingSimTuneRUN1
    {this, "FEI3BarrelTimingSimTuneRUN1", {2009,2009,2009}, "FEI3 timing tuning set of barrel pixel layers in RUN1"};

    Gaudi::Property<std::vector<double>> m_BarrelCrossTalkRUN1
    {this, "BarrelCrossTalkRUN1", {0.06,0.06,0.06}, "Cross-talk fraction of barrel pixel layers in RUN1"};

    Gaudi::Property<std::vector<double>> m_BarrelNoiseOccupancyRUN1
    {this, "BarrelNoiseOccupancyRUN1", {5e-8,5e-8,5e-8}, "Noise occupancy of barrel pixel layers in RUN1"};

    Gaudi::Property<std::vector<double>> m_BarrelDisableProbabilityRUN1
    {this, "BarrelDisableProbabilityRUN1", {9e-3,9e-3,9e-3}, "Disable probability of barrel pixel layers in RUN1"};

    // This is ad-hoc solution.
    Gaudi::Property<std::vector<float>> m_BLayerNoiseShapeRUN1
    {this, "BLayerNoiseShapeRUN1", {0.0,1.0}, "Noise shape for b-layer in RUN1"};

    Gaudi::Property<std::vector<float>> m_PixelNoiseShapeRUN1
    {this, "PixelNoiseShapeRUN1", {0.0,1.0}, "Noise shape for PIXEL in RUN1"};

    // Endcap RUN1
    Gaudi::Property<std::vector<int>> m_EndcapToTThresholdRUN1
    {this, "EndcapToTThresholdRUN1", { 3, 3, 3}, "ToT thresholds for endcap pixel layers in RUN1"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapLatencyRUN1
    {this, "FEI3EndcapLatencyRUN1", {256,256,256}, "FEI3 latency for endcap pixel layers in RUN1"};

    Gaudi::Property<std::vector<bool>> m_FEI3EndcapHitDuplicationRUN1
    {this, "FEI3EndcapHitDuplicationRUN1", {true,true,true}, "Use FEI3 hit-duplicaiton for endcap pixel layers in RUN1"};

    Gaudi::Property<std::vector<int>>  m_FEI3EndcapSmallHitToTRUN1
    {this, "FEI3EndcapSmallHitToTRUN1", {7,7,7}, "Small hit ToT thresold for FEI3 hit-duplication of endcap pixel layers in RUN1"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapTimingSimTuneRUN1
    {this, "FEI3EndcapTimingSimTuneRUN1", {2009,2009,2009}, "FEI3 timing tuning set of endcap pixel layers in RUN1"};

    Gaudi::Property<std::vector<double>> m_EndcapCrossTalkRUN1
    {this, "EndcapCrossTalkRUN1", {0.06,0.06,0.06}, "Cross-talk fraction of barrel endcap layers in RUN1"};

    Gaudi::Property<std::vector<double>> m_EndcapNoiseOccupancyRUN1
    {this, "EndcapNoiseOccupancyRUN1", {5e-8,5e-8,5e-8}, "Noise occupancy of endcap pixel layers in RUN1"};

    Gaudi::Property<std::vector<double>> m_EndcapDisableProbabilityRUN1
    {this, "EndcapDisableProbabilityRUN1", {9e-3,9e-3,9e-3}, "Disable probability of endcap pixel layers in RUN1"};

    //====================================================================================
    // Barrel ITK
    Gaudi::Property<std::vector<int>> m_BarrelToTThresholdITK
    {this, "BarrelToTThresholdITK", {3,3,3,3,3}, "ToT thresholds for barrel pixel layers in ITK"};

    Gaudi::Property<std::vector<double>> m_BarrelCrossTalkITK
    {this, "BarrelCrossTalkITK", {0.06,0.06,0.06,0.06,0.06}, "Cross-talk fraction of barrel pixel layers in ITK"};

    Gaudi::Property<std::vector<double>> m_BarrelNoiseOccupancyITK
    {this, "BarrelNoiseOccupancyITK", {5e-8,5e-8,5e-8,5e-8,5e-8}, "Noise occupancy of barrel pixel layers in ITK"};

    Gaudi::Property<std::vector<double>> m_BarrelDisableProbabilityITK
    {this, "BarrelDisableProbabilityITK", {9e-3,9e-3,9e-3,9e-3,9e-3}, "Disable probability of barrel pixel layers in ITK"};

    // This is ad-hoc solution.
    Gaudi::Property<std::vector<float>> m_InnermostNoiseShapeITK
    {this, "InnermostNoiseShapeITK", {0.0,1.0}, "Noise shape for IBL in ITK"};

    Gaudi::Property<std::vector<float>> m_NextInnermostNoiseShapeITK
    {this, "NextInnermostNoiseShapeITK", {0.0,1.0}, "Noise shape for b-layer in ITK"};

    Gaudi::Property<std::vector<float>> m_PixelNoiseShapeITK
    {this, "PixelNoiseShapeITK", {0.0,1.0}, "Noise shape for PIXEL in ITK"};

    // Endcap ITK
    Gaudi::Property<std::vector<int>> m_EndcapToTThresholdITK
    {this, "EndcapToTThresholdITK", {3,3,3,3,3,3,3,3,3,3,3,3,3,3}, "ToT thresholds for endcap pixel layers in ITK"};

    Gaudi::Property<std::vector<double>> m_EndcapCrossTalkITK
    {this, "EndcapCrossTalkITK", {0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06}, "Cross-talk fraction of barrel endcap layers in ITK"};

    Gaudi::Property<std::vector<double>> m_EndcapNoiseOccupancyITK
    {this, "EndcapNoiseOccupancyITK", {5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8,5e-8}, "Noise occupancy of endcap pixel layers in ITK"};

    Gaudi::Property<std::vector<double>> m_EndcapDisableProbabilityITK
    {this, "EndcapDisableProbabilityITK", {9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3,9e-3}, "Disable probability of endcap pixel layers in ITK"};

    //====================================================================================
    // The following parameters are default values which will be overwritten by the one 
    // from the conditions DB. Otherwise the DB is not retrieved nor available, these 
    // values are used.
    //====================================================================================
    Gaudi::Property<std::vector<int>> m_BarrelAnalogThreshold
    {this, "DefaultBarrelAnalogThreshold", {-1,-1,-1,-1}, "Default analog thresholds of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_EndcapAnalogThreshold
    {this, "DefaultEndcapAnalogThreshold", {-1,-1,-1}, "Default analog thresholds of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMAnalogThreshold
    {this, "DefaultDBMAnalogThreshold", {-1,-1,-1}, "Default analog thresholds of DBMlayers"};

    Gaudi::Property<std::vector<int>> m_BarrelAnalogThresholdSigma
    {this, "DefaultBarrelAnalogThresholdSigma", {45,35,30,30}, "Default analog threshold sigma of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_EndcapAnalogThresholdSigma
    {this, "DefaultEndcapAnalogThresholdSigma", {30,30,30}, "Default analog threshold sigma of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMAnalogThresholdSigma
    {this, "DefaultDBMAnalogThresholdSigma", {70,70,70}, "Default analog threshold sigma of DBM pixel layers"};

    Gaudi::Property<std::vector<int>> m_BarrelAnalogThresholdNoise
    {this, "DefaultBarrelAnalogThresholdNoise", {130,150,160,160}, "Default threshold noise of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_EndcapAnalogThresholdNoise
    {this, "DefaultEndcapAnalogThresholdNoise", {150,150,150}, "Default threshold noise of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMAnalogThresholdNoise
    {this, "DefaultDBMAnalogThresholdNoise", {190,190,190}, "Default threshold noise of DBM pixel layers"};

    Gaudi::Property<std::vector<int>> m_BarrelInTimeThreshold
    {this, "DefaultBarrelInTimeThreshold", {2000,5000,5000,5000}, "Default in-time thresholds of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_EndcapInTimeThreshold
    {this, "DefaultEndcapInTimeThreshold", {5000,5000,5000}, "Default in-time thresholds of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMInTimeThreshold
    {this, "DefaultDBMInTimeThreshold", {1200,1200,1200}, "Default in-time thresholds of DBM pixel layers"};

    Gaudi::Property<std::vector<double>> m_BarrelThermalNoise
    {this, "BarrelThermalNoise", {160.0,160.0,160.0,160.0}, "Thermal noise of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapThermalNoise
    {this, "EndcapThermalNoise", {160.0,160.0,160.0}, "Thermal noise of endcap pixel layers"};

    Gaudi::Property<std::vector<double>> m_DBMThermalNoise
    {this, "DBMThermalNoise", {160.0,160.0,160.0}, "Thermal noise of DBM layers"};

    Gaudi::Property<std::vector<int>> m_FEI4BarrelHitDiscConfig
    {this, "FEI4BarrelHitDiscConfig", {2}, "Set HitDiscConfig parameter for barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI4EndcapHitDiscConfig
    {this, "FEI4EndcapHitDiscConfig", {2}, "Set HitDiscConfig parameter for endcap pixel layers"};

    // Charge calibration parameters
    Gaudi::Property<float> m_CalibrationParameterA
    {this, "DefaultCalibrationParameterA", 70.2, "Default charge calibration parameter A"};

    Gaudi::Property<float> m_CalibrationParameterE
    {this, "DefaultCalibrationParameterE", -3561.25, "Default charge calibration parameter E"};

    Gaudi::Property<float> m_CalibrationParameterC
    {this, "DefaultCalibrationParameterC", 26000.0, "Default charge calibration parameter C"};
    
    // DCS parameters
    Gaudi::Property<float> m_biasVoltage
    {this, "DefaultBiasVoltage", 150.0, "Default bias voltage"};

    Gaudi::Property<float> m_temperature
    {this, "DefaultTemperature", -7.0, "Default temperature in Celcius"};

    // Distortion parameters
    /** @brief Flag controlling how module distortions are taken into account:
      case 0 -----> No distorsions implemented;
      case 1 -----> Set curvature (in 1/meter) and twist (in radiant) equal for all modules;
      case 2 -----> Read curvatures and twists from textfile containing Survey data;
      case 3 -----> Set curvature and twist from Gaussian random generator with mean and RMS coming from Survey data;
      case 4 -----> Read curvatures and twists from database;
     */
    Gaudi::Property<int> m_distortionInputSource
    {this, "DistortionInputSource", 4, "Source of module distortions: 0 (none), 1 (constant), 2 (text file), 3 (random), 4 (database)"};

    Gaudi::Property<int> m_distortionVersion
    {this, "DistortionVersion", -1, "Version number for distortion model"};

    Gaudi::Property<double> m_distortionR1
    {this, "DistortionR1", 0.1/CLHEP::meter, "Fixed distortion parameters: radius of curvature"}; //corresponding to a sagitta of 50 um
    
    Gaudi::Property<double> m_distortionR2
    {this, "DistortionR2", 0.1/CLHEP::meter, "Fixed distortion parameters: radius of curvature"}; //corresponding to a sagitta of 50 um

    Gaudi::Property<double> m_distortionTwist
    {this, "DistortionTwist", 0.0005,"Fixed distortion parameters: twist angle (tan(theta))"}; 

    Gaudi::Property<double> m_distortionMeanR
    {this, "DistortionMean_R", 0.12/CLHEP::meter, "Random distortion parameters: Mean of radius of curvature"}; //Mean value from Survey
    
    Gaudi::Property<double> m_distortionRMSR
    {this, "DistortionRMS_R", 0.08/CLHEP::meter, "Random distortion parameters: RMS of curvature radius"}; //RMS value from Survey

    Gaudi::Property<double> m_distortionMeanTwist
    {this, "DistortionMean_twist", -0.0005,"Random distortion parameters: Mean twist angle"}; //Mean value from Survey

    Gaudi::Property<double> m_distortionRMSTwist
    {this, "DistortionRMS_twist", 0.0008,"Random distortion parameters: RMS of twist angle"}; //RMS value from Survey

    Gaudi::Property<bool> m_distortionWriteToFile
    {this, "DistortionWriteToFile", false, "Record data in storegate"}; 

    Gaudi::Property<std::string> m_distortionFileName
    {this, "DistortionFileName", "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/TrackingCP/PixelDistortions/PixelDistortionsData_v2_BB.txt","Read distortions from this file"};

    // Cabling parameters
    Gaudi::Property<bool> m_cablingMapToFile
    {this, "CablingMapToFile", false, "Dump pixel cabling map into file"};

    Gaudi::Property<std::string> m_cablingMapFileName
    {this, "CablingMapFileName", "PixelCabling/Pixels_Atlas_IdMapping_2016.dat", "Read cabling map from file"};

    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
};

#endif

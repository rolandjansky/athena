/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

class PixelConfigCondAlg : public AthReentrantAlgorithm {
  public:
    PixelConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    // Switch parameters
    Gaudi::Property<bool> m_useCalibConditions
    {this, "UseCalibConditions", true, "Use conditions DB for charge calibration"};

    Gaudi::Property<bool> m_useDeadmapConditions
    {this, "UseDeadmapConditions", true, "Use conditions DB for pixel deadmap module/FE"};

    Gaudi::Property<bool> m_useDCSStateConditions
    {this, "UseDCSStateConditions", false, "Use DCSState conditions DB"};

    Gaudi::Property<bool> m_useDCSStatusConditions
    {this, "UseDCSStatusConditions", false, "Use DCSStatus conditions DB"};

    Gaudi::Property<bool> m_useDCSHVConditions
    {this, "UseDCSHVConditions", true, "Use DCS HV conditions DB"};

    Gaudi::Property<bool> m_useDCSTemperatureConditions
    {this, "UseDCSTemperatureConditions", true, "Use DCS temperature conditions DB"};

    Gaudi::Property<bool> m_useTDAQConditions
    {this, "UseTDAQConditions", false, "Use TDAQ conditions DB (NOTE only valid in RUN-1)"};

    Gaudi::Property<bool> m_useCablingConditions
    {this, "UseCablingConditions", true, "Use cabling map conditions DB"};

    // Digitization parameters
    Gaudi::Property<double> m_bunchSpace
    {this, "BunchSpace", 25.0, "Bunch space [ns]"};

    Gaudi::Property<bool> m_UseComTime
    {this, "UseComTime", false, "Use computed time used in cosmic/commissioning timing"};

    double m_ComTime{0.0};

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

    Gaudi::Property<std::vector<int>> m_BarrelAnalogThreshold
    {this, "DefaultBarrelAnalogThreshold", {2000,4300,3500,3500}, "Default analog thresholds of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_EndcapAnalogThreshold
    {this, "DefaultEndcapAnalogThreshold", {3500,3500,3500}, "Default analog thresholds of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMAnalogThreshold
    {this, "DefaultDBMAnalogThreshold", {2000,2000,2000}, "Default analog thresholds of DBMlayers"};

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

    Gaudi::Property<std::vector<int>> m_BarrelToTThreshold
    {this, "BarrelToTThreshold", {-1, 3, 5, 5}, "Default ToT thresholds for barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_EndcapToTThreshold
    {this, "EndcapToTThreshold", { 5, 5, 5}, "Default ToT thresholds for endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMToTThreshold
    {this, "DBMToTThreshold", {-1,-1,-1}, "Default ToT thresholds for DBM layers"};

    Gaudi::Property<std::vector<double>> m_BarrelCrossTalk
    {this, "BarrelCrossTalk", {0.06,0.06,0.06,0.06}, "Cross-talk fraction of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapCrossTalk
    {this, "EndcapCrossTalk", {0.06,0.06,0.06}, "Cross-talk fraction of barrel endcap layers"};

    Gaudi::Property<std::vector<double>> m_DBMCrossTalk
    {this, "DBMCrossTalk", {0.06,0.06,0.06}, "Cross-talk fraction of barrel DBM layers"};

    Gaudi::Property<std::vector<double>> m_BarrelThermalNoise
    {this, "BarrelThermalNoise", {160.0,160.0,160.0,160.0}, "Thermal noise of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapThermalNoise
    {this, "EndcapThermalNoise", {160.0,160.0,160.0}, "Thermal noise of endcap pixel layers"};

    Gaudi::Property<std::vector<double>> m_DBMThermalNoise
    {this, "DBMThermalNoise", {160.0,160.0,160.0}, "Thermal noise of DBM layers"};

    Gaudi::Property<std::vector<double>> m_BarrelNoiseOccupancy
    {this, "BarrelNoiseOccupancy", {5e-8,5e-8,5e-8,5e-8}, "Noise occupancy of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapNoiseOccupancy
    {this, "EndcapNoiseOccupancy", {5e-8,5e-8,5e-8}, "Noise occupancy of endcap pixel layers"};

    Gaudi::Property<std::vector<double>> m_DBMNoiseOccupancy
    {this, "DBMNoiseOccupancy", {5e-8,5e-8,5e-8}, "Noise occupancy of DBM layers"};

    Gaudi::Property<std::vector<double>> m_BarrelDisableProbability
    {this, "BarrelDisableProbability", {9e-3,9e-3,9e-3,9e-3}, "Disable probability of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapDisableProbability
    {this, "EndcapDisableProbability", {9e-3,9e-3,9e-3}, "Disable probability of endcap pixel layers"};

    Gaudi::Property<std::vector<double>> m_DBMDisableProbability
    {this, "DBMDisableProbability", {9e-3,9e-3,9e-3}, "Disable probability of DBM pixel layers"};

//
// So far, Gaudi::Property does not support 2D vector.
//
//    Gaudi::Property<std::vector<std::vector<float>>> m_BarrelNoiseShape
//    {this, "BarrelNoiseShape", {{0.0,1.0},{0.0,1.0},{0.0,1.0},{0.0,1.0}}, "Noise shape of barrel pixel layers"};

//    Gaudi::Property<std::vector<std::vector<float>>> m_EndcapNoiseShape
//    {this, "EndcapNoiseShape", {{0.0,1.0},{0.0,1.0},{0.0,1.0}}, "Noise shape of endcap pixel layers"};

//    Gaudi::Property<std::vector<std::vector<float>>> m_DBMNoiseShape
//    {this, "DBMNoiseShape", {{0.0,1.0},{0.0,1.0},{0.0,1.0}}, "Noise shape of DBM pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelLatency
    {this, "FEI3BarrelLatency", {  0,151,256,256}, "FEI3 latency for barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapLatency
    {this, "FEI3EndcapLatency", {256,256,256}, "FEI3 latency for endcap pixel layers"};

    Gaudi::Property<std::vector<bool>> m_FEI3BarrelHitDuplication
    {this, "FEI3BarrelHitDuplication", {false,false,false,false}, "Use FEI3 hit-duplicaiton for barrel pixel layers"};

    Gaudi::Property<std::vector<bool>> m_FEI3EndcapHitDuplication
    {this, "FEI3EndcapHitDuplication", {false,false,false}, "Use FEI3 hit-duplicaiton for endcap pixel layers"};

    Gaudi::Property<std::vector<int>>  m_FEI3BarrelSmallHitToT
    {this, "FEI3BarrelSmallHitToT", {-1,-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of barrel pixel layers"};

    Gaudi::Property<std::vector<int>>  m_FEI3EndcapSmallHitToT
    {this, "FEI3EndcapSmallHitToT", {-1,-1,-1}, "Small hit ToT thresold for FEI3 hit-duplication of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelTimingSimTune
    {this, "FEI3BarrelTimingSimTune", {2015,2015,2015,2015}, "FEI3 timing tuning set of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapTimingSimTune
    {this, "FEI3EndcapTimingSimTune", {2015,2015,2015}, "FEI3 timing tuning set of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI4BarrelHitDiscConfig
    {this, "FEI4BarrelHitDiscConfig", {2}, "Set HitDiscConfig parameter for barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI4EndcapHitDiscConfig
    {this, "FEI4EndcapHitDiscConfig", {2}, "Set HitDiscConfig parameter for endcap pixel layers"};

    std::vector<float> m_IBLNoiseShape;      // This is ad-hoc solution.
    std::vector<float> m_BLayerNoiseShape;
    std::vector<float> m_PixelNoiseShape;

    SG::ReadHandleKey<ComTime> m_ComTimeKey
    {this, "ComTimeKey", "ComTime", "Commissioning time for cosmic"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readDeadMapKey
    {this, "ReadDeadMapKey", "/PIXEL/PixMapOverlay", "Input key of deadmap conditions folder"};

    SG::WriteCondHandleKey<PixelModuleData> m_writeKey
    {this, "WriteKey", "PixelModuleData", "Output key of pixel module data"};

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

    // Cabling parameters
    Gaudi::Property<bool> m_cablingMapToFile
    {this, "CablingMapToFile", false, "Dump pixel cabling map into file"};

    Gaudi::Property<std::string> m_cablingMapFileName
    {this, "CablingMapFileName", "PixelCabling/Pixels_Atlas_IdMapping_2016.dat", "Read cabling map from file"};

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
    {this, "DistortionVersion", 1, "Version number for distortion model"};

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
    {this, "DistortionFileName", "PixelSurveyDistortions.txt","Read distortions from this file"}; 

    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
};

#endif

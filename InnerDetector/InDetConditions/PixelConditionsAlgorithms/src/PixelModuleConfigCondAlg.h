/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELMODULECONFIGCONDALG
#define PIXELMODULECONFIGCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

class PixelModuleConfigCondAlg : public AthReentrantAlgorithm {
  public:
    PixelModuleConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    // Key for basic pixel parameters
    SG::WriteCondHandleKey<PixelModuleData> m_writeKey
    {this, "WriteKey", "PixelModuleData", "Output key of pixel module data"};

    // Digitization parameters
    Gaudi::Property<double> m_bunchSpace
    {this, "BunchSpace", 25.0, "Bunch space [ns]"};

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

    Gaudi::Property<std::vector<int>> m_BarrelToTThreshold
    {this, "BarrelToTThreshold", {}, "ToT thresholds for barrel pixel layers"};

    Gaudi::Property<std::vector<float>> m_BarrelBiasVoltage
    {this, "DefaultBarrelBiasVoltage", {}, "Default barrel bias voltage"};

    Gaudi::Property<std::vector<double>> m_BarrelFluence
    {this, "BarrelFluence", {}, "Barrel fluence for radiation damage"};

    Gaudi::Property<std::vector<std::string>> m_BarrelFluenceMap
    {this, "BarrelFluenceMap", {},
                                   "Barrel fluence map for radiation damage"};

    Gaudi::Property<std::vector<double>> m_BarrelCrossTalk
    {this, "BarrelCrossTalk", {}, "Cross-talk fraction of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_BarrelNoiseOccupancy
    {this, "BarrelNoiseOccupancy", {}, "Noise occupancy of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_BarrelDisableProbability
    {this, "BarrelDisableProbability", {}, "Disable probability of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_BarrelLorentzAngleCorr
    {this, "BarrelLorentzAngleCorr", {}, "Scale factor for Lorentz angle of barrel pixel layers"};

    // This is ad-hoc solution.
    Gaudi::Property<std::vector<float>> m_InnermostNoiseShape
    {this, "InnermostNoiseShape", {}, "Noise shape for L0"};

    Gaudi::Property<std::vector<float>> m_NextInnermostNoiseShape
    {this, "NextInnermostNoiseShape", {}, "Noise shape for L1"};

    Gaudi::Property<std::vector<float>> m_PixelNoiseShape
    {this, "PixelNoiseShape", {}, "Noise shape for PIXEL"};

    // Endcap ITK
    Gaudi::Property<std::vector<int>> m_EndcapToTThreshold
    {this, "EndcapToTThreshold", {}, "ToT thresholds for endcap pixel layers"};

    Gaudi::Property<std::vector<float>> m_EndcapBiasVoltage
    {this, "DefaultEndcapBiasVoltage", {}, "Default endcap bias voltage"};

    Gaudi::Property<std::vector<double>> m_EndcapCrossTalk
    {this, "EndcapCrossTalk", {}, "Cross-talk fraction of barrel endcap layers"};

    Gaudi::Property<std::vector<double>> m_EndcapNoiseOccupancy
    {this, "EndcapNoiseOccupancy", {}, "Noise occupancy of endcap pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapDisableProbability
    {this, "EndcapDisableProbability", {}, "Disable probability of endcap pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapLorentzAngleCorr
    {this, "EndcapLorentzAngleCorr", {}, "Scale factor for Lorentz angle of endcap pixel layers"};

    // 3D
    Gaudi::Property<std::vector<double>> m_3DFluence
    {this, "Barrel3DFluence", {}, "Barrel3D fluence for radiation damage"};

    Gaudi::Property<std::vector<std::string>> m_3DFluenceMap
    {this, "Barrel3DFluenceMap", {},
                                     "Barrel3D fluence map for radiation damage"};

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

    Gaudi::Property<std::vector<int>> m_FEI4BarrelHitDiscConfig
    {this, "FEI4BarrelHitDiscConfig", {2}, "Set HitDiscConfig parameter for barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI4EndcapHitDiscConfig
    {this, "FEI4EndcapHitDiscConfig", {2}, "Set HitDiscConfig parameter for endcap pixel layers"};

    Gaudi::Property<float> m_chargeScaleFEI4
    {this, "ChargeScaleFEI4", 1.0, "Scaling of the FEI4 charge"};

    Gaudi::Property<bool> m_UseFEI4SpecialScalingFunction
    {this, "UseFEI4SpecialScalingFunction", true, "Use FEI4 special scaling function"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelLatency
    {this, "FEI3BarrelLatency", { }, "FEI3 latency for barrel pixel layers"};

    Gaudi::Property<std::vector<bool>> m_FEI3BarrelHitDuplication
    {this, "FEI3BarrelHitDuplication", { }, "Use FEI3 hit-duplicaiton for barrel pixel layers"};

    Gaudi::Property<std::vector<int>>  m_FEI3BarrelSmallHitToT
    {this, "FEI3BarrelSmallHitToT", { }, "Small hit ToT thresold for FEI3 hit-duplication of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI3BarrelTimingSimTune
    {this, "FEI3BarrelTimingSimTune", { }, "FEI3 timing tuning set of barrel pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapLatency
    {this, "FEI3EndcapLatency", { }, "FEI3 latency for endcap pixel layers"};

    Gaudi::Property<std::vector<bool>> m_FEI3EndcapHitDuplication
    {this, "FEI3EndcapHitDuplication", { }, "Use FEI3 hit-duplicaiton for endcap pixel layers"};

    Gaudi::Property<std::vector<int>>  m_FEI3EndcapSmallHitToT
    {this, "FEI3EndcapSmallHitToT", { }, "Small hit ToT thresold for FEI3 hit-duplication of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_FEI3EndcapTimingSimTune
    {this, "FEI3EndcapTimingSimTune", { }, "FEI3 timing tuning set of endcap pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMToTThreshold
    {this, "DBMToTThreshold", { }, "ToT thresholds for DBM layers"};

    Gaudi::Property<std::vector<float>> m_DBMBiasVoltage
    {this, "DefaultDBMBiasVoltage", { }, "Default DBM bias voltage"};

    Gaudi::Property<std::vector<double>> m_DBMCrossTalk
    {this, "DBMCrossTalk", { }, "Cross-talk fraction of barrel DBM layers"};

    Gaudi::Property<std::vector<double>> m_DBMNoiseOccupancy
    {this, "DBMNoiseOccupancy", { }, "Noise occupancy of DBM layers"};

    Gaudi::Property<std::vector<double>> m_DBMDisableProbability
    {this, "DBMDisableProbability", { }, "Disable probability of DBM pixel layers"};

    Gaudi::Property<std::vector<int>> m_DBMInTimeThreshold
    {this, "DefaultDBMInTimeThreshold", { }, "Default in-time thresholds of DBM pixel layers"};

    Gaudi::Property<std::vector<double>> m_BarrelThermalNoise
    {this, "BarrelThermalNoise", { }, "Thermal noise of barrel pixel layers"};

    Gaudi::Property<std::vector<double>> m_EndcapThermalNoise
    {this, "EndcapThermalNoise", { }, "Thermal noise of endcap pixel layers"};

    Gaudi::Property<std::vector<double>> m_DBMThermalNoise
    {this, "DBMThermalNoise", { }, "Thermal noise of DBM layers"};


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

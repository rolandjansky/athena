/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

class PixelConfigCondAlg : public AthReentrantAlgorithm {
  public:
    PixelConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    double m_bunchSpace;
    bool m_UseComTime;
    double m_ComTime;
    std::vector<int> m_BarrelNumberOfBCID;
    std::vector<int> m_EndcapNumberOfBCID;
    std::vector<int> m_DBMNumberOfBCID;
    std::vector<double> m_BarrelTimeOffset;
    std::vector<double> m_EndcapTimeOffset;
    std::vector<double> m_DBMTimeOffset;
    std::vector<double> m_BarrelTimeJitter;
    std::vector<double> m_EndcapTimeJitter;
    std::vector<double> m_DBMTimeJitter;
    bool m_useCalibConditions;
    std::vector<int> m_BarrelAnalogThreshold;
    std::vector<int> m_EndcapAnalogThreshold;
    std::vector<int> m_DBMAnalogThreshold;
    std::vector<int> m_BarrelAnalogThresholdSigma;
    std::vector<int> m_EndcapAnalogThresholdSigma;
    std::vector<int> m_DBMAnalogThresholdSigma;
    std::vector<int> m_BarrelAnalogThresholdNoise;
    std::vector<int> m_EndcapAnalogThresholdNoise;
    std::vector<int> m_DBMAnalogThresholdNoise;
    std::vector<int> m_BarrelInTimeThreshold;
    std::vector<int> m_EndcapInTimeThreshold;
    std::vector<int> m_DBMInTimeThreshold;
    float m_CalibrationParameterA;
    float m_CalibrationParameterE;
    float m_CalibrationParameterC;
    std::vector<int> m_BarrelToTThreshold;
    std::vector<int> m_EndcapToTThreshold;
    std::vector<int> m_DBMToTThreshold;
    std::vector<double> m_BarrelCrossTalk; 
    std::vector<double> m_EndcapCrossTalk; 
    std::vector<double> m_DBMCrossTalk; 
    std::vector<double> m_BarrelThermalNoise;
    std::vector<double> m_EndcapThermalNoise;
    std::vector<double> m_DBMThermalNoise;
    std::vector<double> m_BarrelNoiseOccupancy;
    std::vector<double> m_EndcapNoiseOccupancy;
    std::vector<double> m_DBMNoiseOccupancy;
    std::vector<double> m_BarrelDisableProbability;
    std::vector<double> m_EndcapDisableProbability;
    std::vector<double> m_DBMDisableProbability;
    std::vector<std::vector<float>> m_BarrelNoiseShape;
    std::vector<std::vector<float>> m_EndcapNoiseShape;
    std::vector<std::vector<float>> m_DBMNoiseShape;
    std::vector<float> m_IBLNoiseShape;      // This is ad-hoc solution.
    std::vector<float> m_BLayerNoiseShape;
    std::vector<float> m_PixelNoiseShape;
    std::vector<int> m_FEI3BarrelLatency;
    std::vector<int> m_FEI3EndcapLatency;
    std::vector<bool> m_FEI3BarrelHitDuplication;
    std::vector<bool> m_FEI3EndcapHitDuplication;
    std::vector<int>  m_FEI3BarrelSmallHitToT;
    std::vector<int>  m_FEI3EndcapSmallHitToT;
    std::vector<int> m_FEI3BarrelTimingSimTune;
    std::vector<int> m_FEI3EndcapTimingSimTune;
    std::vector<int> m_FEI4BarrelHitDiscConfig;
    std::vector<int> m_FEI4EndcapHitDiscConfig;

    bool m_useDeadMap;

    SG::ReadHandleKey<ComTime> m_ComTimeKey
    {this, "ComTimeKey", "ComTime", "Commissioning time for cosmic"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readDeadMapKey
    {this, "ReadDeadMapKey", "/PIXEL/PixMapOverlay", "Input key of deadmap conditions folder"};

    SG::WriteCondHandleKey<PixelModuleData> m_writeKey
    {this, "WriteKey", "PixelModuleData", "Output key of pixel module data"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif // PIXELCONFIGCONDALG

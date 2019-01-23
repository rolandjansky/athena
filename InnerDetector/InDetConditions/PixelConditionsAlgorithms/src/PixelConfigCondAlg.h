/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELCONFIGCONDALG
#define PIXELCONFIGCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

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
    std::vector<int> m_BarrelAnalogThreshold;
    std::vector<int> m_EndcapAnalogThreshold;
    std::vector<int> m_DBMAnalogThreshold;
    std::vector<int> m_BarrelToTThreshold;
    std::vector<int> m_EndcapToTThreshold;
    std::vector<int> m_DBMToTThreshold;
    std::vector<int> m_BarrelLatency;
    std::vector<int> m_EndcapLatency;
    std::vector<int> m_DBMLatency;
    std::vector<double> m_BarrelCrossTalk; 
    std::vector<double> m_EndcapCrossTalk; 
    std::vector<double> m_DBMCrossTalk; 
    std::vector<double> m_BarrelThermalNoise;
    std::vector<double> m_EndcapThermalNoise;
    std::vector<double> m_DBMThermalNoise;
    std::vector<bool> m_BarrelHitDuplication;
    std::vector<bool> m_EndcapHitDuplication;
    std::vector<bool> m_DBMHitDuplication;
    std::vector<int>  m_BarrelSmallHitToT;
    std::vector<int>  m_EndcapSmallHitToT;
    std::vector<int>  m_DBMSmallHitToT;
    int m_IBLHitDisConfig;

    bool m_useDeadMap;

    SG::ReadCondHandleKey<CondAttrListCollection> m_readDeadMapKey
    {this, "ReadDeadMapKey", "/PIXEL/PixMapOverlay", "Input key of deadmap conditions folder"};

    SG::WriteCondHandleKey<PixelModuleData> m_writeKey
    {this, "WriteKey", "PixelModuleData", "Output key of pixel module data"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif // PIXELCONFIGCONDALG

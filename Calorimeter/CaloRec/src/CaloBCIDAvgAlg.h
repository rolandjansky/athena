//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOBCIDAVGALG_H
#define CALOREC_CALOBCIDAVGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArMinBiasAverage.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

class CaloBCIDAvgAlg : public AthReentrantAlgorithm {
public:

  // constructor 
  CaloBCIDAvgAlg(const std::string& name, ISvcLocator* pSvcLocator);

  // Algorithm virtual methods 
  StatusCode initialize();
  StatusCode execute(const EventContext& ctx) const;
  StatusCode finalize();

private:
  //Event input: Only the BCID from Event Info
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","SG Key of EventInfo object"};

  //Event output: CaloBCIDAverage object
  SG::WriteHandleKey<CaloBCIDAverage> m_bcidAvgKey{this,"WriteKey","CaloBCIDAverage","SG Key of resulting CaloBCIDAverage object"};

  //ConditionsInput
  SG::ReadCondHandleKey<ILArOFC> m_ofcKey{this,"OFCKey","LArOFC","SG Key of OFC conditions object"};
  SG::ReadCondHandleKey<ILArShape> m_shapeKey{this,"ShapeKey","LArShape32","SG Key of Shape conditions object"};
  SG::ReadCondHandleKey<ILArMinBiasAverage> m_minBiasAvgKey{this,"MinBiasAvgKey","LArPileupAverage","SGKey of LArMinBiasAverage object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArMCSym> m_mcSym{this,"MCSym","LArMCSym","SG Key of LArMCSym object"};
  SG::ReadCondHandleKey<LuminosityCondData> m_lumiDataKey{this,"LuminosityCondDataKey","LuminosityCondData","SG Key of LuminosityCondData object"};


  //Tool Handles:
  ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;

  //Other Properties
  Gaudi::Property<bool> m_isMC{this,"isMC",false,"Real data or MC"};
  //Gaudi::Property<unsigned> m_firstSampleEMB{this,"firstSampleEMB",0,"First sample EMB in 4 samples mode"};
  //Gaudi::Property<unsigned> m_firstSampleEMEC{this,"firstSampleEMEC",0,"First sample EMEC in 4 samples mode"};
  Gaudi::Property<unsigned> m_firstSampleHEC{this,"firstSampleHEC",1,"First sample HEC in 4 samples mode"};
  //Gaudi::Property<unsigned> m_firstSampleFCAL{this,"firstSampleFCAL",0,"First sample FCAL in 4 samples mode"};


  const LArOnlineID* m_lar_on_id=nullptr;

  //Constants: 
  const unsigned m_bcidMax=3564;

  //private methods: 
  std::vector<float> accumulateLumi(const std::vector<float>& luminosity,
                                    const unsigned int bcid,
                                    const float xlumiMC) const;

};

#endif

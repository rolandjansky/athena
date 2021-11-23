/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArShapeCompleteMaker
 * @author Nicolas.Berger@cern.ch
 *   */

#ifndef LArShapeCompleteMakerAlg_H
#define LArShapeCompleteMakerAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArCafJobs/ILArShapeDumperTool.h"
#include "LArCafJobs/DataStore.h"
#include "LArSamplesMon/TreeShapeErrorGetter.h"
#include "StoreGate/ReadCondHandleKey.h"
#include <string>

class MsgStream;
class StoreGateSvc;
class ILArShape;
class HWIdentifier;
class Identifier;
class LArOnlineID;

class LArShapeCompleteMakerAlg : public AthAlgorithm
{
 public:
  LArShapeCompleteMakerAlg(const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~LArShapeCompleteMakerAlg();

  //standart algorithm methods
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

  //virtual StatusCode fillHistograms(const EventContext& ctx) const override final;
  
 private:
  
  const LArOnlineID* m_onlineHelper = nullptr;  //good

  LArSamples::DataStore* m_template;

  ToolHandle<ILArShapeDumperTool> m_dumperTool;
  
  unsigned int m_nSamples = 0U;
  unsigned int m_nPhases  = 0U;
  unsigned int m_iPhase   = 0U;
  //unsigned int m_nSamples, m_nPhases, m_iPhase;
  //
  Gaudi::Property<int> m_minNPulses{this,"minNPulses",0}; //default
  Gaudi::Property<std::string> m_shapeErrorFileName{this,"shapeErrorFileName", {} };
  Gaudi::Property<std::string> m_outputTemplateFileName{this,"outputTemplateFileName", {} };
  Gaudi::Property<std::string> m_outputFileName{this,"outputFileName", {} };
  Gaudi::Property<std::string> m_sgKey{this,"sgKey", {} };
  Gaudi::Property<std::string> m_groupingType{this,"groupingType", {} };
  //int m_minNPulses;
  //std::string m_shapeErrorFileName, m_outputFileName, m_outputTemplateFileName, m_sgKey;
  //std::string m_groupingType;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_onOffMapKey
    { this, "LArOnOffIdMappingKey", "LArOnOffIdMapping", "LArOnOffIdMapping key" };
};

#endif

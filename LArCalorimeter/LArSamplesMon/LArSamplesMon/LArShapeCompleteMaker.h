/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArShapeCompleteMaker
 * @author Nicolas.Berger@cern.ch
 *   */

#ifndef LArShapeCompleteMaker_H
#define LArShapeCompleteMaker_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
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

class LArShapeCompleteMaker : public AthAlgorithm
{
 public:
  LArShapeCompleteMaker(const std::string & name, ISvcLocator* pSvcLocator);
  ~LArShapeCompleteMaker();

  //standart algorithm methods
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private:
   
  const LArOnlineID* m_onlineHelper = nullptr;  

  LArSamples::DataStore* m_template;

  ToolHandle<ILArShapeDumperTool> m_dumperTool;
  
  unsigned int m_nSamples, m_nPhases, m_iPhase;
  int m_minNPulses;
  std::string m_shapeErrorFileName, m_outputFileName, m_outputTemplateFileName, m_sgKey;
  std::string m_groupingType;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_onOffMapKey
    { this, "LArOnOffIdMappingKey", "LArOnOffIdMapping", "LArOnOffIdMapping key" };
};

#endif

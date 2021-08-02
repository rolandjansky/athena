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
#include "StoreGate/DataHandle.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include <string>
#include "LArCafJobs/ILArShapeDumperTool.h"
#include "LArCafJobs/DataStore.h"
#include "LArSamplesMon/TreeShapeErrorGetter.h"

class MsgStream;
class StoreGateSvc;
class CaloDetDescrManager;
class ILArShape;
class HWIdentifier;
class Identifier;
class CaloDetDescrElement;
class LArOnlineID;

class LArShapeCompleteMaker : public AthAlgorithm
{
 public:
  LArShapeCompleteMaker(const std::string & name, ISvcLocator* pSvcLocator);
  ~LArShapeCompleteMaker();

  //standart algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
 private:
   
  const LArOnlineID* m_onlineHelper;  

  LArSamples::DataStore* m_template;

  ToolHandle<ILArShapeDumperTool> m_dumperTool;
  ToolHandle<LArCablingLegacyService> m_larCablingSvc;
  
  unsigned int m_nSamples, m_nPhases, m_iPhase;
  int m_minNPulses;
  std::string m_shapeErrorFileName, m_outputFileName, m_outputTemplateFileName, m_sgKey;
  std::string m_groupingType;
};

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARCONDITIONSTOOLS_H
#define TESTLARCONDITIONSTOOLS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>

class CaloCell_ID;

#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArAutoCorrTotalTool.h"
#include "LArRawUtils/LArRoI_Map.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"

class TestLArConditionsTools : public AthAlgorithm
{
 public:
  TestLArConditionsTools(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~TestLArConditionsTools();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  
  StatusCode retrieveTools();  

private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  StatusCode testCaloNoiseDDE();
  StatusCode testCaloCellNoise();
  StatusCode testLArRoI_Map();

  ToolHandle<ILArOFCTool> m_ofcTool;
  ToolHandle<ILArAutoCorrTotalTool> m_acTotalTool;
  ToolHandle<ILArAutoCorrNoiseTool> m_acNoiseTool;
  ToolHandle<ILArADC2MeVTool> m_adc2MeVTool      ;
  ToolHandle<ICaloNoiseTool> m_noiseTool         ;
  ToolHandle<LArRoI_Map> m_roiMap                ;
  bool m_retrieveInInit ;

  const CaloCell_ID*  m_idHelper ;
};

#endif // TESTLARCONDITIONSTOOLS_H

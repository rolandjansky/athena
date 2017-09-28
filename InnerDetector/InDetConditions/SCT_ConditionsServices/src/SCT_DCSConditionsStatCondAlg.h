/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_DCSCONDITIONSSTATCONDALG
#define SCT_DCSCONDITIONSSTATCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSStatCondData.h"

#include "GaudiKernel/ICondSvc.h"

#include "GaudiKernel/Property.h"

class SCT_DCSConditionsStatCondAlg : public AthAlgorithm 
{  
 public:
  SCT_DCSConditionsStatCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_DCSConditionsStatCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyHV;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState;
  SG::WriteCondHandleKey<SCT_DCSStatCondData> m_writeKeyState;

  ServiceHandle<ICondSvc> m_condSvc;

  BooleanProperty m_readAllDBFolders;
  BooleanProperty m_returnHVTemp;
  bool m_doState;
  std::string m_chanstatCut;
  float m_hvLowLimit;
  float m_hvUpLimit;
  bool m_useHV;
  float m_useHVLowLimit;
  float  m_useHVUpLimit;
  std::string m_useHVChanCut;
};

#endif // SCT_DCSCONDITIONSSTATCONDALG

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  virtual ~SCT_DCSConditionsStatCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyHV{this, "ReadKeyHV", "/SCT/DCS/HV", "Key of input (raw) HV conditions folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState{this, "ReadKeyState", "/SCT/DCS/CHANSTAT", "Key of input (raw) State conditions folder"};
  SG::WriteCondHandleKey<SCT_DCSStatCondData> m_writeKeyState{this, "WriteKeyState", "SCT_DCSStatCondData", "Key of output (derived) State conditions data"};

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

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_DCSCONDITIONSSTATCONDALG
#define SCT_DCSCONDITIONSSTATCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSStatCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class SCT_DCSConditionsStatCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_DCSConditionsStatCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_DCSConditionsStatCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyHV{this, "ReadKeyHV", "/SCT/DCS/HV", "Key of input (raw) HV conditions folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState{this, "ReadKeyState", "/SCT/DCS/CHANSTAT", "Key of input (raw) State conditions folder"};
  SG::WriteCondHandleKey<SCT_DCSStatCondData> m_writeKeyState{this, "WriteKeyState", "SCT_DCSStatCondData", "Key of output (derived) State conditions data"};

  ServiceHandle<ICondSvc> m_condSvc;

  bool m_doState;
  BooleanProperty m_readAllDBFolders{this, "ReadAllDBFolders", true};
  BooleanProperty m_returnHVTemp{this, "ReturnHVTemp", true};
  StringProperty m_chanstatCut{this, "StateCut", "NORM"};
  FloatProperty m_hvLowLimit{this, "HVCutLow", 0.0};
  FloatProperty m_hvUpLimit{this, "HVCutUp", 1000000.0};
  BooleanProperty m_useHV{this, "UseDefaultHV", false};
  FloatProperty m_useHVLowLimit{this, "useHVLow", 19.};
  FloatProperty m_useHVUpLimit{this, "useHVUp", 1000000.0};
  StringProperty m_useHVChanCut{this, "useHVChan", "LOOSE"};
};

#endif // SCT_DCSCONDITIONSSTATCONDALG

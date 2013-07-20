/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigMenuNtupleAlg_h__
#define __TrigMenuNtupleAlg_h__
/*
  TrigMenuNtupleAlg.h
*/
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "TrigConfigSvc/ITrigConfigSvc.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuAnalysis/RoILinksCnvTool.h"

class TrigMenuNtupleAlg : public NtupleAlgorithm {
public:
  TrigMenuNtupleAlg(const std::string& name, ISvcLocator* svcloc);
  ~TrigMenuNtupleAlg();

  StatusCode initialize();
  StatusCode beginRun();
  StatusCode finalize();

  ActiveStoreSvc* activeStoreSvc();
  TrigConf::ITrigConfigSvc* trigConfigSvc();
  Trig::TrigDecisionTool* trigDecisionTool();
  TrigAccessTool* trigAccessTool();
  RoILinksCnvTool* roILinksCnvTool();

private:
  ActiveStoreSvc* mActiveStoreSvc;
  ServiceHandle<TrigConf::ITrigConfigSvc> mTrigConfigSvc;
  ToolHandle<Trig::TrigDecisionTool> mTrigDecisionTool;
  ToolHandle<TrigAccessTool> mTrigAccessTool;
  ToolHandle<RoILinksCnvTool> mRoILinksCnvTool;

};

#endif // __TrigMenuNtupleAlg_h__


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSISI_TRIGMENUNTUPLEALG_H
#define TRIGGERMENUANALYSISI_TRIGMENUNTUPLEALG_H
/*
  TrigMenuNtupleAlg.h
*/
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
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
  ActiveStoreSvc* m_activeStoreSvc;
  ServiceHandle<TrigConf::ITrigConfigSvc> m_trigConfigSvc;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  ToolHandle<TrigAccessTool> m_trigAccessTool;
  ToolHandle<RoILinksCnvTool> m_RoILinksCnvTool;

};

#endif // TRIGGERMENUANALYSISI_TRIGMENUNTUPLEALG_H

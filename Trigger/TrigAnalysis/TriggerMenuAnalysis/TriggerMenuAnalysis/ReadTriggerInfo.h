/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS__READTRIGGERINFO_H
#define TRIGGERMENUANALYSIS__READTRIGGERINFO_H
/*
  ReadTriggerInfo.h
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuAnalysis/TrigMenuEvent.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>
#include <string>

class ReadTriggerInfo : public AthAlgorithm {
public:
  ReadTriggerInfo(const std::string& name, ISvcLocator* svcloc);
  virtual ~ReadTriggerInfo();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandle<TrigAccessTool> m_trigAccessTool;
  //  ToolHandle<TrigMenuHists> mTrigMenuHists;

  TrigMenuEvent* m_event;

  std::vector<std::string> m_muonChains;
  std::vector<std::string> m_tileMuChains;
  std::vector<std::string> m_electronChains;
  std::vector<std::string> m_tauChains;
  std::vector<std::string> m_jetChains;
  std::vector<std::string> m_metChains;
};

#endif // TRIGGERMENUANALYSIS__READTRIGGERINFO_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ReadTriggerInfo_h__
#define __ReadTriggerInfo_h__
/*
  ReadTriggerInfo.h
*/
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuAnalysis/TrigMenuEvent.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>
#include <string>

class ReadTriggerInfo : public Algorithm {
public:
  ReadTriggerInfo(const std::string& name, ISvcLocator* svcloc);
  virtual ~ReadTriggerInfo();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ServiceHandle<StoreGateSvc> mStoreGateSvc;
  ToolHandle<TrigAccessTool> mTrigAccessTool;
  //  ToolHandle<TrigMenuHists> mTrigMenuHists;

  TrigMenuEvent* mEvent;

  std::vector<std::string> mMuonChains;
  std::vector<std::string> mTileMuChains;
  std::vector<std::string> mElectronChains;
  std::vector<std::string> mTauChains;
  std::vector<std::string> mJetChains;
  std::vector<std::string> mMetChains;
};

#endif // __ReadTriggerInfo_h__

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANAYSIS_FLATHLTNAVIMAKER_H
#define TRIGGERMENUANAYSIS_FLATHLTNAVIMAKER_H
/*
  FlatHltNaviMaker.h
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TriggerMenuAnalysis/RoILinksCnvTool.h"
#include "GaudiKernel/ToolHandle.h"

class FlatHltNaviMaker : public AthAlgorithm {
public:
  FlatHltNaviMaker(const std::string& name, ISvcLocator* svcloc);
  ~FlatHltNaviMaker();

  StatusCode initialize();
  StatusCode beginRun();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandle<RoILinksCnvTool> m_RoILinksCnvTool;
};

#endif // TRIGGERMENUANAYSIS_FLATHLTNAVIMAKER_H

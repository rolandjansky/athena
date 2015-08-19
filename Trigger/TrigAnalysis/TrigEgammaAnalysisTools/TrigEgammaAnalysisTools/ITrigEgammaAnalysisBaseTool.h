/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ITrigEgammaAnalysisBaseTool.h

#ifndef ITrigEgammaAnalysisBaseTool_H
#define ITrigEgammaAnalysisBaseTool_H

#include "AsgTools/IAsgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IInterface.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
//#include "StoreGate/StoreGateSvc.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

class ITrigEgammaAnalysisBaseTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(ITrigEgammaAnalysisBaseTool)

public:

  virtual StatusCode initialize()=0;
  virtual StatusCode book()=0;
  virtual StatusCode execute()=0;
  virtual StatusCode finalize()=0;
  virtual void setParent(IHLTMonTool *)=0;
  virtual StatusCode childInitialize(){return StatusCode::SUCCESS;};
  virtual StatusCode childBook(){return StatusCode::SUCCESS;};
  virtual StatusCode childExecute(){return StatusCode::SUCCESS;};
  virtual StatusCode childFinalize(){return StatusCode::SUCCESS;};

};

#endif

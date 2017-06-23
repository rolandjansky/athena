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
#include "TrigEgammaAnalysisTools/ITrigEgammaPlotTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationTool.h"

//#include "StoreGate/StoreGateSvc.h"

#include "xAODEgamma/Egamma.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include <utility>


class ITrigEgammaAnalysisBaseTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(ITrigEgammaAnalysisBaseTool)

public:

  virtual StatusCode initialize()=0;
  virtual StatusCode book()=0;
  virtual StatusCode execute()=0;
  virtual StatusCode finalize()=0;
  virtual void setParent(IHLTMonTool *)=0;
  virtual void setPlotTool(ToolHandle<ITrigEgammaPlotTool>)=0;
  virtual void setDetail(bool)=0;
  virtual void setTP(bool)=0;
  virtual void setEmulation(bool)=0;
  virtual void setEmulationTool(ToolHandle<Trig::ITrigEgammaEmulationTool>)=0;
  virtual void setAvgMu(const float, const float)=0;
  virtual void setPVertex(const float, const float)=0;
  virtual void setSGContainsRnn(bool)=0;
  virtual void setSGContainsTrigPhoton(bool)=0; 
  
  virtual StatusCode childInitialize(){return StatusCode::SUCCESS;};
  virtual StatusCode childBook(){return StatusCode::SUCCESS;};
  virtual StatusCode childExecute(){return StatusCode::SUCCESS;};
  virtual StatusCode childFinalize(){return StatusCode::SUCCESS;};
  virtual StatusCode toolExecute(const std::string,const TrigInfo,std::vector<std::pair< const xAOD::Egamma*,const HLT::TriggerElement*>>){return StatusCode::SUCCESS;};

};

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavTPAnalysisTool_H
#define TrigEgammaNavTPAnalysisTool_H


#include "TrigEgammaAnalysisTools/TrigEgammaNavTPBaseTool.h"
class TrigEgammaNavTPAnalysisTool
: public TrigEgammaNavTPBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavTPAnalysisTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavTPAnalysisTool( const std::string& myname );
  virtual ~TrigEgammaNavTPAnalysisTool() {};

  virtual StatusCode childInitialize () override;
  virtual StatusCode childExecute(const EventContext& ctx) const override;
  virtual StatusCode childFinalize() override;

  
};

#endif

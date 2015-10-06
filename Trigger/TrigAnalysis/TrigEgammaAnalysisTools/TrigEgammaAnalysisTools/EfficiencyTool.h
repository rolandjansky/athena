/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EfficiencyTool_H
#define EfficiencyTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"

class EfficiencyTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(EfficiencyTool, ITrigEgammaAnalysisBaseTool)

public:

  EfficiencyTool( const std::string& myname );
  ~EfficiencyTool() {};

  StatusCode childInitialize();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();
protected:

private:
};

#endif

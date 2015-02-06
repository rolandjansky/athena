/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPIneff_H
#define TrigEgammaNavZeeTPIneff_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"

class TrigEgammaNavZeeTPIneff
: public TrigEgammaNavZeeTPBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPIneff, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPIneff( const std::string& myname );
  ~TrigEgammaNavZeeTPIneff() {};

  StatusCode childInitialize ();
  StatusCode childExecute();
  StatusCode childFinalize();

private:

  unsigned int m_eventCounter;
  unsigned int m_probesTried;
  // Tools needed for isEM rebuilding
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronOnlPPCutIDTool;
  std::vector< std::string > m_labels;

};

#endif

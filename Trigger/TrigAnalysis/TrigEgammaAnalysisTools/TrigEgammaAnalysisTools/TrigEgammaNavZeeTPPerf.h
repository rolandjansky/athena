/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPPerf_H
#define TrigEgammaNavZeeTPPerf_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"

class TrigEgammaNavZeeTPPerf
: public TrigEgammaNavZeeTPBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPPerf, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPPerf( const std::string& myname );
  ~TrigEgammaNavZeeTPPerf() {};

  StatusCode childInitialize();
  StatusCode childExecute();
  StatusCode childFinalize();

private:

  unsigned int m_eventCounter;
  unsigned int m_nProbes[3];
  unsigned int m_nProbesPassed[3];

};

#endif

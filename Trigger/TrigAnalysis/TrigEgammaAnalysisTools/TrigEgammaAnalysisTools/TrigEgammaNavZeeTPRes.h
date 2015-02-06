/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPRes_H
#define TrigEgammaNavZeeTPRes_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"

class TrigEgammaNavZeeTPRes
: public TrigEgammaNavZeeTPBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPRes, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPRes( const std::string& myname );
  ~TrigEgammaNavZeeTPRes() {};

  StatusCode childInitialize ();
  StatusCode childExecute();
  StatusCode childFinalize();

private:
  unsigned int m_eventCounter;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPEff_H
#define TrigEgammaNavZeeTPEff_H


#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"

class TrigEgammaNavZeeTPEff
: public TrigEgammaNavZeeTPBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPEff, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPEff( const std::string& myname );
  ~TrigEgammaNavZeeTPEff() {};

  StatusCode childInitialize ();
  StatusCode childExecute();
  StatusCode childFinalize();

private:

  unsigned int m_eventCounter;
};

#endif

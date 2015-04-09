/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPEff_H
#define TrigEgammaNavZeeTPEff_H


#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"
//#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisHelpers.h"
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

  void fillProbeHistos(const std::string,const float,const float,const float,const float);
  void fillMatchHistos(const std::string,const float,const float,const float,const float);
  unsigned int m_eventCounter;
};

#endif

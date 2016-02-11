/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  ~TrigEgammaNavTPAnalysisTool() {};

  StatusCode childInitialize ();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();

private:

  
  /*! Method to book histograms for each trigger */
  void bookPerSignature(const std::string);
  
  unsigned int m_eventCounter;
  std::vector<std::string> m_probelabels;
  std::vector<std::string> m_taglabels;
};

#endif

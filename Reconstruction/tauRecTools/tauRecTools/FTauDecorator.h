/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_FTAUDECORATOR_H
#define TAURECTOOLS_FTAUDECORATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"

// xAOD include(s)
#include "xAODEventInfo/EventInfo.h"

class FTauDecorator
: public TauRecToolBase
{
 public:
  
  ASG_TOOL_CLASS2( FTauDecorator, TauRecToolBase, ITauToolBase )
    
  FTauDecorator(const std::string& name="FTauDecorator");
  virtual ~FTauDecorator();
    
  StatusCode eventInitialize();
  StatusCode execute(xAOD::TauJet& xTau);
  StatusCode eventFinalize();
  
 private:
  //LFstd::string m_tauContainerName;
  //LFconst xAOD::EventInfo* m_xEventInfo;  //!
  //LFconst xAOD::VertexContainer* m_xVertexContainer; //!
  //LFint m_mu; //!
  //LFint m_nVtxPU; //!
  //LFbool m_emitVertexWarning=true; //!
  
};


#endif // TAURECTOOLS_FTAUDECORATOR_H

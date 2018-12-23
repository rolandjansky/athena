/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_FTAUDECORATOR_H
#define TAURECTOOLS_FTAUDECORATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"

class FTauDecorator
: public TauRecToolBase
{
 public:
  
  ASG_TOOL_CLASS2( FTauDecorator, TauRecToolBase, ITauToolBase )
    
  FTauDecorator(const std::string& name="FTauDecorator");
  virtual ~FTauDecorator();
    
  virtual StatusCode execute(xAOD::TauJet& xTau) override;
  
 private:
  
};


#endif // TAURECTOOLS_FTAUDECORATOR_H

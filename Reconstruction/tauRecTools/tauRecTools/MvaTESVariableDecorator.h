/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H
#define TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"

// xAOD include(s)
#include "xAODEventInfo/EventInfo.h"

class MvaTESVariableDecorator
: virtual public TauRecToolBase
{
 public:
  
  ASG_TOOL_CLASS2( MvaTESVariableDecorator, TauRecToolBase, ITauToolBase )
    
  MvaTESVariableDecorator(const std::string& name="MvaTESVariableDecorator");
  virtual ~MvaTESVariableDecorator();
    
  StatusCode eventInitialize();
  StatusCode execute(xAOD::TauJet& xTau);
  StatusCode eventFinalize();
  
 private:
  const xAOD::EventInfo* m_xEventInfo;  //!
  const xAOD::VertexContainer* m_xVertexContainer; //!
  int m_mu; //!
  int m_nVtx; //!
  
  
};


#endif // TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H

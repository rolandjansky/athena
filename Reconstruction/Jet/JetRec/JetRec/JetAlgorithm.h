//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// JetAlgorithm.h 

#ifndef JetAlgorithm_H
#define JetAlgorithm_H

#include "AsgTools/ToolHandle.h"
#include <AnaAlgorithm/AnaReentrantAlgorithm.h>

#include "JetInterface/IJetExecuteTool.h"

class JetAlgorithm : public EL::AnaReentrantAlgorithm { 

public: 
  
  JetAlgorithm(const std::string& name, ISvcLocator* pSvcLocator) : EL::AnaReentrantAlgorithm(name, pSvcLocator) {}
  /// Destructor: 
  ~JetAlgorithm(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;

private:

  /// Athena configured tools
  ToolHandleArray<IJetExecuteTool> m_exetools{this,"Tools",{},"Jet-tools to run"};

}; 

#endif

//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetAlgorithm.h 

#ifndef JetAlgorithm_H
#define JetAlgorithm_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IJetExecuteTool;

class JetAlgorithm : public AthReentrantAlgorithm { 

public: 
  
  //Delegate to base-class constructor
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /// Destructor: 
  ~JetAlgorithm(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;
  StatusCode  finalize() override;

private:

  /// Athena configured tools
  ToolHandleArray<IJetExecuteTool> m_exetools{this,"Tools",{},"Jet-tools to run"};

}; 

#endif

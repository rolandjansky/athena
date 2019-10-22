/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetAlgorithm.h 

#ifndef JetAlgorithm_H
#define JetAlgorithm_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IJetExecuteTool;

class JetAlgorithm : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  JetAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~JetAlgorithm(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  JetAlgorithm();

private:

  /// Athena configured tools
  ToolHandleArray<IJetExecuteTool> m_exetools;

}; 

#endif

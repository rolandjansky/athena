/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaAnalysisAlg.h 

#ifndef TrigEgammaAnalysisAlg_H
#define TrigEgammaAnalysisAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class ITrigEgammaAnalysisBaseTool;
class IExecuteTool;

class TrigEgammaAnalysisAlg : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  TrigEgammaAnalysisAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~TrigEgammaAnalysisAlg(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  TrigEgammaAnalysisAlg();

private:

  /// Athena configured tools
  ToolHandleArray< ITrigEgammaAnalysisBaseTool > m_asgtools;

}; 

#endif

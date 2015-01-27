/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonResonanceAlg.h 

#ifndef MuonResonanceAlg_H
#define MuonResonanceAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class ITHistSvc;
class IMuonResonanceTool;

class MuonResonanceAlg : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  MuonResonanceAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~MuonResonanceAlg(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  MuonResonanceAlg();

private:

  std::string m_muonContainerName;
  // std::string m_probeContainerName;
  // std::string m_matchContainerName;

  /// Athena configured tools
  ServiceHandle<ITHistSvc> m_histSvc;
  ToolHandleArray<IMuonResonanceTool> m_restools;
}; 

#endif

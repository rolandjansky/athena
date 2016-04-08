/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPAlg.h 

#ifndef MuonTPAlg_H
#define MuonTPAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class ITHistSvc;
class IMuonTPTool;

class MuonTPAlg : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  MuonTPAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~MuonTPAlg(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  MuonTPAlg();

private:

  std::string m_tagContainerName;
  std::string m_probeContainerName;
  std::string m_matchContainerName;
  std::string m_topLevelOutputFolderName;

  /// Athena configured tools
  ServiceHandle<ITHistSvc> m_histSvc;
  ToolHandleArray<IMuonTPTool> m_tptools;

  // a flag to note if truth is requested, but we are running on data.
  // Checked in the first event. If true, prevents alg execution to avoid errors.
  bool m_is_TruthOnData;
}; 

#endif

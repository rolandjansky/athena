/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETVERTEXTRUTHMATCHALGORITHM_H
#define INDETVERTEXTRUTHMATCHALGORITHM_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "InDetTruthVertexValidation/IInDetVertexTruthMatchTool.h"

class InDetVertexTruthMatchAlgorithm : public AthAlgorithm {

 public:
  /// Regular Algorithm constructor
  InDetVertexTruthMatchAlgorithm( const std::string& name, ISvcLocator* svcLoc );

  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();

 private:
  /// StoreGate key for the muon container to investigate
  std::string m_sgKey;
  
  ToolHandle<IInDetVertexTruthMatchTool> m_matchTool;

};

#endif

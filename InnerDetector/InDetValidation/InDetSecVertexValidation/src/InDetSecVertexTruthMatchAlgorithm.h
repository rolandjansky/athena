/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSECVERTEXTRUTHMATCHALGORITHM_H
#define INDETSECVERTEXTRUTHMATCHALGORITHM_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "InDetSecVertexValidation/IInDetSecVertexTruthMatchTool.h"

class InDetSecVertexTruthMatchAlgorithm : public AthAlgorithm {

 public:
  /// Regular Algorithm constructor
  InDetSecVertexTruthMatchAlgorithm( const std::string& name, ISvcLocator* svcLoc );

  /// Function initialising the algorithm
  virtual StatusCode initialize() override final;
  /// Function executing the algorithm
  virtual StatusCode execute() override final;

 private:
  /// StoreGate key for the vertex container to investigate
  std::string m_secVtxSGKey;
  std::string m_truthVtxSGKey;
  
  ToolHandle<IInDetSecVertexTruthMatchTool> m_matchTool;

};

#endif

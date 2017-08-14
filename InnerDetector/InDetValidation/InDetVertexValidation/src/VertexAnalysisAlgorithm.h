/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_ANALYSIS
#ifndef VERTEXANALYSISALGORITHM_H
#define VERTEXANALYSISALGORITHM_H


// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"


#include "InDetVertexValidation/VertexAnalysis.h"

#include "GaudiKernel/ToolHandle.h"
#include "PileupReweighting/PileupReweightingTool.h"

class VertexAnalysisAlgorithm : public AthAlgorithm {

 public:
  // Regular Algorithm constructor
  VertexAnalysisAlgorithm( const std::string& name, ISvcLocator* svcLoc );

  // Function initialising the algorithm
  virtual StatusCode initialize();
  // Function executing the algorithm
  virtual StatusCode execute();

 private:

  // Directory name
  std::string m_dirName;

  // StoreGate key for the vertex container to investigate
  std::string m_sgKey;
  std::string m_sgKey2;

  float m_MaxLayoutEta;
  double m_muCorrected;
  double m_muWeight;
  
  // Analyzer code
  VertexAnalysis m_analyzer;

  // Pileup Reweighting Tool
  ToolHandle<CP::IPileupReweightingTool> m_prwTool;

};

#endif

#endif

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLSEXAMPLEATHENA_H
#define TAUANALYSISTOOLSEXAMPLEATHENA_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/ITauSmearingTool.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"

namespace TauAnalysisTools
{

class TauAnalysisToolsExampleAthena : public AthAlgorithm
{

public:
  /// Regular Algorithm constructor
  TauAnalysisToolsExampleAthena( const std::string& name, ISvcLocator* svcLoc );

  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();

private:
  /// StoreGate key for the muon container to investigate
  std::string m_sgKey;

  /// Connection to the selection tool
  ToolHandle< ITauSelectionTool > m_selTool;
  /// Connection to the smearing tool
  ToolHandle< ITauSmearingTool > m_smearTool;
  /// Connection to the efficiency correction tool
  ToolHandle< ITauEfficiencyCorrectionsTool > m_effTool;

}; // class TauAnalysisToolsExampleAthena

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLSEXAMPLEATHENA_H

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElectronMergerAlgorithm.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ELECTRONMERGERALGORITHM_H
#define ELECTRONMERGERALGORITHM_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "DerivationFrameworkEGamma/ElectronMergerTool.h"

class ElectronMergerAlgorithm : public AthAlgorithm {

 public:
  /// Regular Algorithm constructor
  ElectronMergerAlgorithm( const std::string& name, ISvcLocator* svcLoc );

  /// Function initialising the algorithm
  virtual StatusCode initialize() override final;
  /// Function executing the algorithm
  virtual StatusCode execute() override final;

 private:

  ToolHandle<DerivationFramework::IAugmentationTool> m_electronMergerTool{this,"ElectronMergerTool","DerivationFramework::ElectronMergerTool/ElectronMergerTool","ElectronMergerInstance"};

};

#endif
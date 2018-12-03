/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODBTEFF_TOOLTESTER_H
#define xAODBTEFF_TOOLTESTER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"

  class BTagToolTester : public AthAlgorithm {
  public:
    BTagToolTester(const std::string & name, ISvcLocator * svcLoc );
    StatusCode initialize();
    StatusCode execute(); 

  private:
    std::string m_sgKey; // StoreGate key for the jets

    ToolHandle< IBTaggingEfficiencyTool > m_effTool;
    
  };

#endif

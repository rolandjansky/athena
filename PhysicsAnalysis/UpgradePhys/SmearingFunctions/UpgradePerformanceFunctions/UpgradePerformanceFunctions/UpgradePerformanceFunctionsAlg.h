/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// UpgradePerformanceFunctionsAlg.h

#ifndef UpgradePerformanceFunctionsAlg_H
#define UpgradePerformanceFunctionsAlg_H

// Base class
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

// Tool handles
#include "AsgTools/AnaToolHandle.h"

// Our tool
#include "UpgradePerformanceFunctions/IUpgradePerformanceFunctions.h"

// Timing
#include "TStopwatch.h"

// Standard library includes
#include <string>
#include <vector>


class UpgradePerformanceFunctionsAlg : public AthAnalysisAlgorithm { 

  public: 

    /// Constructor with parameters:
    UpgradePerformanceFunctionsAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~UpgradePerformanceFunctionsAlg();

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    UpgradePerformanceFunctionsAlg();

    asg::AnaToolHandle<Upgrade::IUpgradePerformanceFunctions> m_UpgradePerformanceFunctions;

    unsigned int m_Nevts;

    TStopwatch m_clock0;
    TStopwatch m_clock1;
    TStopwatch m_clock2;

}; 



#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauTruthMatchingWrapper.h
// Author: Evelina Bouhova-Thacker (e.bouhova@cern.ch)
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TAUTRUTHMATCHINGWRAPPER_H
#define DERIVATIONFRAMEWORK_TAUTRUTHMATCHINGWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

/**
 * wrapper tool for tau truth matching 
*/

namespace TauAnalysisTools { class ITauTruthMatchingTool; }

namespace DerivationFramework {

  class TauTruthMatchingWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      TauTruthMatchingWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_tauContainerName;

      ToolHandle < TauAnalysisTools::ITauTruthMatchingTool > m_tTauTruthMatchingTool;

  }; 
}

#endif // DERIVATIONFRAMEWORK_TAUPVTRKSELECTIONTOOL_H

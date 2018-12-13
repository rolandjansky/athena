/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AsgSelectionToolWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ASGSELECTIONTOOLWRAPPER_H
#define DERIVATIONFRAMEWORK_ASGSELECTIONTOOLWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class AsgSelectionToolWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      AsgSelectionToolWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ToolHandle<IAsgSelectionTool> m_tool;
      std::string m_cut;
      std::string m_sgName;
      std::string m_containerName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_ASGSELECTIONTOOLWRAPPER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackSelectionToolWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_INDETTRACKSELECTIONTOOLWRAPPER_H
#define DERIVATIONFRAMEWORK_INDETTRACKSELECTIONTOOLWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace InDet
{
  class IInDetTrackSelectionTool;
}

namespace DerivationFramework {

  class InDetTrackSelectionToolWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      InDetTrackSelectionToolWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ToolHandle< InDet::IInDetTrackSelectionTool > m_tool;
      std::string m_sgName;
      std::string m_containerName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_INDETTRACKSELECTIONTOOLWRAPPER_H

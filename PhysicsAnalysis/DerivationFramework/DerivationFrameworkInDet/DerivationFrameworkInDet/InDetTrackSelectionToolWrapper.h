/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackSelectionToolWrapper.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_INDETTRACKSELECTIONTOOLWRAPPER_H
#define DERIVATIONFRAMEWORK_INDETTRACKSELECTIONTOOLWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

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
      SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksKey{ 
      this, "TrackParticlesKey", "InDetTrackParticles", "The input TrackParticleCollection"};

  }; 
}

#endif // DERIVATIONFRAMEWORK_INDETTRACKSELECTIONTOOLWRAPPER_H

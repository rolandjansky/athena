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
#include "StoreGate/WriteDecorHandleKey.h"

namespace DerivationFramework {

  class InDetTrackSelectionToolWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      InDetTrackSelectionToolWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ToolHandle< InDet::IInDetTrackSelectionTool > m_tool
         {this,"TrackSelectionTool","InDet::InDetTrackSelectionTool/TrackSelectionTool"}; // @TODO should not have a default value, since there is not generally correct default

      SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksKey
         {this, "ContainerName", "InDetTrackParticles", "The input TrackParticleCollection"};

      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer>               m_decorationKey
         {this, "DecorationName", "","Name of the decoration which provides the track selection result."};
  };
}

#endif // DERIVATIONFRAMEWORK_INDETTRACKSELECTIONTOOLWRAPPER_H

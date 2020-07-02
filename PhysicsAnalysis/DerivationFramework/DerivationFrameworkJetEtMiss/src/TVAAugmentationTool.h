/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef DERIVATIONFRAMEWORK_TVAAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_TVAAUGMENTATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "AsgTools/ToolHandle.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "AthLinks/ElementLink.h"
#include "xAODTracking/VertexContainer.h"
#include <memory>

namespace DerivationFramework {
  class TVAAugmentationTool : public AthAlgTool, virtual public IAugmentationTool
  {
    public:
      TVAAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);

      virtual StatusCode initialize();
      virtual StatusCode addBranches() const;
    private:
      // Properties
      std::string m_linkName;
      std::string m_trackName;
      std::string m_vertexName;
      ToolHandle<CP::ITrackVertexAssociationTool> m_tool;
      // Internals
      using vtxLink_t = ElementLink<xAOD::VertexContainer>;
      SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_vtxDec_key {this, "vtxDecKey", "", "Decoration for associated vertex"};

  }; //> end class TVAAugmentationTool
} //> end namespace DerivationFramework

#endif //> !DERIVATIONFRAMEWORK_TVAAUGMENTATIONTOOL_H

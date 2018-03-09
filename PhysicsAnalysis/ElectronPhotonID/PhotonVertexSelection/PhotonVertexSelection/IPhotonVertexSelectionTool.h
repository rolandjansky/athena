/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_IPhotonVertexSelectionTool_h
#define PhotonVertexSelection_IPhotonVertexSelectionTool_h

// Framework includes
#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODTracking/VertexFwd.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/PhotonContainer.h"

// std includes
#include <vector>

// Forward declarations
namespace CP {
  class IPhotonPointingTool;
}

namespace CP {

  class IPhotonVertexSelectionTool : public virtual asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE(CP::IPhotonVertexSelectionTool)

  public:
    /// Given a list of photons, return the most likely vertex based on MVA likelihood
    virtual StatusCode getVertex(const xAOD::EgammaContainer &egammas, const xAOD::Vertex* &vertex, bool ignoreConv = false) = 0;

    /// Given a list of photons, return the MLPs of all vertices in the event 
    /// sorted in descending order
    virtual std::vector<std::pair<const xAOD::Vertex*, float> >& getVertex(const xAOD::EgammaContainer &egammas, bool ignoreConv = false) = 0;

    /// Return the last case treated:
    /// 0=conv track associated to vertex,
    /// 1=at least one conv track with Si hits, 
    /// 2=no tracks with Si hits or conversions ignored
    virtual int getCase() const = 0;

    /// Get possible vertex directly associated with photon conversions
    virtual const xAOD::Vertex* getPrimaryVertexFromConv(const xAOD::PhotonContainer *photons) const = 0;

    /// Get access to the photon pointing tool
    virtual const CP::IPhotonPointingTool* photonPointingTool() const = 0;

  }; // class IPhotonVertexSelectionTool

} // namespace CP


#endif // PhotonVertexSelection_IPhotonVertexSelectionTool_h

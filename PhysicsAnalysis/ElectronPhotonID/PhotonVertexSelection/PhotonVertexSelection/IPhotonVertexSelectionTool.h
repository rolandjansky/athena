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
    /// enum indicating where the tool has failed
    enum FailType {
      Unkown        = -99,  // Init value
      NoFail        =   0,  // Ok to run the MVA algorithm
      NoVxCont      =   1,  // No vertex container
      NoEventInfo   =   2,  // No EventInfo
      FailPointing  =   3,  // Calo pointing failed
      FailEgamVect  =   4,  // No diphoton event
      NoGdCandidate =   5,  // Pointing succeded but too distant from any other vertex
      MatchedTrack  =   6,  // Conversion photon has a track attached to a primary/pileup vertex
    };

    enum yyVtxType{
      Unknown       =  -1,  // Init value
      ConvTrack     =   0,  // conv track associated to vertex,
      SiConvTrack   =   1,  // at least one conv track with Si hits, 
      NoSiTracks    =   2,  // no tracks with Si hits or conversions ignored
    };

    /// Given a list of photons, decorate vertex container with MVA variables
    virtual StatusCode decorateInputs(const xAOD::EgammaContainer &egammas, FailType* failType = nullptr) const = 0;

    /// Given a list of photons, return the most likely vertex based on MVA likelihood
    virtual StatusCode getVertex(const xAOD::EgammaContainer &egammas, const xAOD::Vertex* &vertex, bool ignoreConv = false) const= 0;

    /// Given a list of photons, return the MLPs of all vertices in the event 
    /// sorted in descending order
    virtual std::vector< std::pair<const xAOD::Vertex*, float> > getVertex(const xAOD::EgammaContainer &egammas, bool ignoreConv = false, yyVtxType* vtxCase = nullptr, FailType* failType = nullptr) const = 0;

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

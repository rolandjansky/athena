/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_IPhotonPointingTool_h
#define PhotonVertexSelection_IPhotonPointingTool_h

// Framework includes
#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEventInfo/EventInfo.h"

namespace CP {

  class IPhotonPointingTool : public virtual asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE(CP::IPhotonPointingTool)

  public:
    /// Add calo and conversion (HPV) pointing variables
    virtual StatusCode updatePointingAuxdata(const xAOD::EgammaContainer &egammas) const = 0;

    /// Return calo pointing variables
    virtual std::pair<float, float> getCaloPointing(const xAOD::Egamma *egamma) const = 0;
    
    /// Return conversion (HPV) pointing variables
    virtual std::pair<float, float> getConvPointing(const xAOD::Photon *photon) const = 0;

    /// Return the corrected eta for primary vertex z-position
    virtual float getCorrectedEta(const xAOD::CaloCluster &cluster, float PVz) const = 0;

    /// Correct Egamma vector for primary vertex z-position
    virtual StatusCode correctPrimaryVertex(xAOD::Egamma &egamma, float PVz) const = 0;

    /// Get Egamma TLorentzVector corrected for primary vertex z-position
    virtual TLorentzVector getCorrectedLV(const xAOD::Egamma &egamma, float PVz) const = 0;
    
    /// Return the invariant mass, using corrected Pt/Eta of each object for the
    /// given vertex z-position
    virtual float getCorrectedMass(const xAOD::EgammaContainer &egammas, float PVz) const = 0;

  }; // class IPhotonPointingTool

} // namespace CP


#endif // PhotonVertexSelection_IPhotonPointingTool_h

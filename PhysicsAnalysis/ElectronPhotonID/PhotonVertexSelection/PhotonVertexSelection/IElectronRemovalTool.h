/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_IElectronRemovalTool_h
#define PhotonVertexSelection_IElectronRemovalTool_h

// Framework includes
#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

namespace CP {

  class IElectronRemovalTool : public virtual asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE(CP::IElectronRemovalTool)

  public:
    /// Will remove from output vertices all TrackParticles associated to the input electrons from the toRemove container
    virtual StatusCode removeElectronsFromVertex(xAOD::VertexContainer & output, const xAOD::VertexContainer & input, xAOD::ElectronContainer & toRemove, bool updateAuxInfo = true) const = 0;
    
    //if you already have a non-const vertex container to modify
    virtual StatusCode removeElectronsFromVertex(xAOD::VertexContainer & vertices, xAOD::ElectronContainer & toRemove, bool updateAuxInfo = true) const = 0;

    //version returning a pointer to a vertex container - caller takes ownership
    virtual xAOD::VertexContainer * removeElectronsFromVertex(const xAOD::VertexContainer & input, xAOD::ElectronContainer & toRemove, bool updateAuxInfo = true) const = 0;

    

  }; // class IElectronRemovalTool

} // namespace CP


#endif // PhotonVertexSelection_IPhotonPointingTool_h

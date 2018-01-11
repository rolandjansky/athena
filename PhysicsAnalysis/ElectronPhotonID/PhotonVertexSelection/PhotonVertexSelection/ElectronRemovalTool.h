/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_ElectronRemovalTool_h
#define PhotonVertexSelection_ElectronRemovalTool_h

// Framework includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "PhotonVertexSelection/IElectronRemovalTool.h"

// Forward declarations

namespace CP {

  /// Implementation for the photon pointing tool
  ///
  /// Takes photon shower shape and/or conversion vertex
  /// and extrapolates back to beamline
  ///
  /// @author Christopher Meyer <chris.meyer@cern.ch>
  ///
  class ElectronRemovalTool : public virtual IElectronRemovalTool,
                             public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS(ElectronRemovalTool, CP::IElectronRemovalTool)

  private:

    std::vector<const xAOD::TrackParticle *> getElectronTPs( xAOD::ElectronContainer & electrons) const;

  public:
    ElectronRemovalTool(const std::string &name);
    virtual ~ElectronRemovalTool();

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{
    
    /// Function initialising the tool
    virtual StatusCode initialize();
      
    /// @}

    /// @name Function(s) implementing the IElectronRemovalTool interface
    /// @{
    /// Will remove from output vertices all TrackParticles associated to the input electrons from the toRemove container
    StatusCode removeElectronsFromVertex(xAOD::VertexContainer & output, const xAOD::VertexContainer & input, xAOD::ElectronContainer & toRemove, bool updateAuxInfo = true) const;
    
    //if you already have a non-const vertex container to modify
    StatusCode removeElectronsFromVertex(xAOD::VertexContainer & vertices, xAOD::ElectronContainer & toRemove, bool updateAuxInfo = true) const;
  
    //version returning a pointer to a vertex container - caller takes ownership
    xAOD::VertexContainer * removeElectronsFromVertex(const xAOD::VertexContainer & input, xAOD::ElectronContainer & toRemove, bool updateAuxInfo = true) const;

    /// @}

  }; // class ElectronRemovalTool

} // namespace CP


#endif // PhotonVertexSelection_ElectronRemovalTool_H

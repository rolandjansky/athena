/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETUSEDINFITTRACKDECORATORTOOL_INDETUSEDINFITTRACKDECORATORTOOL_H
#define INDETUSEDINFITTRACKDECORATORTOOL_INDETUSEDINFITTRACKDECORATORTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

// Tool include(s)
#include "InDetRecToolInterfaces/IInDetUsedInFitTrackDecoratorTool.h"

// STL includes:
#include <string>

/**
 * @class InDetUsedInFitTrackDecoratorTool
 * Class for decorating tracks with their used-in-fit AMVF vertices and weights
 */

namespace InDet {

  /// Implementation of the hardscatter selection tool
  class InDetUsedInFitTrackDecoratorTool
    : public asg::AsgTool,
      public virtual IInDetUsedInFitTrackDecoratorTool
  {

    ASG_TOOL_CLASS(InDetUsedInFitTrackDecoratorTool, IInDetUsedInFitTrackDecoratorTool)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Constructor(s)/destructor(s)
    /// @{

    /// Constructor
    InDetUsedInFitTrackDecoratorTool(const std::string& name);

    /// Destructor
    ~InDetUsedInFitTrackDecoratorTool();

    /// @}

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{

    /// Function initialising the tool
    virtual StatusCode initialize() override;

    /// Function finalizing the tool
    virtual StatusCode finalize() override;
    
    /// @}
    
    /// @name Function(s) implementing the IInDetUsedInFitTrackDecoratorTool interface
    /// @{

    /// Function for decorating tracks with their used-in-fit AMVF vertices and weights
    virtual void decorate(const xAOD::TrackParticleContainer* trkCont, const xAOD::VertexContainer* vtxCont) const override;
    virtual void decorate() const override;

    /// @}

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
   private:

    /// @name The properties that can be defined via the python job options
    /// @{

    /// Name of the per-track decoration for the AMVF fit vertices
    std::string m_vtxDecoName;

    /// Name of the per-track decoration for the AMVF fit weights
    std::string m_wgtDecoName;

    /// Name of the track particle container (needed for container-less function calls)
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trkContKey;

    /// Name of the primary vertex container (needed for container-less function calls)
    SG::ReadHandleKey<xAOD::VertexContainer> m_vtxContKey;

    /// @}

  private:

    /// @name Truly private internal data members
    /// @{

    /// Per-track decorator for the AMVF fit vertices
    SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_vtxDecoKey;

    /// Per-track decorator for the AMVF fit weights
    SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_wgtDecoKey;

    /// @}

  }; // class: InDetUsedInFitTrackDecoratorTool
} // namespace: InDet

#endif // end: INDETUSEDINFITTRACKDECORATORTOOL_INDETUSEDINFITTRACKDECORATORTOOL_H

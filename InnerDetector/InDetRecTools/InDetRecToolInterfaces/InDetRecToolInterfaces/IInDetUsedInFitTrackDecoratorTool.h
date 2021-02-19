/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRECTOOLINTERFACES_IINDETUSEDINFITTRACKDECORATORTOOL_H
#define INDETRECTOOLINTERFACES_IINDETUSEDINFITTRACKDECORATORTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

/**
 * @class IInDetUsedInFitTrackDecoratorTool
 * Base class for decorating tracks with their used-in-fit AMVF vertices and weights
 */

namespace InDet {

  class IInDetUsedInFitTrackDecoratorTool
    : virtual public asg::IAsgTool
  {

    ASG_TOOL_INTERFACE(IInDetUsedInFitTrackDecoratorTool)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Method for decorating tracks with their used-in-fit AMVF vertices and weights
    /// @{

    virtual void decorate(const xAOD::TrackParticleContainer* trkCont, const xAOD::VertexContainer* vtxCont) const = 0;
    virtual void decorate() const = 0;

    /// @}

  }; // end: class IInDetUsedInFitTrackDecoratorTool
} // end: namespace InDet

#endif // end: INDETRECTOOLINTERFACES_IINDETUSEDINFITTRACKDECORATORTOOL_H

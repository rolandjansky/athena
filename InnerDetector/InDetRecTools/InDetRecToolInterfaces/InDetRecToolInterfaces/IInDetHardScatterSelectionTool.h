/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRECTOOLINTERFACES_IINDETHARDSCATTERSELECTIONTOOL_H
#define INDETRECTOOLINTERFACES_IINDETHARDSCATTERSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

/**
 * @class IInDetHardScatterSelectionTool
 * Base class for hardscatter vertex selection
 */

namespace InDet {

  class IInDetHardScatterSelectionTool
    : virtual public asg::IAsgTool
  {

    ASG_TOOL_INTERFACE(IInDetHardScatterSelectionTool)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Method returning the selected hardscatter vertex
    /// @{

    virtual const xAOD::Vertex* getHardScatter(const xAOD::VertexContainer* vtxCont) const = 0;
    virtual const xAOD::Vertex* getHardScatter() const = 0;

    /// @}

    /// @name Method returning the selected hardscatter vertex link
    /// @{

    virtual ElementLink<xAOD::VertexContainer> getHardScatterLink(const xAOD::VertexContainer* vtxCont) const = 0;
    virtual ElementLink<xAOD::VertexContainer> getHardScatterLink() const = 0;

    /// @}

  }; // end: class IInDetHardScatterSelectionTool
} // end: namespace InDet

#endif // end: INDETRECTOOLINTERFACES_IINDETHARDSCATTERSELECTIONTOOL_H

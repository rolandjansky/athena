/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HardScatterVertexDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_USEDINVERTEXFITTRACKDECORATOR_H
#define DERIVATIONFRAMEWORK_USEDINVERTEXFITTRACKDECORATOR_H

// Framework include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// Tool include(s):
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "InDetRecToolInterfaces/IInDetUsedInFitTrackDecoratorTool.h"

// STL include(s):
#include <string>

namespace DerivationFramework {

  class UsedInVertexFitTrackDecorator :
    public AthAlgTool,
    public IAugmentationTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Constructor
    /// @{

    UsedInVertexFitTrackDecorator(const std::string& type, const std::string& name, const IInterface* parent);

    /// @}

    /// @name Function(s) implementing the AthAlgTool and IAugmentationTool interfaces
    /// @{

    /// Function initialising the tool
    StatusCode initialize();

    /// Function finalizing the tool
    StatusCode finalize();

    /// Function decorating the inputs
    virtual StatusCode addBranches() const;

    /// @}

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
   private:

    /// @name The properties that can be defined via the python job options
    /// @{

    /// ToolHandle for the IInDetUsedInFitTrackDecoratorTool
    ToolHandle<InDet::IInDetUsedInFitTrackDecoratorTool> m_decoTool;

    /// @}

  }; // end: class UsedInVertexFitTrackDecorator
} // end: namespace DerivationFramework

#endif // end: DERIVATIONFRAMEWORK_USEDINVERTEXFITTRACKDECORATOR_H

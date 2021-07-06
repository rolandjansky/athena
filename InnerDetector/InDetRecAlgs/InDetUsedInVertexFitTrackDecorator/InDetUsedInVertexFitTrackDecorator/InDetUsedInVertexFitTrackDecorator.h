/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HardScatterVertexDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDET_INDETUSEDINVERTEXFITTRACKDECORATOR_H
#define INDET_INDETUSEDINVERTEXFITTRACKDECORATOR_H

// Framework include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Tool include(s):
#include "InDetRecToolInterfaces/IInDetUsedInFitTrackDecoratorTool.h"

// STL include(s):
#include <string>

namespace InDet {

  class InDetUsedInVertexFitTrackDecorator :
    public AthReentrantAlgorithm
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Constructor
    /// @{

    InDetUsedInVertexFitTrackDecorator(const std::string& name, ISvcLocator* pSvcLocator);

    /// @}

    /// @name Function(s) implementing the AthReentrantAlgorithm interface
    /// @{

    /// Function initialising the tool
    virtual StatusCode initialize();

    /// Function finalizing the tool
    virtual StatusCode finalize();

    /// Function decorating the inputs
    virtual StatusCode execute(const EventContext& ctx) const;

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

  }; // end: class InDetUsedInVertexFitTrackDecorator
} // end: namespace InDet

#endif // end: INDET_INDETUSEDINVERTEXFITTRACKDECORATOR_H


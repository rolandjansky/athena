/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HardScatterVertexDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HARDSCATTERVERTEXDECORATOR_H
#define DERIVATIONFRAMEWORK_HARDSCATTERVERTEXDECORATOR_H

// Framework include(s):
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODEventInfo/EventInfo.h"

// Tool include(s):
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "InDetRecToolInterfaces/IInDetHardScatterSelectionTool.h"

namespace DerivationFramework {

  class HardScatterVertexDecorator :
    public AthAlgTool,
    public IAugmentationTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Constructor
    /// @{

    HardScatterVertexDecorator(const std::string& type, const std::string& name, const IInterface* parent);

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

    /// ReadHandleKey for the input vertices
    SG::ReadHandleKey<xAOD::VertexContainer> m_vtxContKey;

    /// Name of the output hardscatter decoration (applied to xAOD::EventInfo)
    std::string m_evtDecoName;

    /// ToolHandle for the IInDetHardScatterSelectionTool
    ToolHandle<InDet::IInDetHardScatterSelectionTool> m_vtxSelectTool;

    /// @}

  private:

    /// @name Truly private internal data members
    /// @{

    /// xAOD::EventInfo ReadHandleKey
    SG::ReadHandleKey<xAOD::EventInfo> m_evtInfoKey {this, "EventInfo", "EventInfo", "EventInfo key"};

    /// WriteDecorHandleKey for the output hardscatter decoration (applied to xAOD::EventInfo)
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_evtDecoKey;

    /// @}

  }; // end: class HardScatterVertexDecorator
} // end: namespace DerivationFramework

#endif // end: DERIVATIONFRAMEWORK_HARDSCATTERVERTEXDECORATOR_H

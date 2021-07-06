/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HardScatterVertexDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Matthew Basso (matthew.joseph.basso@cern.ch)
// A very simple tool for decorating vertices as hardscatter or not-hardscatter

// Local include(s):
#include "DerivationFrameworkInDet/HardScatterVertexDecorator.h"

// FrameWork include(s):
#include "AthLinks/ElementLink.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteDecorHandle.h"
#include "AsgTools/CurrentContext.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"

namespace DerivationFramework {

  HardScatterVertexDecorator::HardScatterVertexDecorator(const std::string& type, const std::string& name, const IInterface* parent) : 
    AthAlgTool(type, name, parent),
    m_vtxContKey("PrimaryVertices"),
    m_evtDecoName("hardScatterVertexLink"),
    m_vtxSelectTool("InDet::InDetHardScatterSelectionTool/" + name + "_IDHSSelectionTool", this)
  {
    // Property declarations
    declareProperty("VertexContainerName",      m_vtxContKey,    "Name of the input vertex container");
    declareProperty("HardScatterDecoName",      m_evtDecoName,   "Name of the hardscatter vertex decoration (applied to xAOD::EventInfo)");
    declareProperty("HardScatterSelectionTool", m_vtxSelectTool, "IInDetHardScatterSelectionTool for selecting the hardscatter vertex");
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode HardScatterVertexDecorator::initialize()
  {
    // Print configuration
    ATH_MSG_DEBUG("Initializing " << name() << "...");
    ATH_MSG_DEBUG("Using VertexContainerName: "      << m_vtxContKey);
    ATH_MSG_DEBUG("Using HardScatterDecoName: "      << m_evtDecoName);
    ATH_MSG_DEBUG("Using HardScatterSelectionTool: " << m_vtxSelectTool);

    if (m_vtxContKey.empty()) {
      ATH_MSG_ERROR("No xAOD::VertexContainer provided!");
      return StatusCode::FAILURE;
    }

    if (m_evtDecoName == "") {
      ATH_MSG_ERROR("Hardscatter decoration name cannot be empty!");
      return StatusCode::FAILURE;
    }

    if (m_vtxSelectTool.empty()) {
      ATH_MSG_ERROR("No InDet::IInDetHardScatterSelectionTool provided!");
      return StatusCode::FAILURE;
    }

    // Initialize our vertex and event info container reads
    ATH_CHECK(m_vtxContKey.initialize());
    ATH_CHECK(m_evtInfoKey.initialize());

    // Instantiate and initialize our event info decorator write
    m_evtDecoKey = SG::WriteDecorHandleKey<xAOD::EventInfo>(m_evtInfoKey.key() + "." + m_evtDecoName);
    this->declare(m_evtDecoKey);
    m_evtDecoKey.setOwner(&(*this));
    ATH_CHECK(m_evtDecoKey.initialize());

    // Fetch our InDet::IInDetHardScatterSelectionTool
    ATH_CHECK(m_vtxSelectTool.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode HardScatterVertexDecorator::finalize()
  {
    ATH_MSG_DEBUG("Finalizing " << name() << "...");

    // Release our tool
    ATH_CHECK(m_vtxSelectTool.release());

    return StatusCode::SUCCESS;
  }

  StatusCode HardScatterVertexDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("In addBranches(...) for " << name() <<  "...");

    const EventContext& ctx = Gaudi::Hive::currentContext();

    // Open our vertex container
    SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_vtxContKey, ctx);
    if (!vtxCont.isValid()) {
      ATH_MSG_ERROR("Unable to retrieve xAOD::VertexContainer!");
      return StatusCode::FAILURE;
    }

    // Open our event info
    SG::ReadHandle<xAOD::EventInfo> evtInfo(m_evtInfoKey, ctx);
    if (!evtInfo.isValid()) {
      ATH_MSG_ERROR("Unable to retrieve xAOD::EventInfo!");
      return StatusCode::FAILURE;
    }  

    // Instantiate our WriteDecorHandle
    SG::WriteDecorHandle<xAOD::EventInfo, ElementLink<xAOD::VertexContainer>> evtDeco(m_evtDecoKey, ctx);

    // Decorate our event info
    evtDeco(*evtInfo) = m_vtxSelectTool->getHardScatterLink(vtxCont.get());

    return StatusCode::SUCCESS;
  }  
  
} // end: namespace DerivationFramework

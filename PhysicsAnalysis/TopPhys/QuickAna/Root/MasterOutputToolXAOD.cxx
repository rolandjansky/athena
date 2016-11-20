/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Shouldn't be used in Athena
#ifdef ROOTCORE

// EDM includes
#include "xAODMissingET/MissingETAuxContainer.h"

// Local includes
#include "QuickAna/MasterOutputToolXAOD.h"
#include "QuickAna/ObjectTypeInfo.h"
#include "QuickAna/OutputToolXAOD.h"

namespace ana
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MasterOutputToolXAOD::MasterOutputToolXAOD(const std::string& name)
    : asg::AsgTool(name)
  {
    declareProperty("EventData", m_eventData, "Analysis objects metadata");
    declareProperty("OutputTypes", m_outputTypes,
                    "List of object types to write out. Defaults to all available");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode MasterOutputToolXAOD::initialize()
  {
    // Determine the types to write out. If the requested list is empty,
    // we take all available types from the EventData.
    auto outputTypes = m_outputTypes;
    if(outputTypes.empty()) {
      auto availTypes = m_eventData.getObjectTypes();
      outputTypes.assign( availTypes.begin(), availTypes.end() );
    }

    // Create and configure an output tool for each object type
    for(auto objType : outputTypes) {
      auto objName = ObjectTypeInfo::name[objType];
      ATH_MSG_INFO("Setting up output tool for ObjectType " << objName);

      // Create a output tool handle
      m_outputTools.emplace_back(name() + "_" + objName, this);
      auto& handle = m_outputTools.back();

      // Create the output tool. For now, use a local mapping
      // of ObjectType to actual container types.
      if(objType == OBJECT_EVENTINFO) {
        using ToolType_t = OutputToolXAOD<xAOD::EventInfo, xAOD::ShallowAuxInfo>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_EVENT_SELECT) {
        using ToolType_t = OutputToolXAOD<EventSelectType, xAOD::ShallowAuxInfo>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_ELECTRON) {
        using ToolType_t = OutputToolXAOD<xAOD::ElectronContainer, xAOD::ShallowAuxContainer>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_MUON) {
        using ToolType_t = OutputToolXAOD<xAOD::MuonContainer, xAOD::ShallowAuxContainer>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_TAU) {
        using ToolType_t = OutputToolXAOD<xAOD::TauJetContainer, xAOD::ShallowAuxContainer>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_JET) {
        using ToolType_t = OutputToolXAOD<xAOD::JetContainer, xAOD::ShallowAuxContainer>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_PHOTON) {
        using ToolType_t = OutputToolXAOD<xAOD::PhotonContainer, xAOD::ShallowAuxContainer>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_MET) {
        using ToolType_t = OutputToolXAOD<xAOD::MissingETContainer, xAOD::MissingETAuxContainer>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else if(objType == OBJECT_MET2) {
        using ToolType_t = OutputToolXAOD<xAOD::MissingETContainer, xAOD::MissingETAuxContainer>;
        ATH_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType_t) );
      }
      else {
        ATH_MSG_ERROR("Unsupported object type: " << objName);
        m_outputTools.pop_back();
        return StatusCode::FAILURE;
      }

      // Configure the output tool with the EventData
      auto copyConts = m_eventData.getSourceNames(objType);
      auto writeConts = m_eventData.getNames(objType);
      ATH_CHECK( handle.setProperty("CopyContainerNames", copyConts) );
      ATH_CHECK( handle.setProperty("WriteContainerNames", writeConts) );
      ATH_CHECK( handle.initialize() );
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Execute the output writing
  //---------------------------------------------------------------------------
  StatusCode MasterOutputToolXAOD::write()
  {
    // Loop over the tools and call them
    for(auto& tool : m_outputTools) {
      ATH_CHECK( tool->write() );
    }
    return StatusCode::SUCCESS;
  }

} // namespace ana

#endif // ROOTCORE

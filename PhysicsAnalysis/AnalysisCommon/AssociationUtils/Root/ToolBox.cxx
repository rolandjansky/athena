/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "AssociationUtils/ToolBox.h"

namespace ORUtils
{

  //-------------------------------------------------------------------------
  // Constructor
  //-------------------------------------------------------------------------
  ToolBox::ToolBox(const std::string& name, parentType_t* theParent)
    : asg::AsgMessaging(name),
      masterTool("", theParent),
      muPFJetORT("", theParent),
      eleEleORT("", theParent),
      eleMuORT("", theParent),
      eleJetORT("", theParent),
      muJetORT("", theParent),
      tauEleORT("", theParent),
      tauMuORT("", theParent),
      tauJetORT("", theParent),
      phoEleORT("", theParent),
      phoMuORT("", theParent),
      phoJetORT("", theParent),
      eleFatJetORT("", theParent),
      jetFatJetORT("", theParent),
      m_parent(theParent)
  {}

  //-------------------------------------------------------------------------
  // Attach and initialize the tools
  //-------------------------------------------------------------------------
  StatusCode ToolBox::initialize()
  {

#ifndef XAOD_STANDALONE
    // For now, in Athena, things only work with private tools.
    // See ATLASG-957 for more details.
    if(!m_parent) {
      ATH_MSG_ERROR("For now, if you want to use the ToolBox in Athena, " <<
                    "the tools must be private to some algorithm, tool, etc.");
      return StatusCode::FAILURE;
    }
#endif

    // Initialize the overlap tools
    ATH_CHECK( initTool(muPFJetORT, "MuPFJetORT") );
    ATH_CHECK( initTool(eleEleORT, "EleEleORT") );
    ATH_CHECK( initTool(eleMuORT, "EleMuORT") );
    ATH_CHECK( initTool(eleJetORT, "EleJetORT") );
    ATH_CHECK( initTool(muJetORT, "MuJetORT") );
    ATH_CHECK( initTool(tauEleORT, "TauEleORT") );
    ATH_CHECK( initTool(tauMuORT, "TauMuORT") );
    ATH_CHECK( initTool(tauJetORT, "TauJetORT") );
    ATH_CHECK( initTool(phoEleORT, "PhoEleORT") );
    ATH_CHECK( initTool(phoMuORT, "PhoMuORT") );
    ATH_CHECK( initTool(phoJetORT, "PhoJetORT") );
    ATH_CHECK( initTool(eleFatJetORT, "EleFatJetORT") );
    ATH_CHECK( initTool(jetFatJetORT, "JetFatJetORT") );

    // Initialize the master tool
    ATH_CHECK( initMaster() );

    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------
  // Get a list of all overlap tool handles
  //-------------------------------------------------------------------------
  std::vector<ToolBox::OverlapHandle_t*> ToolBox::getOverlapTools()
  {
    // This is somewhat error prone, but better than not having it.
    std::vector<OverlapHandle_t*> tools;
    tools.reserve(13);
    if(!muPFJetORT.empty())  tools.push_back(&muPFJetORT);
    if(!eleEleORT.empty()) tools.push_back(&eleEleORT);
    if(!eleMuORT.empty())  tools.push_back(&eleMuORT);
    if(!eleJetORT.empty()) tools.push_back(&eleJetORT);
    if(!muJetORT.empty())  tools.push_back(&muJetORT);
    if(!tauEleORT.empty()) tools.push_back(&tauEleORT);
    if(!tauMuORT.empty())  tools.push_back(&tauMuORT);
    if(!tauJetORT.empty()) tools.push_back(&tauJetORT);
    if(!phoEleORT.empty()) tools.push_back(&phoEleORT);
    if(!phoMuORT.empty())  tools.push_back(&phoMuORT);
    if(!phoJetORT.empty()) tools.push_back(&phoJetORT);
    if(!eleFatJetORT.empty()) tools.push_back(&eleFatJetORT);
    if(!jetFatJetORT.empty()) tools.push_back(&jetFatJetORT);
    return tools;
  }

  //-------------------------------------------------------------------------
  // Initialize one overlap tool
  //-------------------------------------------------------------------------
  StatusCode ToolBox::initTool(OverlapHandle_t& handle, const std::string& key)
  {
    // Handle must be non-empty
    if(!handle.empty()) {
      ATH_MSG_DEBUG("Initializing " << handle.name());
      ATH_MSG_DEBUG("  type " << handle.type());
      ATH_MSG_DEBUG("  isInitialized  " << handle.isInitialized());

      // Initialize it
      if(!handle.isInitialized()) {
        ATH_CHECK( handle.initialize() );
      }
      // Add it to the master tool
      if(!masterTool.empty()) {
        ATH_CHECK( masterTool.setProperty(key, handle) );
      }
    }
    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------
  // Initialize master tool
  //-------------------------------------------------------------------------
  StatusCode ToolBox::initMaster()
  {
    // For now, we look for a non-empty "type"
    if(!masterTool.empty()) {
      ATH_MSG_DEBUG("Initializing master " << masterTool.name());
      ATH_MSG_DEBUG("  type " << masterTool.type());
      ATH_MSG_DEBUG("  isInitialized  " << masterTool.isInitialized());
      if(!masterTool.isInitialized()) {
        ATH_CHECK( masterTool.initialize() );
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace ORUtils

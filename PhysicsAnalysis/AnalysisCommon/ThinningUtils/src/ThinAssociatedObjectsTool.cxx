/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/******************************************************************************
Name:        ThinAssociatedObjectsTool

Author:      Jon Burr
Created:     November 2018
******************************************************************************/

// package includes
#include "ThinAssociatedObjectsTool.h"
#include "DerivationFrameworkInterfaces/IForwardParentThinningTool.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"

//=============================================================================
// Constructor
//=============================================================================
ThinAssociatedObjectsTool::ThinAssociatedObjectsTool(
    const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  ::AthAlgTool(type, name, parent),
  m_thinningSvc("ThinningSvc", name)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc,
      "The associated thinning service.");
  declareProperty("SGKey", m_sgKey,
      "The StoreGate key of the parent container. Thinning already applied "
      "to this container will be taken into account.");
  declareProperty("SelectionString", m_selectionString,
      "A selection string describing any further selection to be applied. "
      "Note that this is a selection on the elements used to thin the child "
      "containers, no further thinning is applied to the parent.");
  declareProperty("ChildThinningTools", m_childTools,
      "The tools responsible for forwarding the parent thinning to child "
      "containers.");
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode ThinAssociatedObjectsTool::initialize()
{
  ATH_MSG_INFO("initialize() " << name() );
  // Verify the set properties
  if (m_sgKey.empty() ) {
    ATH_MSG_ERROR("No SGKey provided!");
    return StatusCode::FAILURE;
  }

  if (!m_selectionString.empty() ) {
    m_parser = std::make_unique<DerivationFramework::ExpressionParserHelper>(
        evtStore() );
    if (!m_parser->parser().loadExpression(m_selectionString) ) {
      ATH_MSG_ERROR( "Failed to interpret expression: " << m_selectionString );
      return StatusCode::FAILURE;
    }
  }

  if (m_childTools.size() == 0) {
    ATH_MSG_ERROR("No child thinning tools - tool would have no effect!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_thinningSvc.retrieve() );
  ATH_CHECK( m_childTools.retrieve() );

  return StatusCode::SUCCESS;
}

//=============================================================================
// doThinning
//=============================================================================
StatusCode ThinAssociatedObjectsTool::doThinning() const
{
  // Retrieve the parent container
  const xAOD::IParticleContainer* parent(nullptr);
  ATH_CHECK( evtStore()->retrieve(parent, m_sgKey) );

  std::size_t nParentObjects = parent->size();
  // Create the parent mask
  std::vector<bool> mask(nParentObjects, false);
  if (!m_thinningSvc->thinningOccurred(parent) )
    mask = std::vector<bool>(nParentObjects, true);
  else
    for (std::size_t ii = 0; ii < nParentObjects; ++ii)
      if (m_thinningSvc->index(parent, ii) != IThinningSvc::RemovedIdx)
        mask.at(ii) = true;

  if (m_parser) {
    // Execute the text parser
    const std::vector<int> parserResult = m_parser->parser().evaluateAsVector();
    // Ensure that the sizes are compatible
    if (parserResult.size() != nParentObjects) {
      ATH_MSG_ERROR("Size of parser result and parent container are "
          "incompatible! Is the parent container the same one used in the "
          "selection string?");
      return StatusCode::FAILURE;
    }
    // Remove elements failing this parser from the mask.
    for (size_t ii = 0; ii < nParentObjects; ++ii)
      if (!parserResult.at(ii) )
        mask.at(ii) = false;
  }

  // Let the child tools do their thing
  for (auto& child : m_childTools)
    ATH_CHECK( child->forwardParentThinning(parent, mask) );

  return StatusCode::SUCCESS;
}

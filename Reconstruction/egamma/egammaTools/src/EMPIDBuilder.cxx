/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:

#include "AthenaKernel/errorcheck.h"

#include "EMPIDBuilder.h"
#include "xAODEgamma/Egamma.h"
#include "PATCore/IAsgSelectionTool.h"

// ===========================================================================
EMPIDBuilder::EMPIDBuilder(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
  : egammaBaseTool(type, name, parent)
{
  //
  // constructor
  //

  // declare interface
  declareInterface<IegammaBaseTool>(this);

  declareProperty("selectors", m_selectors,
    "The selectors that we need to apply to the EGamma object");
  declareProperty("selectorResultNames", m_selectorResultNames,
    "The selector result names");
}

// ===============================================================
EMPIDBuilder::~EMPIDBuilder()
{ 
  //
  // destructor
  //
}

// ==================================================================
StatusCode EMPIDBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG(" Initializing EMPIDBuilder");
 
  for (const auto& selector : m_selectors) {
    CHECK(selector.retrieve());
  }

  if (m_selectors.size() != m_selectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given selector names");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode EMPIDBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}


// =====================================================================
StatusCode EMPIDBuilder::execute(xAOD::Egamma* eg)
{ 
  
  ATH_MSG_DEBUG("Executing EMPIDBuilder::execute");

  // Protection against bad pointers
  if (eg==0) {
    ATH_MSG_WARNING("NULL egamma object given to execute function");
    return StatusCode::SUCCESS;
  }
  
  size_t size = m_selectors.size();

  for (size_t i = 0; i<size; i++) {
    const Root::TAccept& accept = m_selectors[i]->accept(eg);
    eg->setPassSelection(static_cast<bool>(accept), m_selectorResultNames[i]);
  }
  
  return StatusCode::SUCCESS;
}





/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetAlgorithm.cxx 

#include "JetRec/JetAlgorithm.h"
#include "JetInterface/IJetExecuteTool.h"

JetAlgorithm::~JetAlgorithm() { }

//**********************************************************************

StatusCode JetAlgorithm::initialize() {
  StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  sc = m_exetools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Tool retrieval failed.");
  }
  ATH_MSG_INFO("Tool retrieval completed.");
  unsigned int ntool = m_exetools.size();
  ATH_MSG_INFO("  Tool count: " << ntool);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ATH_MSG_INFO("    " << m_exetools[itool]->name());
    m_exetools[itool]->print();
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetAlgorithm::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetAlgorithm::execute(const EventContext&) const {  
  ATH_MSG_VERBOSE("Executing " << name() << "...");
  // Loop over tools.
  unsigned int ntools = m_exetools.size();
  ATH_MSG_DEBUG("Looping over " << ntools << " jet execute tools.");
  const std::string line = "---------------------------------------------------";
  if ( ntools ) ATH_MSG_DEBUG(line);
  for ( ToolHandleArray<IJetExecuteTool>::const_iterator itoo=m_exetools.begin();
        itoo!=m_exetools.end(); ++itoo ) {
    ToolHandle<IJetExecuteTool> htool = *itoo;
    ATH_MSG_DEBUG("Executing tool " << htool->name());
    int jstat = htool->execute();
    if ( jstat != 0 ) ATH_MSG_INFO("Tool returned error " << jstat);
    else ATH_MSG_DEBUG("Tool execution succeeded");
    ATH_MSG_DEBUG(line);
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

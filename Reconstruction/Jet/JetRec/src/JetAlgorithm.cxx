
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// JetAlgorithm.cxx 

#include "JetRec/JetAlgorithm.h"
#include "JetInterface/IJetExecuteTool.h"

JetAlgorithm::~JetAlgorithm() { }

//**********************************************************************

StatusCode JetAlgorithm::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_CHECK( m_exetools.retrieve() );
  ATH_MSG_INFO("  Tool count: " << m_exetools.size());
  for ( const ToolHandle<IJetExecuteTool>& tool : m_exetools ) {
    ATH_MSG_INFO("    " << tool->name());
    tool->print();
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetAlgorithm::execute(const EventContext&) const {  
  // Loop over tools.
  for ( const ToolHandle<IJetExecuteTool>& htool : m_exetools ) {
    ATH_MSG_DEBUG("Executing tool " << htool->name());
    const int jstat = htool->execute();
    if ( jstat != 0 ) ATH_MSG_INFO("Tool " << htool->name() << " returned error " << jstat);
    else ATH_MSG_DEBUG("Tool execution succeeded");
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

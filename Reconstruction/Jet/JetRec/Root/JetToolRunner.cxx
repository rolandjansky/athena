/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// JetToolRunner.cxx 

#include "JetRec/JetToolRunner.h"
#include <iomanip>
#include "JetInterface/IJetExecuteTool.h"

using std::string;
using std::setw;
using std::setprecision;
using std::fixed;

//**********************************************************************

JetToolRunner::JetToolRunner(const std::string& myname): 
  AsgTool(myname){
}

//**********************************************************************

StatusCode JetToolRunner::initialize() {
  StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  sc = m_evstools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Eventshape tool retrieval failed.");
  }
  sc = m_exetools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Execute tool retrieval failed.");
  }
  ATH_MSG_INFO("Tool retrieval completed.");
  unsigned int ntool = m_exetools.size();
  ATH_MSG_INFO("  Tool count: " << ntool);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ATH_MSG_INFO("    " << m_exetools[itool]->name());
    m_exetools[itool]->print();
  }
  unsigned int nevstool = m_evstools.size();
  ATH_MSG_INFO("  Tool count: " << nevstool);
  for ( unsigned int itool=0; itool<nevstool; ++itool ) {
    ATH_MSG_INFO("    " << m_evstools[itool]->name());
    m_evstools[itool]->print();
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetToolRunner::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetToolRunner::execute() const {  
  ATH_MSG_VERBOSE("Executing " << name() << "...");
  string line = "---------------------------------------------------";
  // Loop over event shape tools.
  unsigned int nevstool = m_evstools.size();
  ATH_MSG_DEBUG("Looping over " << nevstool << " event shape tools.");
  if ( nevstool ) ATH_MSG_DEBUG(line);
  for ( unsigned int itool=0; itool<nevstool; ++itool ) {
    ToolHandle<IEventShapeTool> htool = m_evstools[itool];
    ATH_MSG_DEBUG("Executing tool " << htool->name());
    ATH_MSG_VERBOSE("Retrieving tool with IEventShapeTool interface.");
    StatusCode sc = htool->fillEventShape();
    if ( ! sc.isSuccess() ) ATH_MSG_INFO("Events shape tool returned error.");
    else ATH_MSG_DEBUG("Tool execution succeeded");
    ATH_MSG_DEBUG(line);
  }
  // Loop over execute tools.
  unsigned int ntool = m_exetools.size();
  ATH_MSG_DEBUG("Looping over " << ntool << " jet execute tools.");
  if ( ntool ) ATH_MSG_DEBUG(line);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ToolHandle<IJetExecuteTool> htool = m_exetools[itool];
    ATH_MSG_DEBUG("Executing tool " << htool->name());
    ATH_MSG_VERBOSE("Retrieving tool with IJetExecuteTool interface.");
    int jstat = htool->execute();
    if ( jstat != 0 ) ATH_MSG_INFO("Tool returned error " << jstat);
    else ATH_MSG_DEBUG("Tool execution succeeded");
    ATH_MSG_DEBUG(line);
  }

  return 0;
}

//**********************************************************************

void JetToolRunner::print() const {
  ATH_MSG_INFO("Properties for JetToolRunner " << name());
  ATH_MSG_INFO("  Event Shape tool list has " << m_evstools.size() << " entries:");
  for ( ToolHandleArray<IEventShapeTool>::const_iterator itoo=m_evstools.begin();
        itoo!=m_evstools.end(); ++itoo ) {
    ToolHandle<IEventShapeTool> htool = *itoo;
    ATH_MSG_INFO("    " << htool->name());
  }
  ATH_MSG_INFO("  Execute tool list has " << m_exetools.size() << " entries:");
  for ( ToolHandleArray<IJetExecuteTool>::const_iterator itoo=m_exetools.begin();
        itoo!=m_exetools.end(); ++itoo ) {
    ToolHandle<IJetExecuteTool> htool = *itoo;
    ATH_MSG_INFO("    " << htool->name());
  }
}

//**********************************************************************

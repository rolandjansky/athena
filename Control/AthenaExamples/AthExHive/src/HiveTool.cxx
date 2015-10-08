/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveTool.h"

#include "GaudiKernel/SvcFactory.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//DECLARE_TOOL_FACTORY(HiveTool)


HiveTool::HiveTool(const std::string& type, const std::string& name, 
			 const IInterface* parent)
  : base_class( type, name, parent ),
    m_rdh1("b1")
{
  
  declareProperty("MyMessage", m_myMessage, "the default message");
  
  declareProperty("Key_R1",m_rdh1);

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HiveTool::~HiveTool() {
}

StatusCode
HiveTool::initialize() {
  ATH_MSG_INFO("initialize");

  return StatusCode::SUCCESS;
}

StatusCode
HiveTool::finalize() {
  ATH_MSG_INFO("finalize");

  return StatusCode::SUCCESS;
}

StatusCode HiveTool::saySomething() const {
  ATH_MSG_INFO ("message: " << m_myMessage);

  if (!m_rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << m_rdh1.key() << " = " << m_rdh1->val() );

  return StatusCode::SUCCESS;
}

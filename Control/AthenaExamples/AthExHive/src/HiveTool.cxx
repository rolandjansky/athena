/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveTool.h"

#include "GaudiKernel/SvcFactory.h"
#include "StoreGate/ReadHandle.h"


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

  ATH_CHECK( m_rdh1.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
HiveTool::finalize() {
  ATH_MSG_INFO("finalize");

  return StatusCode::SUCCESS;
}

StatusCode HiveTool::saySomething() const {
  ATH_MSG_INFO ("message: " << m_myMessage);

  SG::ReadHandle<HiveDataObj> rh( m_rdh1 );

  // if (!m_rdh1.isValid()) {
  //   ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
  //   return StatusCode::FAILURE;
  // }

  ATH_MSG_INFO("  read: " << rh.key() << " = " << rh->val() );

  return StatusCode::SUCCESS;
}

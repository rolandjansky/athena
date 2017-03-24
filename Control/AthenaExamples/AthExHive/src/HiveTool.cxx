/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveTool.h"

#include "GaudiKernel/SvcFactory.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include <memory>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//DECLARE_TOOL_FACTORY(HiveTool)


HiveTool::HiveTool(const std::string& type, const std::string& name, 
			 const IInterface* parent)
//  : base_class( type, name, parent ),
  : AthAlgTool(type,name,parent),
    m_rdh1("x1"),
    m_wrh1("X1")
{
  
  declareProperty("MyMessage", m_myMessage, "the default message");
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_W1",m_wrh1);

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HiveTool::~HiveTool() {
}

StatusCode
HiveTool::initialize() {
  ATH_MSG_INFO("initialize");

  if (m_rdh1.key() != "") {
  ATH_CHECK( m_rdh1.initialize() );
    ATH_MSG_INFO("read key: " << m_rdh1.key());
  } else {
    ATH_MSG_INFO("RH key is blank. not initializing");
  }

  if (m_wrh1.key() != "") {
    ATH_CHECK( m_wrh1.initialize() );
    ATH_MSG_INFO("write key: " << m_wrh1.key());
  } else {
    ATH_MSG_INFO("WH key is blank. not initializing");
  }



  return StatusCode::SUCCESS;
}

StatusCode
HiveTool::finalize() {
  ATH_MSG_INFO("finalize");

  return StatusCode::SUCCESS;
}

StatusCode HiveTool::saySomething() const {
  ATH_MSG_INFO ("message: " << m_myMessage);

  int val = 0;
  
  if (m_rdh1.key() == "") {
    ATH_MSG_INFO("RH not valid - not retrieving");
  } else {    
  SG::ReadHandle<HiveDataObj> rh( m_rdh1 );
    ATH_MSG_INFO("  read: " << rh.key() << " = " << rh->val() );
    val = rh->val();
  }

  if (m_wrh1.key() == "") {
    ATH_MSG_INFO("WH not valid - not writing");
  } else {
    SG::WriteHandle<HiveDataObj> wrh1( m_wrh1 );
    wrh1 = std::make_unique< HiveDataObj >
      ( HiveDataObj(val + 666) );
    ATH_MSG_INFO("  write: " << wrh1.key() << " = " << wrh1->val() );
  }


  return StatusCode::SUCCESS;
}

/* 
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationToolTest.h"
#include "GaudiKernel/Property.h"
#include "PATCore/TAccept.h"

using std::string;

//**********************************************************************
namespace Trig{

  TrigBtagEmulationToolTest::TrigBtagEmulationToolTest(const std::string& name, 
						       ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator ),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_emulationTool("Trig::TrigBtagEmulationTool/TrigBtagEmulationTool",this)
  {
    declareProperty("TrigBtagEmulationTool",m_emulationTool);
  }
  
  //**********************************************************************
  
  TrigBtagEmulationToolTest::~TrigBtagEmulationToolTest() { }
  
  //**********************************************************************
  
  StatusCode TrigBtagEmulationToolTest::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");

    // RETRIEVE TOOLS
    if ( (m_trigdec.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
      return StatusCode::FAILURE;
    }
    if ( (m_emulationTool.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Emulation Tool! Can't work");
      return StatusCode::FAILURE;
    }
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
      return sc;
    }

    // CHAIN CONFIGURATION IN ATHENA IS PERFORMED VIA JOB OPTION

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode TrigBtagEmulationToolTest::finalize() {
    return StatusCode::SUCCESS;
  }

  //**********************************************************************
  StatusCode TrigBtagEmulationToolTest::execute() {  

    ATH_MSG_DEBUG("TrigBtagEmulationTool::execute()");

    // CHECK RESUT FOR INDIVIDUAL CHAINS
    std::vector<std::string> names;
    names.push_back("HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23");
    //names.push_back("HLT_2j45_bmv2c2077_2j45_L13J25.0ETA23");
    names.push_back("HLT_2j35_bmv2c2070_split_2j35_L13J25.0ETA23");
    names.push_back("HLT_2j45_bmv2c2077_split_2j45_L13J25.0ETA23");
    bool orig=false, emul=false;
    for (auto& name : names) {
      orig=m_trigdec->isPassed(name); emul=m_emulationTool->isPassed(name);
      if (orig!=emul) ATH_MSG_INFO("MISMATCH FOR " << name);
      ATH_MSG_INFO("ORIGINAL RESULT FOR " << name << " = " << (int)orig);
      ATH_MSG_INFO("EMULATED RESULT FOR " << name << " = " << (int)emul);
    }
    
    return StatusCode::SUCCESS;

  }

}///namespace
//**********************************************************************

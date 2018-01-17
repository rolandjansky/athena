/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** J.Montejo - 15/10/2015 - tests for TrigDecisionTool **/

#include "TriggersFiredTest.h"

#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

TriggersFiredTest::TriggersFiredTest(const std::string &name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
  m_first_event(true),
  //m_configHandle( "TrigConf::xAODConfigTool/xAODConfigTool"),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_passed_l1(0),
  m_passed_hlt(0)
{
    declareProperty("TestList",       m_TestList, "Test list");
}


TriggersFiredTest::~TriggersFiredTest() {}

StatusCode TriggersFiredTest::initialize()
{
    // reset event counters
    m_first_event=true;
    
    // print out properties
    msg(MSG::INFO) << "Initializing TriggersFiredTest ..." << endmsg;
    
    // get handle to trigger configuration
    //ToolHandle<TrigConf::ITrigConfigTool> configHandle(&m_configTool);
    //StatusCode sc = m_configHandle.retrieve();
    //if ( sc.isFailure() ) {
    //    msg(MSG::ERROR) << "Could not retrieve TrigDecisionTool!" << endmsg;
    //    return sc;
    //}
		//sc = m_configHandle->initialize();
    
    // get handle to TrigDecisionTool
    //sc = m_trigDec.retrieve();
    StatusCode sc = m_trigDec.retrieve();
    if ( sc.isFailure() ) {
        msg(MSG::ERROR) << "Could not retrieve TrigDecisionTool!" << endmsg;
        return sc;
    }
    //m_trigDec->ExperimentalAndExpertMethods()->enable();
		//sc = m_trigDec->setProperty("ConfigTool",m_configHandle);
		//sc = m_trigDec->setProperty("TrigDecisionKey","xTrigDecision");
    //if ( sc.isFailure() ) {
    //    msg(MSG::ERROR) << "Some problem!" << endmsg;
    //    return sc;
    //}
    
    msg(MSG::INFO) << "Initialization successful" << endmsg;
    
    return StatusCode::SUCCESS;
}


StatusCode TriggersFiredTest::finalize()
{
    
    // print summary of trigger decisions for each level
    msg(MSG::INFO) << "==========================================================" << endmsg;
    msg(MSG::INFO) << "TriggersFiredTest summary:" << endmsg;
    msg(MSG::INFO) << "==========================================================" << endmsg;
    
    
    // print out nr. of events passed in blocks of N events for specific chains (configurable)
    //msg(MSG::INFO) << "REGTEST  Nr.events: ";
    
    msg(MSG::INFO) << "Finalised successfully" << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode TriggersFiredTest::execute()
{
    
    msg(MSG::INFO) << "TriggersFiredTest::execute" << endmsg;
    
    m_passed_l1 += m_trigDec->isPassed("L1_.*");
    m_passed_hlt += m_trigDec->isPassed("HLT_.*");
    
    // Full print of first event
    if (m_first_event){
			m_first_event = false;
		  auto chainGroups = m_trigDec->getChainGroup(".*");
  			for(auto &trig : chainGroups->getListOfTriggers()) {
    		msg(MSG::INFO) << "  " << trig << " " << m_trigDec->isPassed(trig) << endmsg;
  		}
		}
    
    return StatusCode::SUCCESS;
}


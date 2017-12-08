/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** J.Montejo - 10/11/2015 - AthenaTestHarness **/

#include "AthenaTestHarness.h"
#include "TrigAnalysisTest/TestFactory.h"

#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

AthenaTestHarness::AthenaTestHarness(const std::string &name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
	m_test(nullptr),
	m_test_name(name)
{ }

AthenaTestHarness::~AthenaTestHarness() {}

StatusCode AthenaTestHarness::initialize()
{
    
    // print out properties
    msg(MSG::INFO) << "Initializing AthenaTestHarness ..." << endmsg;
    
    StatusCode sc = m_trigDec.retrieve();
    if ( sc.isFailure() ) {
        msg(MSG::ERROR) << "Could not retrieve TrigDecisionTool!" << endmsg;
        return sc;
    }
		m_test = GetTrigAnalysisTest(m_test_name);
		if(!m_test){
        msg(MSG::ERROR) << "Could not retrieve the test: "<< m_test_name << endmsg;
        return StatusCode::FAILURE;
		}

		m_test->setEventStore( evtStore().operator->() ); // explicitly call the arrow operator
    
    msg(MSG::INFO) << "Initialization successful" << endmsg;
    
    return StatusCode::SUCCESS;
}


StatusCode AthenaTestHarness::finalize()
{
    
		if(m_test->finalize())
    	return StatusCode::FAILURE;
    
    msg(MSG::INFO) << "Finalised successfully" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode AthenaTestHarness::execute()
{
    
   	m_test->processEvent(*m_trigDec); 
    return StatusCode::SUCCESS;
}


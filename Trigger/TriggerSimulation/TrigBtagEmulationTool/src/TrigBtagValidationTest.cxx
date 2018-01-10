/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagValidationTest.h"
#include "GaudiKernel/Property.h"
#include "PATCore/TAccept.h"

using std::string;

//**********************************************************************
namespace Trig{

  TrigBtagValidationTest::TrigBtagValidationTest(const std::string& name, 
					       ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ),
      m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
      m_emulationTool("Trig::TrigBtagEmulationTool/TrigBtagEmulationTool",this) {
    declareProperty("TrigBtagEmulationTool",m_emulationTool);
    declareProperty("ToBeEmulatedTriggers",m_toBeEmulatedTriggers);
  }
  
  //**********************************************************************
  
  TrigBtagValidationTest::~TrigBtagValidationTest() { }
  
  //**********************************************************************
  
  StatusCode TrigBtagValidationTest:: initialize() {
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

    ATH_MSG_INFO("Retrieved tools...");

    // CHAIN CONFIGURATION IN ATHENA IS PERFORMED VIA JOB OPTION

    ATH_MSG_INFO("Initializing TrigBtagEmulationTool ...");
    if( m_emulationTool->initialize().isFailure() ) {
      ATH_MSG_ERROR( "Unable to initialize TrigBtagEmulationTool" );
      return StatusCode::FAILURE;
    }


    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++) {
      std::string triggerName = m_toBeEmulatedTriggers.at(index);
      m_counters[ triggerName.c_str() ] = std::make_tuple( 0,0,0,0,0 );
    }
    
    return StatusCode::SUCCESS;
  }
  
  //**********************************************************************
  
  StatusCode TrigBtagValidationTest::finalize() {

    ATH_MSG_INFO("TrigBtagEmulationTool::finalize()");
    ATH_MSG_INFO("Trigger Validation Outcome");

    long int TotalMismatches = 0;
    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++) {
      std::string triggerName = m_toBeEmulatedTriggers.at(index);
      std::string message = Form("  --- Trigger %s : TDT = %ld [EMUL = %ld] : mismatches = %ld [TDT=1;EMUL=0 = %ld] [TDT=0;EMUL=1 = %ld]",
				 triggerName.c_str(),
				 std::get<TDT>( m_counters[ triggerName.c_str() ] ),
				 std::get<EMUL>( m_counters[ triggerName.c_str() ] ),
				 std::get<mismatchesTOT>( m_counters[ triggerName.c_str() ] ),
				 std::get<mismatchesTDT1EMUL0>( m_counters[ triggerName.c_str() ] ),
				 std::get<mismatchesTDT0EMUL1>( m_counters[ triggerName.c_str() ] ) );
				 
      ATH_MSG_INFO(message.c_str());
      TotalMismatches += std::get<mismatchesTOT>( m_counters[ triggerName.c_str() ] );
    }
    ATH_MSG_INFO("Total Mismatches : " << TotalMismatches);
    
    return StatusCode::SUCCESS;
  }
  
  //**********************************************************************
  StatusCode TrigBtagValidationTest::execute() {

    ATH_MSG_DEBUG("TrigBtagEmulationTool::execute()");

    // CHECK RESUT FOR INDIVIDUAL CHAINS
    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++) {
      std::string triggerName = m_toBeEmulatedTriggers.at(index);
      std::string message = Form( "TRIGGER=%s ",triggerName.c_str() );	
      
      bool passTDT  = m_trigdec->isPassed( triggerName );
      bool passEmul = m_emulationTool->isPassed( triggerName );

      if (passTDT) std::get<TDT>( m_counters[ triggerName.c_str() ] )++;
      if (passEmul) std::get<EMUL>( m_counters[ triggerName.c_str() ] )++;
      if (passTDT != passEmul) std::get<mismatchesTOT>( m_counters[ triggerName.c_str() ] )++;
      if (passTDT && !passEmul) std::get<mismatchesTDT1EMUL0>( m_counters[ triggerName.c_str() ] )++;
      if (!passTDT && passEmul) std::get<mismatchesTDT0EMUL1>( m_counters[ triggerName.c_str() ] )++;
       
      message += Form("TDT=%d EMUL=%d MISMATCH=%d",passTDT?1:0,passEmul?1:0,passTDT != passEmul?1:0);
      ATH_MSG_INFO( message.c_str() );
    }
    
    ATH_MSG_INFO( "" );
    
    return StatusCode::SUCCESS;
  }

}///namespace
//**********************************************************************

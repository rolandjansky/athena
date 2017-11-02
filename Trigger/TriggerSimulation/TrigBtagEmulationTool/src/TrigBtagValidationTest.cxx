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
      m_emulationTool("Trig::TrigBtagEmulationTool/TrigBtagEmulationTool",this)
  {
    declareProperty("TrigBtagEmulationTool",m_emulationTool);
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

    // CHAIN CONFIGURATION IN ATHENA IS PERFORMED VIA JOB OPTION
    // EXAMPLE OF CONFIGURATION OF ADDITIONAL EMULATED CHAINS IN ROOTCORE, ONLY VIA SET PROPERTY
    m_toBeEmulatedTriggers.push_back("L1_MJJ-100");
    m_toBeEmulatedTriggers.push_back("HLT_10j40_L14J15");
    m_toBeEmulatedTriggers.push_back("HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085_split_L14J15.0ETA25");

#ifdef XAOD_STANDALONE
    std::vector< std::vector< std::string > > emulatedChainDescription; 
    {
      std::vector< std::string > test;
      test.push_back("L1_MJJ-100");
      test.push_back("EMUL_L1_MJJ-100");
      emulatedChainDescription.push_back( test );
    }
    {
      std::vector< std::string > test;
      test.push_back("HLT_10j40_L14J15");
      test.push_back("EMUL_L1_4J15");
      test.push_back("EMUL_HLT_10j40");
      emulatedChainDescription.push_back( test );
    }
    {
      std::vector< std::string > test;
      test.push_back("HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085_split_L14J15.0ETA25");
      test.push_back("EMUL_L1_4J15.0ETA25");
      test.push_back("EMUL_2j15_gsc35_bmv2c1070_split");
      test.push_back("EMUL_4j15_gsc35_bmv2c1085_split");
      emulatedChainDescription.push_back( test );
    }

    if( m_emulationTool->setProperty("EmulatedChainDefinitions",emulatedChainDescription).isFailure() ) {
      ATH_MSG_ERROR( "Unable to add Trigger Chains to TrigBtagEmulation Tool" );
      return sc;
    }
#endif

    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++)
      {
	std::string name = m_toBeEmulatedTriggers.at(index);
	m_counterEmulation[ name.c_str() ]            = 0;
	m_counterTDT[ name.c_str() ]                  = 0;
	m_counterMismatches_tot[ name.c_str() ]       = 0;
	m_counterMismatches_TDT1EMUL0[ name.c_str() ] = 0;
	m_counterMismatches_TDT0EMUL1[ name.c_str() ] = 0;
      }

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode TrigBtagValidationTest::finalize() {

    ATH_MSG_INFO("TrigBtagEmulationTool::finalize()");
    ATH_MSG_INFO("Trigger Validation Outcome");

    long int TotalMismatches = 0;
    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++)
      {
	std::string name = m_toBeEmulatedTriggers.at(index);
	std::string message = Form("  --- Trigger %s : TDT = %ld [Emulation = %ld] : mismatches = %ld [TDT=1;EMUL=0 = %ld] [TDT=0;EMUL=1 = %ld]",
				   name.c_str(),
				   m_counterTDT[ name.c_str() ],
				   m_counterEmulation[ name.c_str() ],
				   m_counterMismatches_tot[ name.c_str() ],
				   m_counterMismatches_TDT1EMUL0[ name.c_str() ],
				   m_counterMismatches_TDT0EMUL1[ name.c_str() ]);
	ATH_MSG_INFO(message.c_str());
	TotalMismatches += m_counterMismatches_tot[ name.c_str() ];
      }
    ATH_MSG_INFO("Total Mismatches : " << TotalMismatches);

    return StatusCode::SUCCESS;
  }

  //**********************************************************************
  StatusCode TrigBtagValidationTest::execute() {

    ATH_MSG_DEBUG("TrigBtagEmulationTool::execute()");

    // CHECK RESUT FOR INDIVIDUAL CHAINS
    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++)
      {
	std::string name = m_toBeEmulatedTriggers.at(index);
	std::string message = Form( "TRIGGER=%s ",name.c_str() );	

	bool passTDT  = m_trigdec->isPassed( name );
	bool passEmul = m_emulationTool->isPassed( name );
	
	if (passTDT)  m_counterTDT[ name.c_str() ]++;
	if (passEmul) m_counterEmulation[ name.c_str() ]++;
	if (passTDT != passEmul) m_counterMismatches_tot[ name.c_str() ]++;
	if (passTDT && !passEmul) m_counterMismatches_TDT1EMUL0[ name.c_str() ]++;
	if (!passTDT && passEmul) m_counterMismatches_TDT0EMUL1[ name.c_str() ]++;

	message += Form("TDT=%d EMUL=%d MISMATCH=%d",passTDT?1:0,passEmul?1:0,passTDT != passEmul?1:0);
	ATH_MSG_INFO( message.c_str() );
      }

    ATH_MSG_INFO( "" );
    
    return StatusCode::SUCCESS;
  }

}///namespace
//**********************************************************************

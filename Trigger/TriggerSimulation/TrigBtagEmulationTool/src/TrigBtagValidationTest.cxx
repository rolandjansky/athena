// TrigBtagValidationTest.cxx

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

    // EXAMPLE OF CONFIGURATION OF ADDITIONAL EMULATED CHAINS

    m_toBeEmulatedTriggers.push_back("L1_MJJ-400");
    {
      std::vector< std::string > test;
      test.push_back("L1_MJJ-400");
      test.push_back("EMUL_L1_MJJ-400");
      m_emulationTool->addEmulatedChain( test );
    }

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

    // EXECUTE TOOL
    if( m_emulationTool->execute().isFailure() )
      {
	ATH_MSG_ERROR( "Unable to execute TrigBtagEmulationTool" );
	return StatusCode::FAILURE;
      }

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

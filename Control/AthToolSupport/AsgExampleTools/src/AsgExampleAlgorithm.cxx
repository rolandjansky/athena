// AsgExampleAlgorithm.cxx 

#include "AsgExampleAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "AsgExampleTools/IAsgHelloTool.h"

using std::string;

//**********************************************************************

AsgExampleAlgorithm::
AsgExampleAlgorithm(const std::string& name, 
                    ISvcLocator* pSvcLocator )
: ::AthAlgorithm( name, pSvcLocator ) {
  declareProperty( "Tools", m_asgtools);
}

//**********************************************************************

AsgExampleAlgorithm::~AsgExampleAlgorithm() { }

//**********************************************************************

StatusCode AsgExampleAlgorithm::initialize() {
  StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  sc = m_asgtools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Tool retrieval failed.");
  }
  ATH_MSG_INFO("Tool retrieval completed.");
  unsigned int ntool = m_asgtools.size();
  ATH_MSG_INFO("  Tool count: " << ntool);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ATH_MSG_INFO("    " << m_asgtools[itool]->name());
    m_asgtools[itool]->print();
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode AsgExampleAlgorithm::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode AsgExampleAlgorithm::execute() {  
  ATH_MSG_INFO ("Executing " << name() << "...");
  // Loop over hello tools.
  ATH_MSG_INFO("Looping over ASG tools.");
  string line = "---------------------------------------------------";
  ATH_MSG_INFO(line);
  for ( ToolHandleArray<IAsgHelloTool>::const_iterator itoo=m_asgtools.begin();
        itoo!=m_asgtools.end(); ++itoo ) {
    ToolHandle<IAsgHelloTool> htool = *itoo;
    int jstat = htool->talk();
    if ( jstat != 0 ) ATH_MSG_INFO("Tool returned error " << jstat);
    ATH_MSG_INFO(line);
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

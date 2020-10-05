// AsgExampleAlgorithm.cxx 

#include "AsgExampleAlgorithm.h"
#include "Gaudi/Property.h"
#include "AsgExampleTools/IAsgHelloTool.h"
#include "AsgTools/AsgTool.h"

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
    // Get the tool's message property:
    const asg::AsgTool* tool =
       dynamic_cast< const asg::AsgTool* >( m_asgtools[ itool ].operator->() );
    if( ! tool ) {
       ATH_MSG_ERROR( "The received tool is not an AsgTool?!?" );
       return StatusCode::FAILURE;
    }
    const std::string* msg = tool->getProperty< std::string >( "Message" );
    if( ! msg ) {
       ATH_MSG_WARNING( "Couldn't get the \"Message\" property of tool "
                        << tool->name() );
    } else {
       ATH_MSG_INFO( "    Its \"Message\" property: " << *msg );
    }
    // Try some invalid retrievals, for fun:
    tool->getProperty< std::string >( "UnknownProperty" );
    tool->getProperty< int >( "Message" );
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

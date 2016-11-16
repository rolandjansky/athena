// AsgHelloTool.cxx

#include "AsgExampleTools/AsgHelloTool.h"

//**********************************************************************

AsgHelloTool::
AsgHelloTool( const std::string& myname )
: AsgTool(myname) {
  declareProperty("Message", m_msg);
}

//**********************************************************************

StatusCode AsgHelloTool::initialize() {

   ATH_MSG_VERBOSE( "Initialising tool " << name() );

   return StatusCode::SUCCESS;
}

//**********************************************************************

int AsgHelloTool::talk() const {
  ATH_MSG_DEBUG("Talking...");
  ATH_MSG_INFO(m_msg);
  // Create and execute a temporary tool.
  if ( name() != "tmphello" ) {
    AsgHelloTool tmptool("tmphello");
    if( ! tmptool.setProperty("Message", "Hi from the temporary tool.").isSuccess() ) {
       ATH_MSG_ERROR( "Failed to set property on temporary tool" );
       return 1;
    }
    tmptool.talk();
  }
  return 0;
}
//**********************************************************************

void AsgHelloTool::print() const {
  AsgTool::print();
  ATH_MSG_INFO("   Message: \"" << m_msg << "\"");
  ATH_MSG_INFO("  MsgLevel: " << MSG::name( msg().currentLevel() ) );
  ATH_MSG_ALWAYS("This message always appears.");
}

//**********************************************************************

// AsgHelloTool.cxx

#include "AsgExampleTools/AsgHelloTool.h"

//**********************************************************************

AsgHelloTool::
AsgHelloTool(std::string myname)
: AsgTool(myname) {
  declareProperty("Message", m_msg);
}

//**********************************************************************

int AsgHelloTool::talk() const {
  ATH_MSG_DEBUG("Talking...");
  ATH_MSG_INFO(m_msg);
  return 0;
}
//**********************************************************************

void AsgHelloTool::print() const {
  AsgTool::print();
  ATH_MSG_INFO("   Message: \"" << m_msg << "\"");
  ATH_MSG_INFO("  MsgLevel: " << msg_level_name());
  ATH_MSG_VERBOSE("Test getProperty returns the same message: \""
                  << *getProperty<std::string>("Message") << "\"");
}

//**********************************************************************

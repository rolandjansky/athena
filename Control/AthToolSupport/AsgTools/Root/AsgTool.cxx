// AsgTool.cxx

#include "AsgTools/AsgTool.h"

using asg::AsgTool;

#include <iostream>

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
AsgTool::EventPtr AsgTool::event(xAOD::TEvent* a_pevt, bool set) {
  static AsgTool::EventPtr pevt = 0;
  if ( set ) pevt = new asg::SgTEvent(a_pevt);
  return pevt;
}
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
AsgTool::AsgTool(std::string aname)
: asg::Named(aname), m_ppropmgr(0), m_manage_propmgr(false), m_msg(this) {
  //std::cout << "AsgTool::ctor: Prop mgr: " << m_ppropmgr << std::endl;
  if ( m_ppropmgr == 0 ) {
    m_ppropmgr = new PropertyMgr;
    m_manage_propmgr = true;
  }
  msg().setLevel(MSG::INFO);
  declareProperty("OutputLevel", m_msg.mutable_level());
}
#endif

#ifdef ASGTOOL_ATHENA
AsgTool::AsgTool(std::string name)
: AthAlgTool("", name, Gaudi::svcLocator()->service("ToolSvc")) { }
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
AsgTool::~AsgTool() {
  if ( m_manage_propmgr ) delete m_ppropmgr;
}
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
AsgTool::EventPtr AsgTool::evtStore() const {
  asg::SgTEvent* pevt = event();
  if ( pevt != 0 ) return pevt;
  static SgTEvent* pevtx = 0;
  if ( pevtx == 0 ) {
    std::cout << "AsgTool::evtStore: Retrieving xAOD current event." << std::endl;
    pevtx = SgTEvent::currentEvent();
    if ( pevtx == 0 ) {
      std::cout << "AsgTool::evtStore: Current event not found." << std::endl;
    } else {
      std::cout << "AsgTool::evtStore: Current event found." << std::endl;
    }
    std::cout << "AsgTool::evtStore: Current event: " << pevtx->event() << std::endl;
  }
  return pevtx;
}
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
const PropertyMgr* AsgTool::getPropertyMgr() const {
  return m_ppropmgr;
}
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
PropertyMgr* AsgTool::getPropertyMgr() {
  return m_ppropmgr;
}
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
bool AsgTool::msgLvl(MSG::Level) const {
  return false;
}
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
MsgStream& AsgTool::msg() const {
  return m_msg;
}
#endif

//**********************************************************************

#ifdef ASGTOOL_STANDALONE
MsgStream& AsgTool::msg(MSG::Level lvl) const {
  m_msg.setLevel(lvl);
  return m_msg;
}
#endif

//**********************************************************************

std::string AsgTool::msg_level_name() const {
  return MSG::name(msg().level());
}

//**********************************************************************

#ifndef ASGTOOL_ATHENA
StatusCode AsgTool::initialize() {
  if ( getPropertyMgr() == 0 ) {
    ATH_MSG_ERROR("Property manager not found.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
#endif

//**********************************************************************

void AsgTool::print() const {
  ATH_MSG_INFO("AsgTool " << name() << " @ " << this);
}

//**********************************************************************

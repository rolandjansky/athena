// SgTEvent.cxx


#include "AsgTools/SgTEvent.h"
#include <iostream>
#ifdef ASGTOOL_TEVENT 
#include "xAODRootAccess/TActiveEvent.h"
#endif

using std::string;
using std::cout;
using std::endl;
using asg::SgTEvent;
#ifdef ASGTOOL_TEVENT 
using xAOD::TActiveEvent;
using xAOD::TVirtualEvent;
#endif

//**********************************************************************

SgTEvent* SgTEvent::currentEvent() {
#ifdef ASGTOOL_TEVENT
  const string myname = "SgTEvent::currentEvent: ";
  TVirtualEvent* pvevt = TActiveEvent::GetEvent();
  if ( pvevt == 0 ) {
    cout << myname << "TVirtualEvent not found." << endl;
    return 0;
  }
  TEvent* pevt = dynamic_cast<TEvent*>(pvevt);
  if ( pevt == 0 ) {
    cout << myname << "TEvent not found." << endl;
    return 0;
  }
  return new SgTEvent(pevt);
#else
  return new SgTEvent(0);
#endif
}

//**********************************************************************

SgTEvent::SgTEvent(TEvent* pevm) : m_pevm(pevm) { }

//**********************************************************************

TEvent* SgTEvent::event() { return m_pevm; }

//**********************************************************************

// SgTEvent.h

#ifndef asg_SgTEvent_H
#define asg_SgTEvent_H

// David Adams
// January 2014
//
// Wrapper for TEvent to make it look like StoreGate.
// This is not used in Athena.

#include "AsgTools/AsgToolsConf.h"

#ifndef ASGTOOL_STANDALONE
#error("This header should only be used for standalone ASG")
#endif

#include <string>
#include "AsgTools/StatusCode.h"
#ifdef ASGTOOL_TEVENT
#include "xAODRootAccess/TEvent.h"
#else
namespace xAOD {
 class TEvent;
}
#endif

#include <iostream>

using xAOD::TEvent;

namespace asg {

class SgTEvent {

public:  // methods

  // Return an object pointing to the current event.
  static SgTEvent* currentEvent();

  // Ctor from a TEvent object.
  SgTEvent(TEvent* pevm);

  // Return the underlying event manager.
  TEvent* event();

  // Check presence of a container.
  template<typename T>
  bool contains(std::string cname);

  // Retrieve a container.
  template<typename T>
  T* retrieve(std::string cname);
  template<typename T>
  StatusCode retrieve(T*& pobj, std::string cname);
  
  // Record container.
  template<typename T>
  StatusCode record(T* pobj, std::string cname);
  
private:  //data

  TEvent* m_pevm;

};

//**********************************************************************

template<typename T>
bool asg::SgTEvent::contains(std::string cname) {
  if ( retrieve<const T>(cname) ) return true;
  return false;
}

//**********************************************************************

template<typename T>
T* asg::SgTEvent::retrieve(std::string cname) {
  //std::cout << "SgTEvent::retrieve1: Event manager: " << m_pevm << std::endl;
  //std::cout << "SgTEvent::retrieve1: " << cname << std::endl;
  if ( m_pevm == 0 ) return 0;
  T* pobj = 0;
#ifdef ASGTOOL_TEVENT
  m_pevm->Retrieve(pobj, cname);
#endif
  return pobj;
}

//**********************************************************************

template<typename T>
StatusCode asg::SgTEvent::retrieve(T*& pobj, std::string cname) {
  //std::cout << "SgTEvent::retrieve2: Event manager: " << m_pevm << std::endl;
  //std::cout << "SgTEvent::retrieve2: " << cname << " @ " << pobj << std::endl;
  if ( m_pevm == 0 ) return 0;
#ifdef ASGTOOL_TEVENT
  bool rstat = m_pevm->Retrieve(pobj, cname);
  if ( rstat ) return StatusCode::SUCCESS;
#endif
  return StatusCode::FAILURE;
}

//**********************************************************************

template<typename T>
StatusCode asg::SgTEvent::record(T* pobj, std::string cname) {
#ifdef ASGTOOL_TEVENT
  bool rstat = m_pevm->Record(pobj, cname);
  if ( rstat ) return StatusCode::SUCCESS;
#endif
  return StatusCode::FAILURE;
}

//**********************************************************************

}  // end asg namespace

#endif

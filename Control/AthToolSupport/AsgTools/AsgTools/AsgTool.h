// AsgTool.h

#ifndef AsgTool_H
#define AsgTool_H

// David Adams
// January 2014
//
// This is the base for ASG dual-use tools.
// It provides services to access properties and access event data.

#include "AsgTools/AsgToolsConf.h"

#include "AsgTools/IAsgTool.h"
#include "AsgTools/MsgLevel.h"
#ifdef ASGTOOL_STANDALONE
#include "AsgTools/Named.h"
#include "AsgTools/PropertyMgr.h"
#include "AsgTools/MsgStream.h"
#include "AsgTools/SgTEvent.h"
#include "AsgTools/MsgStreamMacros.h"
#include "AsgTools/ToolHandle.h"
#endif
#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IProperty.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthAlgTool.h"
#endif
#include <iostream>

namespace asg {

class AsgTool
: public virtual IAsgTool
#ifdef ASGTOOL_STANDALONE
, public asg::Named
#endif
#ifdef ASGTOOL_ATHENA
, public AthAlgTool
#endif
{

public:

#ifdef ASGTOOL_STANDALONE
  typedef asg::SgTEvent* EventPtr;
#endif

public:   // static methods

#ifdef ASGTOOL_STANDALONE
  // Set/get the ASG statndalone event manager.
  static EventPtr event(xAOD::TEvent* pevt =0, bool set=false);
#endif


public:

  // Ctor specifying the class name.
  AsgTool(std::string myname);

#ifdef ASGTOOL_STANDALONE
  // Dtor.
  virtual ~AsgTool();

  // Access the event store.
  virtual EventPtr evtStore() const;

  // Declare property with Athena or Root.
  template<class T>
  Property* declareProperty(const std::string& name, T& loc);

  // Set property with Root.
  template<class T> void setProperty(const std::string& name, const T& val);

  // Get the property manager.
  PropertyMgr* getPropertyMgr();
  const PropertyMgr* getPropertyMgr() const;

  // Check message level.
  virtual bool msgLvl(MSG::Level) const;

  // Return the message stream.
  virtual MsgStream& msg() const;

  // Return the message stream.
  virtual MsgStream& msg(MSG::Level) const;
#endif

  // Get the value of the property with the specified name.
  // Returns null if property name is not used or is not of type T.
#ifdef ASGTOOL_ATHENA
  using AlgTool::getProperty;
#endif
  template<class T> const T* getProperty(const std::string& name) const;

  // Return the message level name.
  std::string msg_level_name() const;

#ifndef ASGTOOL_ATHENA
  // Opportunity to retrieve tool handles.
  StatusCode initialize();
#endif

  // Print state.
  void print() const;

private:

#ifdef ASGTOOL_STANDALONE
  PropertyMgr* m_ppropmgr;
  bool m_manage_propmgr;
  mutable MsgStream m_msg;
#endif

};

#ifdef ASGTOOL_STANDALONE
template<class T>
Property* AsgTool::declareProperty(const std::string& name, T& loc) {
  if ( m_ppropmgr == 0 ) {
    std::cout << "AsgTool::declareProperty: Property manager is NULL!" << std::endl;
    return 0;
  }
  return m_ppropmgr->declareProperty(name, loc);
}
#endif

#ifdef ASGTOOL_STANDALONE
template<class T>
void AsgTool::setProperty(const std::string& name, const T& val) {
  if ( m_ppropmgr == 0 ) {
    std::cout << "AsgTool::declareProperty: Property manager is NULL!" << std::endl;
    return;
  }
  m_ppropmgr->setProperty(name, val);
}
#endif

template<class T>
const T* AsgTool::getProperty(const std::string& name) const {
#ifdef ASGTOOL_STANDALONE
  if ( m_ppropmgr == 0 ) {
    std::cout << "AsgTool::declareProperty: Property manager is NULL!" << std::endl;
    return 0;
  }
  return 0;
#else
  Property* pprop = Gaudi::Utils::getProperty(this, name);
  if ( pprop == 0 ) return 0;
  PropertyWithValue<T>* pproptyped = dynamic_cast<PropertyWithValue<T>*>(pprop);
  if ( pproptyped == 0 ) return 0;
  return &pproptyped->value();
#endif
}

}  // end asg namespace

#endif

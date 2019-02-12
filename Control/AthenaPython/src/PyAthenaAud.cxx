///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaAud.cxx 
// Implementation file for class PyAthena::Aud
// Author: S.Binet<binet@cern.ch>
// Modified: Wim Lavrijsen <WLavrijsen@lbl.gov>
/////////////////////////////////////////////////////////////////// 

// Python includes
#include "Python.h"

// PyROOT includes
#include "TPython.h"

// AthenaPython includes
#include "AthenaPython/PyAthenaUtils.h"
#include "AthenaPython/PyAthenaAud.h"
#include "AthenaPython/PyAthenaGILStateEnsure.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/System.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPython/IPyComponentMgr.h"

namespace PyAthena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

Aud::Aud( const std::string& name, ISvcLocator* svcLocator ) :
  ::Auditor( name, svcLocator ),
  m_self   ( nullptr )
{}

// Destructor
///////////////
Aud::~Aud()
{ 
  if ( m_self ) {
    PyGILStateEnsure ensure;
    Py_DECREF( m_self );
    m_self = nullptr;
  }
}

// Athena Auditor's Hooks
////////////////////////////
StatusCode 
Aud::initialize()
{
  return PyAthena::callPyMethod( m_self, "sysInitialize" );
}

StatusCode
Aud::sysInitialize()
{
  ServiceHandle<IPyComponentMgr> pyMgr
    ( "PyAthena::PyComponentMgr/PyComponentMgr", name() );
  if ( !pyMgr.retrieve().isSuccess() ) {
    return StatusCode::FAILURE;
  }

  // first retrieve our python object cousin...
  m_self = pyMgr->pyObject( this );

  if ( m_self == Py_None ) {
    return StatusCode::FAILURE;
  }

  // re-route to usual sysInit...
  return ::Auditor::sysInitialize();
}

StatusCode 
Aud::finalize()
{
  return PyAthena::callPyMethod( m_self, "sysFinalize" );
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const char*
Aud::typeName() const
{ 
  static const std::string tname = System::typeinfoName(typeid(*this));
  return tname.c_str();
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void 
Aud::before(StandardEventType evt, INamedInterface* comp)
{
  py_before (evt, comp->name());
}

void 
Aud::before(StandardEventType evt, const std::string& comp)
{
  py_before (evt, comp);
}

void 
Aud::before(CustomEventTypeRef evt, INamedInterface* comp)
{
  py_before (evt, comp->name());
}

void
Aud::before(CustomEventTypeRef evt, const std::string& comp)
{
  py_before (evt, comp);
}

void 
Aud::after(StandardEventType evt, INamedInterface* comp, const StatusCode& sc)
{
  py_after (evt, comp->name(), sc);
}

void 
Aud::after(StandardEventType evt, const std::string& comp, const StatusCode& sc)
{
  py_after (evt, comp, sc);
}

void 
Aud::after(CustomEventTypeRef evt, INamedInterface* comp, const StatusCode& sc)
{
  py_after (evt, comp->name(), sc);
}

void 
Aud::after(CustomEventTypeRef evt, const std::string& comp, 
	   const StatusCode& sc)
{
  py_after (evt, comp, sc);
}

// Obsolete methods
void 
Aud::beforeInitialize(INamedInterface* comp)
{
  py_before (IAuditor::Initialize, comp->name());
}

void 
Aud::afterInitialize(INamedInterface* comp)
{
  py_after (IAuditor::Initialize, comp->name(), StatusCode::SUCCESS);
}

void 
Aud::beforeReinitialize(INamedInterface* comp)
{
  py_before (IAuditor::ReInitialize, comp->name());
}

void 
Aud::afterReinitialize(INamedInterface* comp)
{
  py_after (IAuditor::ReInitialize, comp->name(), StatusCode::SUCCESS);
}

void 
Aud::beforeExecute(INamedInterface* comp)
{
  py_before (IAuditor::Execute, comp->name());
}

void 
Aud::afterExecute(INamedInterface* comp, const StatusCode& sc)
{
  py_after (IAuditor::Execute, comp->name(), sc);
}

void 
Aud::beforeFinalize(INamedInterface* comp)
{
  py_before (IAuditor::Finalize, comp->name());
}

void 
Aud::afterFinalize(INamedInterface* comp)
{
  py_after (IAuditor::Finalize, comp->name(), StatusCode::SUCCESS);
}

void 
Aud::beforeBeginRun(INamedInterface* comp)
{
  py_before (IAuditor::BeginRun, comp->name());
}

void 
Aud::afterBeginRun(INamedInterface* comp)
{
  py_after (IAuditor::BeginRun, comp->name(), StatusCode::SUCCESS);
}

void
Aud::beforeEndRun(INamedInterface* comp)
{
  py_before (IAuditor::EndRun, comp->name());
}

void 
Aud::afterEndRun(INamedInterface* comp)
{
  py_after (IAuditor::EndRun, comp->name(), StatusCode::SUCCESS);
}

/// Audit the start of a standard "event".
void 
Aud::py_before (IAuditor::StandardEventType evt, const std::string& component)
{
  const char* evtname = 0;
  switch (evt) {
  case Initialize:   evtname = "initialize";   break;
  case ReInitialize: evtname = "reinitialize"; break;
  case Execute:      evtname = "execute";      break;
  case BeginRun:     evtname = "beginrun";     break;
  case EndRun:       evtname = "endrun";       break;
  case Finalize:     evtname = "finalize";     break;
  case Start:        evtname = "start";        break;
  case Stop:         evtname = "stop";         break;
  case ReStart:      evtname = "restart";      break;
  }
  return PyAthena::pyAudit (m_self, "before", evtname, component.c_str());
}

void 
Aud::py_before(IAuditor::CustomEventTypeRef evt, const std::string& component)
{
  return PyAthena::pyAudit (m_self, "before", evt.c_str(), component.c_str());
}

void 
Aud::py_after(IAuditor::StandardEventType evt,
	      const std::string& component,
	      const StatusCode& sc)
{
  const char* evtname = 0;
  switch (evt) {
  case Initialize:   evtname = "initialize";   break;
  case ReInitialize: evtname = "reinitialize"; break;
  case Execute:      evtname = "execute";      break;
  case BeginRun:     evtname = "beginrun";     break;
  case EndRun:       evtname = "endrun";       break;
  case Finalize:     evtname = "finalize";     break;
  case Start:        evtname = "start";        break;
  case Stop:         evtname = "stop";         break;
  case ReStart:      evtname = "restart";      break;
  }
  return PyAthena::pyAudit (m_self, "after", evtname, component.c_str(), sc);
}

void 
Aud::py_after(IAuditor::CustomEventTypeRef evt,
	      const std::string& component,
	      const StatusCode& sc)
{
  return PyAthena::pyAudit (m_self, 
			    "after", evt.c_str(), component.c_str(), sc);
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

bool
Aud::setPyAttr( PyObject* o )
{
  // now we tell the PyObject which C++ object it is the cousin of.
  PyGILStateEnsure ensure;
  PyObject* pyobj = TPython::ObjectProxy_FromVoidPtr
    ( (void*)this, this->typeName() );
  if ( !pyobj ) {
    PyErr_Clear();
    // try PyAthena::Aud
    pyobj = TPython::ObjectProxy_FromVoidPtr ((void*)this, "PyAthena::Aud");
    MsgStream msg( msgSvc(), name() );
    msg << MSG::INFO
        << "could not dyncast component [" << name() << "] to a python "
        << "object of type [" << this->typeName() << "] (probably a missing "
        << "dictionary)" << endmsg
        << "fallback to [PyAthena::Aud]..."
        << endmsg;
  }
  if ( !pyobj ) {
    PyErr_Clear();
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Could not dyncast component ["
        << name() << "] to a pyobject of type [" 
        << this->typeName() << "]"
        << endmsg;
  } else {
    if ( -1 == PyObject_SetAttrString(o, "_cppHandle", pyobj) ) {
      PyErr_Clear();
      MsgStream msg( msgSvc(), name() );
      msg << MSG::WARNING 
          << "Could not attach C++ handle [" << name() << "] to its python "
          << "cousin !"
          << endmsg;
      if ( -1 == PyObject_SetAttrString(o, "_cppHandle", Py_None) ) {
        PyErr_Clear();
        msg << MSG::WARNING
            << "could not attach a dummy C++ handle [" << name() << "] to its "
            << "python cousin !"
            << endmsg;
      }
    } else {
      return true;
    }
  }
  return false;
}

} //> end namespace PyAthena

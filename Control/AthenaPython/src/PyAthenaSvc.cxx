///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaSvc.cxx 
// Implementation file for class PyAthena::Svc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Python includes
#include "Python.h"

// PyROOT includes
#include "TPython.h"

// AthenaPython includes
#include "AthenaPython/PyAthenaUtils.h"
#include "AthenaPython/PyAthenaSvc.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPython/IPyComponentMgr.h"


namespace PyAthena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
Svc::Svc( const std::string& name, ISvcLocator* svcLocator ) :
  SvcBase_t( name, svcLocator ),
  m_self   ( 0 )
{}

// Destructor
///////////////
Svc::~Svc()
{ 
  ATH_MSG_DEBUG("Calling destructor");
  Py_XDECREF( m_self );
}

// Athena service's Hooks
////////////////////////////
StatusCode 
Svc::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysInitialize" );
}

StatusCode 
Svc::reinitialize()
{
  ATH_MSG_INFO("Re-Initializing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysReinitialize" );
}

StatusCode 
Svc::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysFinalize" );
}

StatusCode 
Svc::beginRun()
{
  return PyAthena::callPyMethod( m_self, "sysBeginRun" );
}

StatusCode 
Svc::endRun()
{
  return PyAthena::callPyMethod( m_self, "sysEndRun" );
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
Svc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( interfaceID().versionMatch(riid) )    {
    *ppvInterface = static_cast<PyAthena::Svc*>(this);
//   } else if ( 0 != m_self && m_self != Py_None ) {
//     return PyAthena::queryInterface( m_self, riid, ppvInterface );
  } else  {
    // Interface is not directly available: try out a base class
    return SvcBase_t::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

const InterfaceID& 
Svc::interfaceID() { 
  static const InterfaceID IID("PyAthena::Svc", 1, 0);
  return IID; 
}

StatusCode
Svc::sysInitialize()
{
  ServiceHandle<IPyComponentMgr> pyMgr
    ( "PyAthena::PyComponentMgr/PyComponentMgr", name() );
  if ( !pyMgr.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve service [" << pyMgr.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  // first retrieve our python object cousin...
  m_self = pyMgr->pyObject( this );

  if ( m_self == Py_None ) {
    ATH_MSG_ERROR("Wrapped PyObject is NONE !");
    return StatusCode::FAILURE;
  }

  // re-route to usual sysInit...
  return SvcBase_t::sysInitialize();
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const char*
Svc::typeName() const
{ 
  static const std::string tname = System::typeinfoName(typeid(*this));
  return tname.c_str();
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void
Svc::handle (const Incident& inc)
{
  if (0 == PyObject_HasAttrString (m_self, (char*)"handle")) {
    // python side does not implement 'handle'. Fair enough.
    // XXX FIXME: could say something though: we have been registered as 
    //            listener, so there might be some kind of inconsistency...
    return;
  }

  PyObject *o = TPython::ObjectProxy_FromVoidPtr ((void*)(&inc), "Incident");
  if (0 == o) {
    Py_XDECREF (o);
    PyAthena::throw_py_exception();
  }
    
  PyObject *res = PyObject_CallMethod (m_self, 
				       (char*)"handle",
				       (char*)"O", o);
  if (0 == res) {
    Py_XDECREF (res);
    Py_DECREF  (o);
    PyAthena::throw_py_exception();
  }

  Py_DECREF (res);
  Py_DECREF (o);
  return;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

bool
Svc::setPyAttr( PyObject* o )
{
  // now we tell the PyObject which C++ object it is the cousin of.
  PyObject* pyobj = TPython::ObjectProxy_FromVoidPtr
    ( (void*)this, this->typeName() );
  if ( !pyobj ) {
    PyErr_Clear();
    // try PyAthena::Svc
    pyobj = TPython::ObjectProxy_FromVoidPtr ((void*)this, "PyAthena::Svc");
    ATH_MSG_INFO
      ("could not dyncast component [" << name() << "] to a python "
       << "object of type [" << this->typeName() << "] (probably a missing "
       << "dictionary)" << endmsg
       << "fallback to [PyAthena::Svc]...");
  }
  if ( !pyobj ) {
    PyErr_Clear();
    ATH_MSG_WARNING
      ("Could not dyncast component [" << name() << "] to a pyobject of type [" 
       << this->typeName() << "]");
  } else {
    if ( -1 == PyObject_SetAttrString(o, "_cppHandle", pyobj) ) {
      PyErr_Clear();
      ATH_MSG_WARNING
        ("Could not attach C++ handle [" << name() << "] to its python "
         << "cousin !");
      if ( -1 == PyObject_SetAttrString(o, "_cppHandle", Py_None) ) {
        PyErr_Clear();
        ATH_MSG_WARNING
          ("could not attach a dummy C++ handle [" << name() << "] to its "
           << "python cousin !");
      }
    } else {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthenaPython


///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tool.cxx 
// Implementation file for class Tool
// Author: S.Binet<binet@cern.ch>
// Modified: Wim Lavrijsen <WLavrijsen@lbl.gov>
/////////////////////////////////////////////////////////////////// 

// Python includes
#include "Python.h"

// PyROOT includes
#include "TPython.h"

// AthenaPython includes
#include "AthenaPython/PyAthenaTool.h"
#include "AthenaPython/PyAthenaUtils.h"
#include "AthenaPython/PyAthenaGILStateEnsure.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPython/IPyComponentMgr.h"

namespace PyAthena {


// Constructors
////////////////
Tool::Tool( const std::string& type, 
	    const std::string& name, 
	    const IInterface* parent ) : 
  base_class( type, name, parent ),
  m_self    ( nullptr )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
Tool::~Tool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
  if ( m_self ) {
    PyGILStateEnsure ensure;
    Py_DECREF( m_self );
    m_self = nullptr;
  }
}

// Athena AlgTool's Hooks
////////////////////////////
StatusCode Tool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysInitialize" );
}

StatusCode Tool::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysFinalize" );
}

StatusCode
Tool::sysInitialize()
{
  ServiceHandle<IPyComponentMgr> pyMgr
    ( "PyAthena::PyComponentMgr/PyComponentMgr", 
      name() );
  if ( !pyMgr.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Could not retrieve service [" << pyMgr.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  // first retrieve our python object cousin...
  m_self = pyMgr->pyObject( this );

  if ( m_self == Py_None ) {
    ATH_MSG_ERROR("Wrapped PyObject is NONE !");
    return StatusCode::FAILURE;
  }

  // re-route to usual sysInit...
  return ToolBase_t::sysInitialize();
}


const char*
Tool::typeName() const
{ 
  static const std::string tname = System::typeinfoName(typeid(*this));
  return tname.c_str();
}


bool
Tool::setPyAttr( PyObject* o )
{
  // now we tell the PyObject which C++ object it is the cousin of.
  PyGILStateEnsure ensure;
  PyObject* pyobj = TPython::ObjectProxy_FromVoidPtr
    ( (void*)this, this->typeName() );
  if ( !pyobj ) {
    PyErr_Clear();
    // try PyAthena::Tool
    pyobj = TPython::ObjectProxy_FromVoidPtr ((void*)this, "PyAthena::Tool");
    ATH_MSG_INFO
      ("could not dyncast component [" << name() << "] to a python "
       << "object of type [" << this->typeName() << "] (probably a missing "
       << "dictionary)" << endmsg
       << "fallback to [PyAthena::Tool]...");
  }
  if ( !pyobj ) {
    PyErr_Clear();
    ATH_MSG_WARNING("Could not dyncast component ["
                    << name() << "] to a pyobject of type [" 
                    << this->typeName() << "] (missing reflex dict ?)");
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

} //> end namespace PyAthena

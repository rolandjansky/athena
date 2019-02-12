///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyProperty.cxx 
// Implementation file for class PyProperty
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Python
#include <Python.h>

// AthenaPython includes
#include "AthenaPython/PyProperty.h"
#include "AthenaPython/PyAthenaUtils.h"

// STL includes
#include <iostream>

// framework
#include "GaudiKernel/System.h"

// PyRoot
#include <TPython.h>
#include <TPyReturn.h>

namespace {

  class ATLAS_NOT_THREAD_SAFE DeepCopier
  {
    PyObject *m_fct;
  public:
    static DeepCopier* instance ATLAS_NOT_THREAD_SAFE ()
    {
      static DeepCopier self;
      static bool first = true;
      if (first) {
	first = false;
	// make sure the python interpreter has been initialized
	if (!Py_IsInitialized()) {
	  Py_Initialize();
	}

	{// first import pyrootfixes
	  PyObject *module = PyImport_ImportModule 
	    ((char*)"RootUtils.PyROOTFixes");
	  if (!module || !PyModule_Check(module)) {
	    PyAthena::throw_py_exception();
	  }
	  PyObject *pickling = PyDict_GetItemString (PyModule_GetDict (module),
						     (char*)"enable_pickling");
	  Py_XINCREF(pickling);
	  Py_DECREF (module);

	  if (!pickling) {
	    Py_XDECREF(pickling);
	    PyAthena::throw_py_exception();
	  }

	  PyObject *out = PyObject_CallObject (pickling, NULL);
	  Py_DECREF (pickling);
	  if (!out) {
	    PyAthena::throw_py_exception();
	  }
	  Py_DECREF (out);
	}

	{// now import 'copy.deepcopy'
	  PyObject *module = PyImport_ImportModule ((char*)"copy");
	  if (!module || !PyModule_Check(module)) {
	    PyAthena::throw_py_exception();
	  }
	  self.m_fct = PyDict_GetItemString (PyModule_GetDict (module),
					     (char*)"deepcopy");
	  Py_XINCREF(self.m_fct);
	  Py_DECREF (module);
	  if (!self.m_fct) {
	    Py_XDECREF(self.m_fct);
	    PyAthena::throw_py_exception();
	  }
	}
      }
      return &self;
    }
  private:
    DeepCopier() : m_fct (Py_None) {}
  public:
    ~DeepCopier() { Py_XDECREF (m_fct); }
    PyObject *copy (PyObject *in)
    {
      PyObject *out = PyObject_CallFunction (m_fct, (char*)"O", in);
      if (!out) {
	PyAthena::throw_py_exception();
      }
      return out;
    }
  };
}

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

/// Copy constructor: 
PyProperty::PyProperty( const PyProperty& rhs ) :
  PropertyWithHandlers (rhs),
  m_pyobj  (::DeepCopier::instance()->copy (rhs.m_pyobj))
{}

/// Assignment operator: 
PyProperty& 
PyProperty::operator=( const PyProperty& rhs )
{
  if ( this != &rhs ) {
    PropertyWithHandlers::operator= (rhs);
    m_pyobj = ::DeepCopier::instance()->copy (rhs.m_pyobj);
  }
  return *this;
}

/// Constructor with parameters: 
PyProperty::PyProperty(const std::string& name,
		       const std::type_info& type,
		       void* obj) :
  PropertyWithHandlers (name, type),
  m_pyobj  (Py_None)
{
  if (type == typeid(PyObject) ||
      type == typeid(PyObject*)) {
    m_pyobj = ::DeepCopier::instance()->copy ((PyObject*)obj);
  } else {
    m_pyobj = TPython::ObjectProxy_FromVoidPtr 
      (obj,
       System::typeinfoName(type.name()).c_str());
  }
}

PyProperty::PyProperty (const std::string& name,
			PyObject* obj) :
  PropertyWithHandlers (name, typeid(obj)),
  m_pyobj  (::DeepCopier::instance()->copy (obj))
{}

// Destructor
///////////////

PyProperty::~PyProperty()
{
  Py_XDECREF (m_pyobj);
}

// Property interface

/// export the property value to the destination
bool
PyProperty::load (Property& dest) const
{
  return dest.assign (*this);
}

/// import the property value from source
bool 
PyProperty::assign (const Property& src)
{
  const PyProperty *p = dynamic_cast<const PyProperty*> (&src);
  if (p) {
    m_pyobj = ::DeepCopier::instance()->copy (p->m_pyobj);
    return true;
  }

  const std::string value_type = System::typeinfoName (*src.type_info());
  const std::string prop_type = "PropertyWithValue< "+value_type+" >";

  PyObject *pyprop= TPython::ObjectProxy_FromVoidPtr
    ((void*)&src, (char*)prop_type.c_str());
  if (!pyprop) {
    // XXX assume error message installed by PyRoot is explanatory enough.
    PyAthena::throw_py_exception();
  }

  PyObject *value = PyObject_CallMethod (pyprop, 
					 (char*)"value",
					 (char*)"");
  Py_DECREF (pyprop);

  if (!value) {
    PyAthena::throw_py_exception();
  }
  // steal reference
  m_pyobj = value;
  return true;
}

/// export the property value as a @c std::string
std::string
PyProperty::toString() const
{
  useReadHandler();
  // XXX this might not do something useful if m_pyobj is an ObjectProxy...
  return PyAthena::str(m_pyobj);
}

/// insert the property into a stream
void
PyProperty::toStream(std::ostream& out) const
{
  useReadHandler();
  // XXX this might not do something useful if m_pyobj is an ObjectProxy...
  out << PyAthena::str(m_pyobj);
}

/// import the property value from a @c std::string
StatusCode 
PyProperty::fromString (const std::string& value)
{
  PyObject *o = TPython::ObjectProxy_FromVoidPtr
    ((void*)TPython::Eval (value.c_str()),
     System::typeinfoName(*this->type_info()).c_str());
  if (!o) {
    return StatusCode::FAILURE;
  }
  if (!TPython::ObjectProxy_Check(o)) {
    Py_DECREF(o);
    return StatusCode::FAILURE;
  }
  // steals reference.
  m_pyobj = o;
  return StatusCode::SUCCESS;
}

/// clone: the usual "virtual constructor" pattern
PyProperty* 
PyProperty::clone() const
{
  return new PyProperty(*this);
}

/// access underlying wrapped object
void* 
PyProperty::object() const
{
  return TPyReturn (m_pyobj);
}

/// access underlying wrapped object as a python object
PyObject* 
PyProperty::value() const
{
  Py_XINCREF (m_pyobj);
  return m_pyobj;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 



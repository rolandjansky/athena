/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SgPyDataModel.h"
#include "GaudiKernel/ServiceHandle.h"

CLID PyCLID = 72785480;

namespace {


TClass* objectIsA (PyObject* obj)
{
  PyObject* repr = PyObject_Repr (obj);
  if (!repr) return nullptr;
  const char* s = PyString_AsString (repr);
  if (*s == '<') ++s;
  if (strncmp (s, "ROOT.", 5) == 0)
    s += 5;
  const char* p = strstr (s, " object ");
  if (!p) return nullptr;
  std::string name (s, p-s);
  TClass* cls = TClass::GetClass (name.c_str());
  Py_DECREF (repr);
  return cls;
}


}

namespace SG {
  IClassIDSvc* PyDataBucket::s_clidSvc = 0;
  IClassIDSvc* PyDataBucket::clidSvc()
  {
    if ( 0 == s_clidSvc ) {
      Gaudi::svcLocator()->getService ("ClassIDSvc", 
				       *pp_cast<IService>(&s_clidSvc)).ignore();
    }
    return s_clidSvc;
  }

  PyDataBucket::PyDataBucket( PyObject* pyObj,
			      CLID clid ) :
    DataBucketBase(),
    m_pyObj( pyObj ),
    m_clid ( clid  ),
    m_bib  ( SG::BaseInfoBase::find(clid) )
  {
    // prevent Python from sweeping the rug under our feet
    Py_INCREF( pyObj );
  }

  void* PyDataBucket::cast( CLID clid,
                            IRegisterTransient* /*itr*/,
                            bool /*isConst*/ )
  {
    // if requested type is same than myself ==> no conversion needed
    if ( clid == m_clid ) {
      return clid == PyCLID 
	? m_pyObj
	: ObjectProxy_ASVOIDPTR(m_pyObj);
    }
    void* address = (m_clid == PyCLID)
      ? (void*)m_pyObj
      : ObjectProxy_ASVOIDPTR(m_pyObj);

    // try SG-based conversion functions
    {
      void* o = m_bib ? m_bib->cast(address, clid) : 0;
      if ( o ) { return o; }
    }

    // try PyRoot based ones
    PyObject* pytp = PyProxyMgr::instance().pytp(clid);
    if ( !pytp ) {
      PyErr_Format( PyExc_TypeError, "actual type of CLID %lu unknown",
		    (long unsigned int)clid );
      return 0;
    }

    // this will be a conversion for a class instance only (see below:
    // verified that only a ObjectProxy is expected), so bind with cast
    TClass* cls = TClass::GetClass (PyString_AS_STRING(pytp));
    if (!cls) {
      PyErr_Format( PyExc_TypeError, "Can't find TClass for `%s'",
		    PyString_AS_STRING(pytp) );
      return 0;
    }
    TClass* act_class = cls->GetActualClass (address);
    PyObject* value = TPython::ObjectProxy_FromVoidPtr (address, act_class->GetName());

    if ( value && TPython::ObjectProxy_Check(value) ) {
      return ObjectProxy_ASVOIDPTR(value);
    }
    Py_XDECREF(value);
    throw PyROOT::TPyException();
    return 0;
  }

  void* PyDataBucket::cast( const std::type_info& tinfo,
                            IRegisterTransient* /*itr*/,
                            bool /*isConst*/)
  {
    // if regular PyObject, meaningless
    if ( m_clid == PyCLID ) {
      return 0;
    }

    // if requested type is same than myself ==> no conversion needed
    TClass* tcls = objectIsA (m_pyObj);
    if ( tcls && (tinfo == *(tcls->GetTypeInfo())) ) {
      return ObjectProxy_ASVOIDPTR(m_pyObj);
    }
    void* address = ObjectProxy_ASVOIDPTR(m_pyObj);

    // try SG-based conversion functions
    {
      void* o = m_bib ? m_bib->cast(address, tinfo) : 0;
      if ( o ) { return o; }
    }

    // this will be a conversion for a class instance only (see below:
    // verified that only a ObjectProxy is expected), so bind with cast
    TClass* clsnew = TClass::GetClass (tinfo);
    if (!clsnew) {
      PyErr_SetString
        ( PyExc_RuntimeError, 
          "SG::PyDataBucket::cast() can't find TClass" );
      return 0;
    }
    TClass* act_class = clsnew->GetActualClass (address);
    PyObject* value = TPython::ObjectProxy_FromVoidPtr (address, act_class->GetName());
    PyErr_Clear();

    if ( value && TPython::ObjectProxy_Check(value) ) {
      return ObjectProxy_ASVOIDPTR(value);
    }
    Py_XDECREF(value);
    //throw PyROOT::TPyException();
    return 0;
  }

  PyDataBucket* PyDataBucket::clone() const
  { 
    PyErr_SetString
      ( PyExc_RuntimeError, 
	"SG::PyDataBucket::clone() is not meant to be called (yet)" );
    std::abort();
    return 0;
//     new SG::PyDataBucket( m_type, copy<>(m_pyObj), m_clid );
  }

  void PyDataBucket::lock()
  {
    if (!m_pyObj) return;
    if (!PyObject_HasAttrString (m_pyObj, "lock"))
      return;
    PyObject* lock = PyObject_GetAttrString (m_pyObj, "lock");
    if (!lock) return;
    if (PyCallable_Check (lock)) {
      PyObject* ret = PyObject_CallObject (lock, NULL);
      Py_DECREF (ret);
    }
    Py_DECREF (lock);
  }

  /////////////////////////////////////////////////////////////////////////////
  // PyProxyMgr

  PyProxyMgr::PyProxyMgr()
  {
    m_aliases = importDictAliases();
    m_clids   = PyDict_New();
    m_clidSvc = 0;
    {
      ServiceHandle<IClassIDSvc> svc("ClassIDSvc", "SgPyDataModel");
      if ( !svc.retrieve().isSuccess()) {
	throw std::runtime_error
	  ("SG::PyProxyMgr: Could not retrieve ClassIDSvc");
      }
      m_clidSvc = svc.operator->();
    }
    m_dictSvc = 0;
    {
      ServiceHandle<IDictLoaderSvc> svc("AthDictLoaderSvc", "SgPyDataModel");
      if ( !svc.retrieve().isSuccess()) {
	throw std::runtime_error
	  ("SG::PyProxyMgr: Could not retrieve AthDictLoaderSvc");
      }
      m_dictSvc = svc.operator->();
    }
  }

  PyProxyMgr::~PyProxyMgr()
  {
    Py_DECREF(m_aliases);
    Py_DECREF(m_clids);
    // delete the proxy dicts...
    for ( PyProxyMap_t::iterator 
	    i    = m_proxyMap.begin(),
	    iEnd = m_proxyMap.end();
	  i != iEnd;
	  ++i ) {
      delete i->second; i->second = 0;
    }
  }

} //< end namespace SG

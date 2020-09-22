/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "SgPyDataModel.h"
#include "GaudiKernel/ServiceHandle.h"
#include "RootUtils/PyAthenaGILStateEnsure.h"

CLID PyCLID = 72785480;

namespace {


TClass* objectIsA (PyObject* obj)
{
  PyObject* repr = PyObject_Repr (obj);
  if (!repr) return nullptr;
  std::string str = RootUtils::PyGetString (repr).first;
  const char* s = str.c_str();
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
    RootUtils::PyGILStateEnsure gil;
    // if requested type is same than myself ==> no conversion needed
    if ( clid == m_clid ) {
      return clid == PyCLID 
	? m_pyObj
	: CPPInstance_ASVOIDPTR(m_pyObj);
    }
    void* address = (m_clid == PyCLID)
      ? (void*)m_pyObj
      : CPPInstance_ASVOIDPTR(m_pyObj);

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
    // verified that only a CPPInstance is expected), so bind with cast
    std::string pytpstr = RootUtils::PyGetString(pytp).first;
    TClass* cls = TClass::GetClass (pytpstr.c_str());
    if (!cls) {
      PyErr_Format( PyExc_TypeError, "Can't find TClass for `%s'",
		    pytpstr.c_str() );
      return 0;
    }
    TClass* act_class = cls->GetActualClass (address);
    PyObject* value = TPython::CPPInstance_FromVoidPtr (address, act_class->GetName());

    if ( value && TPython::CPPInstance_Check(value) ) {
      return CPPInstance_ASVOIDPTR(value);
    }
    Py_XDECREF(value);
    throw CPyCppyy::PyException();
    return 0;
  }

  void* PyDataBucket::cast( const std::type_info& tinfo,
                            IRegisterTransient* /*itr*/,
                            bool /*isConst*/)
  {
    RootUtils::PyGILStateEnsure gil;
    // if regular PyObject, meaningless
    if ( m_clid == PyCLID ) {
      return 0;
    }

    // if requested type is same than myself ==> no conversion needed
    TClass* tcls = objectIsA (m_pyObj);
    if ( tcls && (tinfo == *(tcls->GetTypeInfo())) ) {
      return CPPInstance_ASVOIDPTR(m_pyObj);
    }
    void* address = CPPInstance_ASVOIDPTR(m_pyObj);

    // try SG-based conversion functions
    {
      void* o = m_bib ? m_bib->cast(address, tinfo) : 0;
      if ( o ) { return o; }
    }

    // this will be a conversion for a class instance only (see below:
    // verified that only a CPPInstance is expected), so bind with cast
    TClass* clsnew = TClass::GetClass (tinfo);
    if (!clsnew) {
      PyErr_SetString
        ( PyExc_RuntimeError, 
          "SG::PyDataBucket::cast() can't find TClass" );
      return 0;
    }
    TClass* act_class = clsnew->GetActualClass (address);
    PyObject* value = TPython::CPPInstance_FromVoidPtr (address, act_class->GetName());
    PyErr_Clear();

    if ( value && TPython::CPPInstance_Check(value) ) {
      return CPPInstance_ASVOIDPTR(value);
    }
    Py_XDECREF(value);
    //throw PyROOT::TPyException();
    return 0;
  }

  void PyDataBucket::lock()
  {
    RootUtils::PyGILStateEnsure gil;
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
    // Don't do this if don't have a valid thread state.
    // (With py3, the interpreter gets shut down before global dtors run...)
#if PY_VERSION_HEX >= 0x03000000
    if (_PyThreadState_UncheckedGet())
#else
    if (PyThreadState_GET())
#endif
    {
      Py_DECREF(m_aliases);
      Py_DECREF(m_clids);
    }
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

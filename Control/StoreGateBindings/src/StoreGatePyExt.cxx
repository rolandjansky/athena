///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGateBindings/StoreGatePyExt.h"

#include "Python.h"

#include "StoreGateBindingsDict.h"

// Framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DataObject.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/DataBucketBase.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/ClassID_traits.h"

// SGTools includes
#include "SGTools/BuiltinsClids.h"

// ROOT includes
#include "DataModelRoot/RootType.h"
#include "TClass.h"
#include "TClassRef.h"
#include "TROOT.h"

// PyROOT includes
#include "AthenaPyRoot.h"

#include "SgPyDataModel.h"
#include "SgPyMsg.h"

#include <string>
#include <vector>
#include <algorithm> // for stable_partition

namespace {
  static CLID bool_clid = ClassID_traits<bool>::ID();
  static CLID char_clid = ClassID_traits<char>::ID();
  static CLID int_clid =  ClassID_traits<int>::ID();
  static CLID uint_clid = ClassID_traits<unsigned int>::ID();
  static CLID long_clid = ClassID_traits<long>::ID();
  static CLID ulong_clid= ClassID_traits<unsigned long>::ID();
  static CLID longlong_clid= ClassID_traits<long long>::ID();
  static CLID float_clid= ClassID_traits<float>::ID();
  static CLID double_clid=ClassID_traits<double>::ID();


/// Given a type object TP, return the name of the type as a python string.
/// TP could also be a string.
/// Ownership is transferred back to the caller.
/// On error, returns 0 with PyErr set.
PyObject* pynameFromType (PyObject* tp)
{
  PyObject* pyname = nullptr;

  if ( ! PyType_Check( tp ) ) {
#if PY_VERSION_HEX < 0x03000000
    if ( ! PyString_Check( tp ) )
#else
    if ( ! PyUnicode_Check( tp ) )
#endif
    {
      PyErr_SetString( PyExc_TypeError, 
                       "contains() argument 1 must be type or class name" );
      return nullptr;
    }
    else {
      Py_INCREF( tp );
      pyname = tp;
    }
  }
  else {
    pyname = PyObject_GetAttrString( tp, (char*)"__cpp_name__" );
    if (!pyname) {
      pyname = PyObject_GetAttrString( tp, (char*)"__name__" );
    }
#if PY_VERSION_HEX < 0x03000000
    if ( pyname && ! PyString_Check( pyname ) )
#else
    if ( pyname && ! PyUnicode_Check( pyname ) )
#endif
    {
      PyObject* pystr = PyObject_Str( pyname );
      if ( pystr ) {
        Py_DECREF( pyname );
        pyname = pystr;
      }
    }

    if ( PyErr_Occurred() )
      return nullptr;
  }

  return pyname;
}


}

PyObject* 
AthenaInternal::retrieveObjectFromStore( PyObject* self, 
                                         PyObject* tp, PyObject* pykey )
{
  StoreGateSvc* store = (StoreGateSvc*)CPPInstance_ASVOIDPTR(self);
  return retrieveObjectFromStore(store,tp,pykey);
}

PyObject* 
AthenaInternal::retrieveObjectFromStore( StoreGateSvc* store, 
                                         PyObject* tp, PyObject* pykey )
{
  void* res = 0;
  PyObject* objProxy = NULL;

  static SG::PyProxyMgr& s_mgr = SG::PyProxyMgr::instance();


  // unlikely to happen, but checking is cheap
  if ( ! store ) {
    PyErr_SetString( PyExc_RuntimeError, 
                     "no store available, is Athena initialized?" );
    return 0;
  }

  // expect a type or type name and an optional string key
  PyObject* pyname = pynameFromType( tp );
  if (!pyname) {
    return pyname;
  }

  auto [namestr, nameflag] = RootUtils::PyGetString( pyname );
  auto [keystr, keyflag] = RootUtils::PyGetString( pykey );
  if ( !keyflag ) {
    if (pykey == Py_None) {
      keystr = "<None>";
    }
    else {
      PyErr_SetString( PyExc_TypeError, 
                       "retrieve() argument 2 must be string key" );
      Py_XDECREF (pyname);
      return 0;
    }
  }

  SG::PyProxyDict* proxyDict = s_mgr.pyproxy(store);
  // retrieve CLID corresponding to the request
  PyObject* pyclid = s_mgr.pyclid(pyname);
  if ( ! pyclid ) {
    PyErr_Format( PyExc_NameError, 
                  "ID of \"%s\" is unknown", namestr.c_str() );
    Py_XDECREF (pyname);
    return 0;
  }

  Py_XDECREF (pyname);
  pyname = 0;

  _SGPY_MSG("retrieving py-proxy...");
  PyObject* pyproxy = proxyDict->proxy(pyclid, pykey);
  if ( ! pyproxy ) {
    PyErr_Format( PyExc_LookupError, 
                  "no py-proxies for (clid=%lu, type=%s, key=%s)", 
                  PyLong_AsUnsignedLong(pyclid),
                  namestr.c_str(),
                  (char*)( (pykey == Py_None) 
                           ? "<None>" 
                           : keystr.c_str() )
                  );
    return 0;
  }

  _SGPY_MSG("retrieved py-proxy [clid=" << PyLong_AsUnsignedLong(pyclid)
            << ", type=" << namestr.c_str()
            << ", key=" << (char*)( (pykey == Py_None) 
                                    ? "<None>" 
                                    : keystr.c_str() )
            << "]");

  SG::DataProxy* proxy = (SG::DataProxy*)CPPInstance_ASVOIDPTR(pyproxy);

  if ( ! proxy ) {
    PyErr_Format( PyExc_LookupError, 
                  "no proxies for (clid=%lu, type=%s, key=%s)", 
                  PyLong_AsUnsignedLong(pyclid),
                  namestr.c_str(),
                  (char*)( (pykey == Py_None) 
                           ? "<None>" 
                           : keystr.c_str() )
                  );
    return 0;
  }

  _SGPY_MSG("retrieved cpp-proxy [clid=" << proxy->clID()
            << ", key=" << proxy->name() << "]");

  // cast proxy to pointer type if needed (setting on return type is evil hack)
  DataObject* dobj = proxy->accessData();
  if ( ! dobj ) {
    PyErr_Format( PyExc_LookupError, 
                  "no such object \"%s\"", namestr.c_str() );
    return 0;
  }

  _SGPY_MSG("retrieved dobj [clID=" << dobj->clID()
            << ", classID=" << dobj->classID()
            << ", name=" << dobj->name() << "]");

  DataBucketBase* dbb = dynamic_cast< DataBucketBase* >( dobj );
  if ( ! dbb ) {
    PyErr_SetString
      ( PyExc_TypeError,
        "unexpected kind of DataObject: can not verify final type" );
    return 0;
  }

  _SGPY_MSG("retrieved dbb [clID=" << dbb->clID()
            << ", classID=" << dbb->classID()
            << ", name=" << dbb->name() << "]");

  CLID id     = proxy->clID();
  CLID realID = dbb->clID();

  _SGPY_MSG("pyid= " << PyLong_AsUnsignedLong(pyclid)
            << ", id=" << id
            << ", realID=" << realID);

  // special case of a regular PyObject
  if ( PyCLID == realID ) {
    objProxy = (PyObject*)dbb->object();

  } else if (realID == bool_clid) {
    res = dbb->cast( bool_clid );
    bool *v = reinterpret_cast<bool*>(res);
    objProxy = PyBool_FromLong(*v);
    return objProxy;

  } else if (realID == char_clid) {
    res = dbb->cast( char_clid );
    char *v = reinterpret_cast<char*>(res);
#if PY_VERSION_HEX < 0x03000000
    objProxy = PyString_FromStringAndSize(v, 1);
#else
    objProxy = PyUnicode_FromStringAndSize(v, 1);
#endif
    return objProxy;

  } else if (realID == int_clid) {
    res = dbb->cast( int_clid );
    int *v = reinterpret_cast<int*>(res);
#if PY_VERSION_HEX < 0x03000000
    objProxy = PyInt_FromLong(*v);
#else
    objProxy = PyLong_FromLong(*v);
#endif
    return objProxy;

  } else if (realID == uint_clid) {
    res = dbb->cast( uint_clid );
    unsigned int *v = reinterpret_cast<unsigned int*>(res);
#if PY_VERSION_HEX < 0x03000000
    objProxy = PyInt_FromLong(*v);
#else
    objProxy = PyLong_FromLong(*v);
#endif
    return objProxy;

  } else if (realID == long_clid) {
    res = dbb->cast( long_clid );
    long *v =reinterpret_cast<long*>(res);
    objProxy = PyLong_FromLong(*v);
    return objProxy;

  } else if (realID == ulong_clid) {
    res = dbb->cast( ulong_clid );
    unsigned long *v =reinterpret_cast<unsigned long*>(res);
    objProxy = PyLong_FromUnsignedLong(*v);
    return objProxy;
    
  } else if (realID == longlong_clid) {
    res = dbb->cast( longlong_clid );
    long long *v =reinterpret_cast<long long*>(res);
    objProxy = PyLong_FromUnsignedLong(*v);
    return objProxy;
    
  } else if (realID == float_clid) {
    res = dbb->cast( float_clid );
    float *v =reinterpret_cast<float*>(res);
    objProxy = PyFloat_FromDouble(*v);
    return objProxy;
  
  } else if (realID == double_clid) {
    res = dbb->cast( double_clid );
    double *v =reinterpret_cast<double*>(res);
    objProxy = PyFloat_FromDouble(*v);
    return objProxy;
  
  } else if ( id == realID ) {
    res = dbb->object();
    
    _SGPY_MSG("pyid= " << PyLong_AsUnsignedLong(pyclid)
              << ", id=" << id
              << ", realID=" << realID << " => res = [" << res << "]");

    if ( ! res ) {
      PyErr_Format( PyExc_RuntimeError, "found an invalid object" );
      return 0;
    }
    
    const char* klass_name = s_mgr.load_type(id);
    _SGPY_MSG("retrieving objProxy(" << klass_name
              << ", " << res << ")...");
    objProxy = proxyDict->newPyDataObject(klass_name, res);
    _SGPY_MSG("retrieving objProxy(" << klass_name
              << ", " << res << ")... [done]");

  } else {
    // either use CLID BaseInfo<> or Reflex, try both as appropriate
    res = dbb->cast( id );

    // ensure everything is loaded for that clid
    const char* real_typename = s_mgr.load_type(realID);
    
    _SGPY_MSG("dbb::cast(" << id << ") = " << res);

    if ( res ) {

      objProxy= proxyDict->newPyDataObject(real_typename, res);
    } else {
      // -> try Reflex...
      IClassIDSvc* clidSvc = s_mgr.m_clidSvc;
      std::string realName = "";
      if ( !clidSvc->getTypeNameOfID(realID, realName).isSuccess() ) {
        PyErr_Format( PyExc_TypeError, "actual type of CLID %lu unknown",
                      (long unsigned int)realID );
        return 0;
      }
      
      const RootType& fromType = RootType::ByName(realName);
      
      if ( (bool)fromType ) {
        const RootType& toType = RootType::ByName( namestr );
        res = dbb->object();
        if (fromType.Class() && toType.Class())
          res = fromType.Class()->DynamicCast (toType.Class(), res);
	
        if ( res ) {
          objProxy = proxyDict->newPyDataObject(realName.c_str(), res);
        }
      }
    }

    if ( ! res ) {
      PyErr_SetString( PyExc_TypeError, "cast to requested type failed" );
      return 0;
    }

  }

  Py_INCREF(objProxy);
  return objProxy;
}

PyObject* 
AthenaInternal::py_sg_contains (PyObject* self,
                                PyObject* tp, PyObject* pykey)
{
  StoreGateSvc* store = (StoreGateSvc*)CPPInstance_ASVOIDPTR(self);
  return py_sg_contains(store,tp,pykey);
}

PyObject* 
AthenaInternal::py_sg_contains (StoreGateSvc* store,
                                PyObject* tp, PyObject* pykey)
{
  static SG::PyProxyMgr& s_mgr = SG::PyProxyMgr::instance();

  

  // unlikely to happen, but checking is cheap
  if ( ! store ) {
    PyErr_SetString( PyExc_RuntimeError, 
                     "no store available, is Athena initialized?" );
    return 0;
  }

  // expect a type or type name and an optional string key
  PyObject* pyname = pynameFromType( tp );
  if (!pyname) {
    return pyname;
  }

  auto [namestr, nameflag] = RootUtils::PyGetString (pyname);
  auto [keystr,  keyflag]  = RootUtils::PyGetString (pykey);

  if ( !keyflag ) {
    PyErr_SetString( PyExc_TypeError, 
                     "contains() argument 2 must be string key" );
    Py_XDECREF (pyname);
    return 0;
  }

  // retrieve CLID corresponding to the request
  const CLID clid = s_mgr.clid (pyname);
  if ( clid == CLID_NULL ) {
    PyErr_Format( PyExc_NameError, 
                  "ID of \"%s\" is unknown", namestr.c_str() );
    return 0;
  }

  Py_XDECREF (pyname);
  pyname = 0;

  const bool sg_contains = 
    store->contains (clid, keystr) ;

  _SGPY_MSG("sg::contains(" << clid
            << "/" << namestr.c_str() << ", "
            << keystr.c_str()
            << ") = [" 
            << (sg_contains ? std::string("true") : std::string("false"))
            << "]");

  PyObject* o = sg_contains
    ? Py_True
    : Py_False;
  Py_INCREF (o);
  return o;
}


PyObject* 
AthenaInternal::py_sg_getitem (PyObject* self,
                               PyObject* pykey)
{
  StoreGateSvc* store = (StoreGateSvc*)CPPInstance_ASVOIDPTR(self);
  return py_sg_getitem(store,pykey);
}

PyObject* 
AthenaInternal::py_sg_getitem (StoreGateSvc* store,
                               PyObject* pykey)
{
  static SG::PyProxyMgr& s_mgr = SG::PyProxyMgr::instance();

  

  // unlikely to happen, but checking is cheap
  if ( ! store ) {
    PyErr_SetString( PyExc_RuntimeError, 
                     "no store available, is Athena initialized?" );
    return 0;
  }

  auto [keystr, keyflag] = RootUtils::PyGetString (pykey);
  if ( ! keyflag ) {
    PyErr_SetString( PyExc_TypeError, 
                     "__getitem__() argument 1 must be string key" );
    return 0;
  }

  // Retrieve the main @c CLID of the object recorded in @c StoreGate
  const CLID clid = store->clid (keystr);
  if ( CLID_NULL == clid ) {
    PyErr_Format (PyExc_LookupError, 
                  "no clid for key=%s", 
                  keystr.c_str());
    return 0;
  }
  
  return AthenaInternal::retrieveObjectFromStore (store, 
                                                  s_mgr.pytp(clid), 
                                                  pykey);
}

void 
AthenaInternal::py_sg_clearProxyPayload(StoreGateSvc* self, SG::DataProxy* dp)
{
  return self->clearProxyPayload(dp);
}

PyObject* 
AthenaInternal::recordObjectToStore( PyObject* self,
                                     PyObject* obj,
                                     PyObject* pykey,
                                     bool allowMods /*= true*/,
                                     bool resetOnly /*= true*/,
                                     bool noHist /*= false*/)
{
  StoreGateSvc* store = (StoreGateSvc*)CPPInstance_ASVOIDPTR(self);
  return recordObjectToStore(store,obj,pykey,allowMods,resetOnly,noHist);
}

PyObject* 
AthenaInternal::recordObjectToStore( StoreGateSvc* store,
                                     PyObject* obj,
                                     PyObject* pykey,
                                     bool allowMods /*= true*/,
                                     bool resetOnly /*= true*/,
                                     bool noHist /*= false*/)
{
  static SG::PyProxyMgr& s_mgr = SG::PyProxyMgr::instance();

  

  // unlikely to happen, but checking is cheap
  if ( ! store ) {
    PyErr_SetString( PyExc_RuntimeError, 
                     "no store available, is Athena initialized?" );
    return 0;
  }

  // expect a type or type name
  PyObject* tp = PyObject_GetAttrString( obj, (char*)"__class__" );
  if ( ! PyType_Check( tp ) ) {
    PyErr_SetString( PyExc_RuntimeError, 
                     "could not retrieve type of object" );
    return 0;
  }

  PyObject* pyname = 0;
  // check if this is a PyRoot object or a 'regular' PyObject
  const bool isPlainPyObj = !TPython::CPPInstance_Check (obj);
  if ( isPlainPyObj ) {
#if PY_VERSION_HEX < 0x03000000
    pyname = PyString_FromString ((char*)"PyObject");
#else
    pyname = PyUnicode_FromString ((char*)"PyObject");
#endif
  } else {
    pyname = pynameFromType( tp );
  }

  if ( PyErr_Occurred() )
    return 0;

  auto [keystr, keyflag] = RootUtils::PyGetString (pykey);
  auto [namestr, nameflag] = RootUtils::PyGetString (pyname);

  if ( ! keyflag ) {
    PyErr_SetString( PyExc_TypeError, 
                     "record() argument 2 must be string key" );
    Py_XDECREF (pyname);
    return 0;
  }
  
  // retrieve CLID corresponding to the request
  const CLID id = s_mgr.clid(pyname);
  if ( CLID_NULL == id ) {
    PyErr_Format( PyExc_NameError, 
                  "ID of \"%s\" is unknown", namestr.c_str() );
    return 0;
  }

  Py_XDECREF (pyname);
  pyname = 0;

  // make sure everything has been loaded for that clid...
  s_mgr.load_type(id);

  _SGPY_MSG("ID=" << id
            << ", tp=" << namestr.c_str()
            << ", key=" << keystr.c_str());

  SG::PyDataBucket* dbb = new SG::PyDataBucket (obj, id);

  _SGPY_MSG("created a pdb @" << dbb << ", clID=" << dbb->clID()
            << ", pdb-obj @" << dbb->object()
            << ", obj @" << (void*)obj
            << ", cc: " << CPPInstance_ASVOIDPTR(obj)
            << ", isplain-pyobj: [" << (int)isPlainPyObj << "]"
            << ", pyrobj @" << CPPInstance_ASVOIDPTR(obj));
  _SGPY_MSG("pdb-cast(" << dbb->clID() << "): " << dbb->cast(dbb->clID()));
	    
  int sc = store->typeless_record( dbb, keystr,
                                   isPlainPyObj 
                                   ? (void*)obj
                                   : CPPInstance_ASVOIDPTR(obj),
                                   allowMods,
                                   resetOnly,
                                   noHist ).isSuccess()
    ? 1
    : 0; 
  return Py_BuildValue( const_cast<char*>("i"), sc );
}

//

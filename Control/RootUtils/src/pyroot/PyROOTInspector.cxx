/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PyROOTInspector.cxx 790007 2016-12-15 17:46:55Z ssnyder $

//#define PYROOT_INSPECTOR_DBG 1
#define PYROOT_INSPECTOR_DBG 0

// Interacts with python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "Python.h"

// ROOT includes
#include "TClassRef.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TVirtualCollectionProxy.h"

#include "Utility.h"

// PyROOT includes
#include <TPython.h>
#include <TPyException.h>
#ifndef ROOT_TPyException
# define ROOT_TPyException 1 /* there was a typo in TPyException-v20882 */
#endif

// fixes 'dereferencing type-punned pointer will break strict-aliasing rules'
#ifdef Py_True
#undef Py_True
#define Py_True ( (PyObject*)(void*)&_Py_TrueStruct )
#endif
#ifdef Py_False
#undef Py_False
#define Py_False ( (PyObject*)(void*)&_Py_ZeroStruct )
#endif


// ------------------------------------
#include "RootUtils/PyROOTInspector.h"

#include "TClass.h"
#include "TPython.h"
#include "TList.h"

#include <iostream>

namespace {
PyObject*
to_pyobj(void* ptr, EDataType mbr_dtype) 
{
  switch(mbr_dtype) {

  case kChar_t:
  case kUChar_t:
  case kchar:
    return Py_BuildValue("c", ((char*)ptr)[0] & 0x7F); // WTF ?!?!

  case kBool_t:
    return Py_BuildValue("i", (*(bool*)ptr) ? 1 : 0);

  case kShort_t:
    return Py_BuildValue("h", (*(short*)ptr));

  case kUShort_t:
    return Py_BuildValue("H", (*(unsigned short*)ptr));

  case kCounter:
  case kInt_t:
    return Py_BuildValue("i", (*(int*)ptr));

  case kUInt_t:
    return Py_BuildValue("I", (*(unsigned int*)ptr));

  case kDouble_t:
  case kDouble32_t:
    return Py_BuildValue("d", (*(double*)ptr));

  case kFloat_t:
  case kFloat16_t:
    return Py_BuildValue("f", (*(float*)ptr));

  case kLong_t:
    return Py_BuildValue("l", (*(long*)ptr));

  case kULong_t:
    return Py_BuildValue("k", (*(unsigned long*)ptr));

  case kLong64_t:
    return Py_BuildValue("L", (*(Long64_t*)ptr));

  case kULong64_t:
    return Py_BuildValue("K", (*(ULong64_t*)ptr));

  case kCharStar:
    return Py_BuildValue("s", (*(char**)ptr));

  case kBits:
  case kOther_t:
  case kNoType_t:
  default:
    return PyROOT_PyUnicode_FromString("(UNKNOWN)");
    //py_mbr = Py_None;
    //Py_INCREF(py_mbr);
  } // switch
  // not reached.
  //Py_INCREF(Py_None);
  //return Py_None;
}

#if PYROOT_INSPECTOR_DBG
inline
std::string to_str(PyObject *obj)
{
  PyObject *py_str = PyObject_Str(obj);
  std::string s = py_str && PyString_Check(py_str) 
    ? PyString_AS_STRING(py_str)
    : "";
  Py_XDECREF(py_str);
  return s;
}
#endif

inline
PyObject*
new_pylist(PyObject *pylist, PyObject *item)
{
  PyObject *obj = PySequence_List(pylist);
  PyList_Append(obj, item);
  return obj;
}

void
recurse_pyinspect(PyObject *pyobj,
                  PyObject *pyobj_name,
                  PyObject *&pystack,
                  bool persistentOnly)
{
  // handle non-pyroot objects
  if (!TPython::ObjectProxy_Check(pyobj)) {
    PyObject *val = PyTuple_New(2);
    PyObject *v0 = pyobj_name; Py_INCREF(v0);
    PyObject *v1 = pyobj;      Py_XINCREF(pyobj);
    PyTuple_SET_ITEM(val, 0, v0);
    PyTuple_SET_ITEM(val, 1, v1);
    PyList_Append(pystack, val);
    Py_DECREF(val);
    return;
  }

  TClass *tcls = RootUtils::objectIsA(pyobj);
  if (0 == tcls ||
      (tcls->IsTObject() && strcmp(tcls->GetName(), "TLorentzVector") != 0))
  {
    PyObject *val = PyTuple_New(2);
    PyObject *v0 = pyobj_name; Py_INCREF(v0);
    PyObject *v1 = pyobj;      Py_XINCREF(pyobj);
    PyTuple_SET_ITEM(val, 0, v0);
    PyTuple_SET_ITEM(val, 1, v1);
    PyList_Append(pystack, val);
    Py_DECREF(val);
    return;
  }
  void *obj = TPython::ObjectProxy_AsVoidPtr(pyobj);

  if (!strcmp(tcls->GetName(), "string")) {
    std::string *str = (std::string*)obj;
    PyObject *val= PyTuple_New(2);
    PyObject *v0 = pyobj_name; Py_INCREF(v0);
    PyObject *v1 = PyROOT_PyUnicode_FromString(str->c_str());
    PyTuple_SET_ITEM(val, 0, v0);
    PyTuple_SET_ITEM(val, 1, v1);
    PyList_Append(pystack, val);
    Py_DECREF(val);
    return;
  }

  TString tstring = tcls->GetName();
  if (tstring.BeginsWith("pair<") ||
      tstring.BeginsWith("std::pair<")) {
    {
      PyObject *v0 = PyROOT_PyUnicode_FromString("first");
      PyObject *v1 = PyObject_GetAttrString(pyobj, "first");
      PyObject *v1_name = ::new_pylist(pyobj_name, v0);
      recurse_pyinspect(v1, v1_name, pystack, persistentOnly);
      Py_DECREF(v1_name);
      Py_DECREF(v0);
      Py_DECREF(v1);
    }

    {
      PyObject *v0 = PyROOT_PyUnicode_FromString("second");
      PyObject *v1 = PyObject_GetAttrString(pyobj, "second");
      PyObject *v1_name = ::new_pylist(pyobj_name, v0);
      recurse_pyinspect(v1, v1_name, pystack, persistentOnly);
      Py_DECREF(v1_name);
      Py_DECREF(v1);
      Py_DECREF(v0);
    }
    return;
  }

  Int_t hdr = 0;
  if (PySequence_Check(pyobj)) {
    if (!strcmp(tcls->GetName(), "CLHEP::Hep3Vector") ||
        !strcmp(tcls->GetName(), "TLorentzVector") ||
        !strcmp(tcls->GetName(), "TVector3"))
    {
      hdr = 0;
    } else {
      hdr = 1;
    }
  } else {
    hdr = 0;
  }

  TList *members = tcls->GetListOfDataMembers();
  // members can be null for STL containers in root 6.08.
  const Int_t nmembers = members ? members->GetEntries() : 0;

#if PYROOT_INSPECTOR_DBG
  std::cerr << "==[" << tcls->GetName() << "]== (#mbrs:"
            << nmembers 
            << " #stl:" << hdr /*PySequence_Size(pyobj)*/
            << ")...\n";
#endif

  if (hdr) {
    // handle collection
#if PYROOT_INSPECTOR_DBG
    {
      const Py_ssize_t nelems = PySequence_Size(pyobj);
      std::cerr << "== sequence (" << nelems << ")...\n";
    }
#endif

    // This used to use PySequence_GetItem.
    // However, xAOD::MissingETContainer redefines operator[] to do
    // something completely different, so that didn't work.
    // Rewriting in terms of iteration avoids this.
    // .. except that it mysteriously fails (sometimes) for TileCellVec.
    // .. and if we try to use the iterator interface for vector<char>,
    //    then with python 3, pyroot will try to convert its contents
    //    to a unicode string object, which will likely fail.
    Py_ssize_t nelems = PySequence_Size(pyobj);
    if (strcmp(tcls->GetName(), "TileCellVec") == 0 ||
        strcmp(tcls->GetName(), "vector<char>") == 0)
    {
      for (Py_ssize_t i = 0; i < nelems; ++i) {
        PyObject *pyidx = PyLong_FromLong(i);
        PyObject *itr = PySequence_GetItem(pyobj, i);
        PyObject *itr_name = ::new_pylist(pyobj_name, pyidx);
        recurse_pyinspect(itr, itr_name, pystack, persistentOnly);
        Py_XDECREF(itr_name);
        Py_XDECREF(pyidx);
        Py_XDECREF(itr);
      }
    }
    else {
      PyObject* iter = PyObject_GetIter(pyobj);
      size_t i = 0;
      if (iter) {
        PyObject* item = nullptr;
        // Sometimes iterator comparison doesn't work correctly in pyroot.
        // So protect against overrunning by also counting
        // the number of elements.
        while (nelems-- && (item = PyIter_Next(iter))) {
          PyObject *pyidx = PyLong_FromLong(i++);
          PyObject *itr_name = ::new_pylist(pyobj_name, pyidx);
          recurse_pyinspect(item, itr_name, pystack, persistentOnly);
          Py_XDECREF(itr_name);
          Py_XDECREF(pyidx);
          Py_DECREF(item);
        }
      }
    }

#if PYROOT_INSPECTOR_DBG
    std::cerr << "== sequence (" << nelems << ")... [done]\n";
#endif
  }


  for (Int_t j = 0; j<nmembers; ++j) {
    TDataMember *mbr = (TDataMember*)(members->At(j));
    if (mbr->Property() & kIsStatic) continue;
    Int_t offset =     mbr->GetOffset();
    char *ptr =        (char*)obj + offset;

#if PYROOT_INSPECTOR_DBG
    TClass *mbr_cls = TClass::GetClass(mbr->GetTypeName());
    std::cerr << "==[" << j << "] - [" << mbr->GetTypeName() << "] "
              << "[" << mbr->GetName() 
              << "]"
              << "[" << (mbr_cls ? mbr_cls->GetName() : "N/A") << "]\n";
#endif

    PyObject *py_mbr_name = 0;
    PyObject *py_mbr = 0;

    if (persistentOnly && !mbr->IsPersistent())
      continue;
    if (mbr->IsaPointer())
      continue;
    if (mbr->IsBasic()) {
      TDataType * mbr_type = mbr->GetDataType();
      EDataType mbr_dtype = (EDataType)mbr_type->GetType();
      py_mbr_name = PyROOT_PyUnicode_FromString(mbr->GetName());
      py_mbr = to_pyobj(ptr, mbr_dtype);

    } else if (mbr->IsEnum()) {
#if PYROOT_INSPECTOR_DBG
      std::cerr << "==[" << mbr->GetTypeName() << "]["
                << mbr->GetDataType()->GetType() << "][val="
                << (*(int*)ptr) << "]["
                << mbr->GetName() << "] is an enum !!\n";
#endif
      py_mbr_name = PyROOT_PyUnicode_FromString(mbr->GetTypeName());
      py_mbr = PyROOT_PyUnicode_FromString(mbr->GetName());
    } else {
      py_mbr_name = PyROOT_PyUnicode_FromString(mbr->GetName());
      py_mbr = TPython::ObjectProxy_FromVoidPtr((void*)ptr,
                                                mbr->GetTypeName());
    }

    if (!py_mbr || !py_mbr_name) {
      std::cerr << "could not create py-object of type ["
                << mbr->GetTypeName() << "] !\n";
      Py_XDECREF(py_mbr);
      Py_XDECREF(py_mbr_name);
      throw PyROOT::TPyException();
    }

    PyObject *this_name = ::new_pylist(pyobj_name, py_mbr_name);
    recurse_pyinspect(py_mbr, this_name, pystack, persistentOnly);
    Py_DECREF(this_name);
    Py_DECREF(py_mbr_name);
    Py_DECREF(py_mbr);
    
  }

#if PYROOT_INSPECTOR_DBG
  std::cerr << "==[" << tcls->GetName() << "]== (#mbrs:"
            << nmembers << ")... [done]\n";
#endif

  return;
}

} // anon-namespace

namespace RootUtils {

PyObject*
PyROOTInspector::pyroot_inspect(PyObject* pyobj,
                                bool persistentOnly /*= false*/)
{
  // handle non-pyroot objects
  if (!TPython::ObjectProxy_Check(pyobj)) {
    Py_XINCREF(pyobj);
    return pyobj;
  }

  TClass *tcls = RootUtils::objectIsA(pyobj);
  if (0 == tcls) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  void *obj = TPython::ObjectProxy_AsVoidPtr(pyobj);

  if (!strcmp(tcls->GetName(), "string")) {
    std::string *str = (std::string*)obj;
    return PyROOT_PyUnicode_FromString(str->c_str());
  }

  TString tstring = tcls->GetName();
  if (tstring.BeginsWith("pair<") ||
      tstring.BeginsWith("std::pair<")) {
    PyObject *val = PyTuple_New(2);
    PyObject *v0 = PyObject_GetAttrString(pyobj, "first");
    PyObject *v1 = PyObject_GetAttrString(pyobj, "second");
    PyTuple_SET_ITEM(val, 0, pyroot_inspect(v0, persistentOnly));
    PyTuple_SET_ITEM(val, 1, pyroot_inspect(v1, persistentOnly));
    Py_DECREF(v0);
    Py_DECREF(v1);
    return val;
  }

  Int_t hdr = 0;
  if (PySequence_Check(pyobj)) {
    if (!strcmp(tcls->GetName(), "CLHEP::Hep3Vector")) {
      hdr = 0;
    } else {
      hdr = 1;
    }
  } else {
    hdr = 0;
  }

  TList *members = tcls->GetListOfDataMembers();
  const Int_t nmembers = members->GetEntries();

  PyObject *py_members = PyList_New(nmembers+hdr);
#if PYROOT_INSPECTOR_DBG
  std::cerr << "==[" << tcls->GetName() << "]== (#mbrs:"
            << nmembers 
            << " #stl:" << hdr /*PySequence_Size(pyobj)*/
            << " #py-sz:" << PyList_Size(py_members)
            << ")...\n";
#endif

  if (hdr) {
    // handle collection
    const Py_ssize_t nelems = PySequence_Size(pyobj);
#if PYROOT_INSPECTOR_DBG
    std::cerr << "== sequence (" << nelems << ")...\n";
#endif
    PyObject *py_elems = PyList_New(nelems);
    for (Py_ssize_t i = 0; i < nelems; ++i) {
      PyObject *itr = PySequence_GetItem(pyobj, i);
      PyObject *itr_pyroot = pyroot_inspect(itr, persistentOnly);
      PyList_SET_ITEM(py_elems, i, itr_pyroot);
      Py_DECREF(itr);
      //Py_DECREF(itr_pyroot);
    }
    // add the elements to the "members" list
    PyList_SET_ITEM(py_members, 0, py_elems);
#if PYROOT_INSPECTOR_DBG
    std::cerr << "== sequence (" << nelems << ")... content:\n"
              << ::to_str(py_elems)
              << "\n";
    std::cerr << "== sequence (" << nelems << ")... [done]\n";
#endif
  }


  for (Int_t j = 0; j<nmembers; ++j) {
    TDataMember *mbr = (TDataMember*)(members->At(j));
    Int_t offset =     mbr->GetOffset();
    char *ptr =        (char*)obj + offset;

#if PYROOT_INSPECTOR_DBG
    TClass *mbr_cls = TClass::GetClass(mbr->GetTypeName());
    std::cerr << "==[" << j << "] - [" << mbr->GetTypeName() << "] "
              << "[" << mbr->GetName() 
              << "]"
              << "[" << (mbr_cls ? mbr_cls->GetName() : "N/A") << "]\n";
#endif

    PyObject *py_mbr = 0;

    if (persistentOnly && !mbr->IsPersistent())
      continue;
    if (mbr->IsaPointer())
      continue;
    if (mbr->IsBasic()) {
      TDataType * mbr_type = mbr->GetDataType();
      EDataType mbr_dtype = (EDataType)mbr_type->GetType();
      py_mbr = to_pyobj(ptr, mbr_dtype);
    } else if (mbr->IsEnum()) {
#if PYROOT_INSPECTOR_DBG
      std::cerr << "==[" << mbr->GetTypeName() << "]["
                << mbr->GetDataType()->GetType() << "][val="
                << (*(int*)ptr) << "]["
                << mbr->GetName() << "] is an enum !!\n";
#endif
      py_mbr = PyROOT_PyUnicode_FromString(mbr->GetTypeName());
    } else {
      PyObject *pyroot_obj = TPython::ObjectProxy_FromVoidPtr
        ((void*)ptr,
         mbr->GetTypeName());
      if (pyroot_obj) {
        py_mbr = pyroot_inspect(pyroot_obj, persistentOnly);
      }
      Py_XDECREF(pyroot_obj);
    }
    if (!py_mbr) {
      std::cerr << "could not create py-object of type ["
                << mbr->GetTypeName() << "] !\n";
      Py_DECREF(py_members);
      throw PyROOT::TPyException();
    }

    PyObject *py_item = PyTuple_New(2);
    PyTuple_SET_ITEM(py_item, 0, 
                     PyROOT_PyUnicode_FromString(mbr->GetName()));
    PyTuple_SET_ITEM(py_item, 1, py_mbr);
    PyList_SET_ITEM(py_members, j+hdr, py_item);
  }
#if PYROOT_INSPECTOR_DBG
  std::cerr << "==[" << tcls->GetName() << "]== (#mbrs:"
            << nmembers << ")... [done]\n";
#endif
  return py_members;
}

PyObject*
PyROOTInspector::pyroot_inspect2(PyObject *pyobj,
                                 PyObject *pyobj_name,
                                 bool persistentOnly /*= false*/)
{
  PyObject *pystack = PyList_New(0);
  ::recurse_pyinspect(pyobj, pyobj_name, pystack, persistentOnly);
  return pystack;
}


} // namespace RootUtils


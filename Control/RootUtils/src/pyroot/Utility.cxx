// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootUtils/src/pyroot/Utility.cxx
 * @author scott snyder, Wim Lavrijsen
 * @date Jul, 2015
 * @brief Utility code copied from pyroot.
 */


// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


#include "Utility.h"
#include "TInterpreter.h"
#include <sstream>

namespace RootUtils {
enum EMemoryPolicy { kHeuristics = 1, kStrict = 2 };
}
namespace PyROOT {
struct TCallContext {
      enum ECallFlags {
         kNone           =    0,
         kIsSorted       =    1,   // if method overload priority determined
         kIsCreator      =    2,   // if method creates python-owned objects
         kIsConstructor  =    4,   // if method is a C++ constructor
         kUseHeuristics  =    8,   // if method applies heuristics memory policy
         kUseStrict      =   16,   // if method applies strict memory policy
         kReleaseGIL     =   32,   // if method should release the GIL
         kFast           =   64,   // if method should NOT handle signals
         kSafe           =  128    // if method should return on signals
      };
  static ECallFlags sMemoryPolicy;
  std::vector<int> fArgs;
  UInt_t fFlags;
};
}

namespace RootUtils {


int GetBuffer( PyObject* pyobject, char tc, int size, void*& buf, Bool_t check )
{
// Retrieve a linear buffer pointer from the given pyobject.

// special case: don't handle character strings here (yes, they're buffers, but not quite)
   if ( PyBytes_Check( pyobject ) )
      return 0;

// attempt to retrieve pointer to buffer interface
   PyBufferProcs* bufprocs = Py_TYPE(pyobject)->tp_as_buffer;

   PySequenceMethods* seqmeths = Py_TYPE(pyobject)->tp_as_sequence;
   if ( seqmeths != 0 && bufprocs != 0
#if  PY_VERSION_HEX < 0x03000000
        && bufprocs->bf_getwritebuffer != 0
        && (*(bufprocs->bf_getsegcount))( pyobject, 0 ) == 1
#else
        && bufprocs->bf_getbuffer != 0
#endif
      ) {

   // get the buffer
#if PY_VERSION_HEX < 0x03000000
      Py_ssize_t buflen = (*(bufprocs->bf_getwritebuffer))( pyobject, 0, &buf );
#else
      Py_buffer bufinfo;
      (*(bufprocs->bf_getbuffer))( pyobject, &bufinfo, PyBUF_WRITABLE );
      buf = (char*)bufinfo.buf;
      Py_ssize_t buflen = bufinfo.len;
#endif

      if ( buf && check == kTRUE ) {
      // determine buffer compatibility (use "buf" as a status flag)
         PyObject* pytc = PyObject_GetAttrString( pyobject, "typecode");
         if ( pytc != 0 ) {     // for array objects
            if ( PyROOT_PyUnicode_AsString( pytc )[0] != tc )
               buf = 0;         // no match
            Py_DECREF( pytc );
         } else if ( seqmeths->sq_length &&
                     (int)(buflen / (*(seqmeths->sq_length))( pyobject )) == size ) {
         // this is a gamble ... may or may not be ok, but that's for the user
            PyErr_Clear();
         } else if ( buflen == size ) {
         // also a gamble, but at least 1 item will fit into the buffer, so very likely ok ...
            PyErr_Clear();
         } else {
            buf = 0;                      // not compatible

         // clarify error message
            PyObject* pytype = 0, *pyvalue = 0, *pytrace = 0;
            PyErr_Fetch( &pytype, &pyvalue, &pytrace );
            PyObject* pyvalue2 = PyROOT_PyUnicode_FromFormat(
               (char*)"%s and given element size (%ld) do not match needed (%d)",
               PyROOT_PyUnicode_AsString( pyvalue ),
               seqmeths->sq_length ? (Long_t)(buflen / (*(seqmeths->sq_length))( pyobject )) : (Long_t)buflen,
               size );
            Py_DECREF( pyvalue );
            PyErr_Restore( pytype, pyvalue2, pytrace );
         }
      }

      return buflen;
   }

   return 0;
}


//____________________________________________________________________________
Long_t UpcastOffset( ClassInfo_t* clDerived, ClassInfo_t* clBase, void* obj, bool derivedObj ) {
// Forwards to TInterpreter->ClassInfo_GetBaseOffset(), just adds caching
   if ( clBase == clDerived || !(clBase && clDerived) )
      return 0;

   Long_t offset = gInterpreter->ClassInfo_GetBaseOffset( clDerived, clBase, obj, derivedObj );
   if ( offset == -1 ) {
   // warn to allow diagnostics, but 0 offset is often good, so use that and continue
      std::string bName = gInterpreter->ClassInfo_FullName( clBase );    // collect first b/c
      std::string dName = gInterpreter->ClassInfo_FullName( clDerived ); //  of static buffer
      std::ostringstream msg;
      msg << "failed offset calculation between " << bName << " and " << dName << std::endl;
      PyErr_Warn( PyExc_RuntimeWarning, const_cast<char*>( msg.str().c_str() ) );
      return 0;
   }

   return offset;
}


//- public functions ---------------------------------------------------------
ULong_t PyLongOrInt_AsULong( PyObject* pyobject )
{
// Convert <pybject> to C++ unsigned long, with bounds checking, allow int -> ulong.
   ULong_t ul = PyLong_AsUnsignedLong( pyobject );
   if ( PyErr_Occurred() && PyInt_Check( pyobject ) ) {
      PyErr_Clear();
      Long_t i = PyInt_AS_LONG( pyobject );
      if ( 0 <= i ) {
         ul = (ULong_t)i;
      } else {
         PyErr_SetString( PyExc_ValueError,
            "can\'t convert negative value to unsigned long" );
      }
   }

   return ul;
}

//____________________________________________________________________________
ULong64_t PyLongOrInt_AsULong64( PyObject* pyobject )
{
// Convert <pyobject> to C++ unsigned long long, with bounds checking.
   ULong64_t ull = PyLong_AsUnsignedLongLong( pyobject );
   if ( PyErr_Occurred() && PyInt_Check( pyobject ) ) {
      PyErr_Clear();
      Long_t i = PyInt_AS_LONG( pyobject );
      if ( 0 <= i ) {
         ull = (ULong64_t)i;
      } else {
         PyErr_SetString( PyExc_ValueError,
            "can\'t convert negative value to unsigned long long" );
      }
   }

   return ull;
}


PyObject* getRootModule()
{
  return PyImport_ImportModule ("ROOT");
}


PyObject* rootModule()
{
  static PyObject* const rootModule = getRootModule();
  Py_INCREF (rootModule);
  return rootModule;
}


PyObject* getNullPtrObject()
{
  PyObject* root = rootModule();
  PyObject* ret = PyObject_GetAttrString (root, "nullptr");
  Py_DECREF (root);
  return ret;
}

PyObject* nullPtrObject()
{
  static PyObject* const nullPtr = getNullPtrObject();
  Py_INCREF (nullPtr);
  return nullPtr;
}

TClass* objectIsA (PyObject* obj)
{
  PyObject* repr = PyObject_Repr (obj);
  if (!repr) return nullptr;
  const char* s = PyROOT_PyUnicode_AsString (repr);
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


PyObject* getSetOwnershipFunc()
{
  PyObject* root = rootModule();
  PyObject* ret = PyObject_GetAttrString (root, "SetOwnership");
  Py_DECREF (root);
  return ret;
}

PyObject* setOwnershipFunc()
{
  static PyObject* const func = getSetOwnershipFunc();
  Py_INCREF (func);
  return func;
}

bool setOwnership (PyObject* obj, bool flag)
{
  PyObject* func = setOwnershipFunc();
  PyObject* ret = PyObject_CallFunction (func, const_cast<char*>("OI"),
                                         obj,
                                         static_cast<unsigned int>(flag));
  Py_DECREF (func);
  if (ret) {
    Py_DECREF (ret);
    return true;
  }
  return false;
}


  //CallContext no longer exposed by CPyCppyy
  bool isStrict()
  {
  //using CPyCppyy::CallContext;
  //return CallContext::sMemoryPolicy == CallContext::kUseStrict;
    return true;
  }


bool useStrictOwnership (Long_t user)
{
  using PyROOT::TCallContext;
  TCallContext* ctxt = reinterpret_cast<TCallContext*>(user);
  return ctxt ? (ctxt->fFlags & TCallContext::kUseStrict) :  isStrict();
}


} // namespace RootUtils

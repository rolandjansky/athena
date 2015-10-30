// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootUtils/src/pyroot/Utility.h
 * @author scott snyder, Wim Lavrijsen
 * @date Jul, 2015
 * @brief Utility code copied from pyroot.
 */


#ifndef ROOTUTILS_UTILITY_H
#define ROOTUTILS_UTILITY_H


#include "Python.h"
#include "TPython.h"
#include "TClass.h"


#if PY_VERSION_HEX < 0x03000000
# define PyROOT_PyUnicode_Check              PyString_Check
# define PyROOT_PyUnicode_FromFormat         PyString_FromFormat
# define PyROOT_PyUnicode_AsString           PyString_AS_STRING
# define PyROOT_PyUnicode_AsStringChecked    PyString_AsString
# define PyROOT_PyUnicode_GET_SIZE           PyString_GET_SIZE
# define PyROOT_PyUnicode_FromString         PyString_FromString
# define PyROOT_PyUnicode_FromStringAndSize  PyString_FromStringAndSize
# define PyROOT_PyCapsule_CheckExact    PyCObject_Check
static inline void* PyROOT_PyCapsule_GetPointer( PyObject* capsule, const char* /* name */ )
{
   return (void*)PyCObject_AsVoidPtr( capsule );
}
#else
# define PyROOT_PyUnicode_Check              PyUnicode_Check
# define PyROOT_PyUnicode_FromFormat         PyUnicode_FromFormat
# define PyROOT_PyUnicode_AsString           _PyUnicode_AsString
# define PyROOT_PyUnicode_AsStringChecked    _PyUnicode_AsString
# define PyROOT_PyUnicode_GET_SIZE           PyUnicode_GET_SIZE
# define PyROOT_PyUnicode_FromString         PyUnicode_FromString
# define PyROOT_PyUnicode_FromStringAndSize  PyUnicode_FromStringAndSize
# define PyROOT_PyCapsule_CheckExact    PyCapsule_CheckExact
# define PyROOT_PyCapsule_GetPointer    PyCapsule_GetPointer
#endif


#ifdef R__MACOSX
# if SIZEOF_SIZE_T == SIZEOF_INT
#   if defined(MAC_OS_X_VERSION_10_4)
#      define PY_SSIZE_T_FORMAT "%ld"
#   else
#      define PY_SSIZE_T_FORMAT "%d"
#   endif
# elif SIZEOF_SIZE_T == SIZEOF_LONG
#   define PY_SSIZE_T_FORMAT "%ld"
# endif
#else
# define PY_SSIZE_T_FORMAT "%zd"
#endif



namespace RootUtils {


int GetBuffer( PyObject* pyobject, char tc, int size, void*& buf, Bool_t check = kTRUE );
Long_t UpcastOffset( ClassInfo_t* clDerived, ClassInfo_t* clBase, void* obj, bool derivedObj );
inline Long_t UpcastOffset( TClass* clDerived, TClass* clBase, void* obj, bool derivedObj ) {
  return UpcastOffset( clDerived->GetClassInfo(), clBase->GetClassInfo(), obj, derivedObj );
}


  ULong_t PyLongOrInt_AsULong( PyObject* pyobject );
  ULong64_t PyLongOrInt_AsULong64( PyObject* pyobject );
  TClass* objectIsA (PyObject* obj);
  PyObject* nullPtrObject();
  bool setOwnership (PyObject* obj, bool flag);
  bool isStrict();
  bool useStrictOwnership (Long_t user);

}


#endif // not ROOTUTILS_UTILITY_H

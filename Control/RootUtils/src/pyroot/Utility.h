// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootUtils/src/pyroot/Utility.h
 * @author scott snyder, Wim Lavrijsen
 * @date Jul, 2015
 * @brief Utility code originally from pyroot.
 */


#ifndef ROOTUTILS_UTILITY_H
#define ROOTUTILS_UTILITY_H


#include "Python.h"
#include "TPython.h"
#include "TClass.h"


#if PY_VERSION_HEX < 0x03000000
# define PyROOT_PyUnicode_Type               PyString_Type
# define PyROOT_PyUnicode_FromFormat         PyString_FromFormat
# define PyROOT_PyUnicode_AsString           PyString_AS_STRING
# define PyROOT_PyUnicode_GET_SIZE           PyString_GET_SIZE
# define PyROOT_PyUnicode_FromString         PyString_FromString
# define PyROOT_PyUnicode_FromStringAndSize  PyString_FromStringAndSize
# define PyROOT_PyUnicode_InternFromString   PyString_InternFromString
#else
# define PyROOT_PyUnicode_Type               PyUnicode_Type
# define PyROOT_PyUnicode_FromFormat         PyUnicode_FromFormat
# define PyROOT_PyUnicode_AsString           _PyUnicode_AsString
# define PyROOT_PyUnicode_GET_SIZE           PyUnicode_GET_SIZE
# define PyROOT_PyUnicode_FromString         PyUnicode_FromString
# define PyROOT_PyUnicode_FromStringAndSize  PyUnicode_FromStringAndSize
# define PyROOT_PyUnicode_InternFromString   PyUnicode_InternFromString
#endif


#if ROOT_VERSION_CODE < ROOT_VERSION(6,22,0)
# define CPPInstance_Check         ObjectProxy_Check
# define CPPInstance_AsVoidPtr     ObjectProxy_AsVoidPtr
# define CPPInstance_FromVoidPtr   ObjectProxy_FromVoidPtr
#include "TPyException.h"
namespace RootUtils {
  using PyException = PyROOT::TPyException;
}
#else
#include "CPyCppyy/PyException.h"
namespace RootUtils {
  using PyException = CPyCppyy::PyException;
}
#endif



namespace RootUtils {


int GetBuffer( PyObject* pyobject, char tc, int size, void*& buf, Bool_t check = kTRUE );
TClass* objectIsA (PyObject* obj);
bool setOwnership (PyObject* obj, bool flag);


}


#endif // not ROOTUTILS_UTILITY_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file RootUtils/PyGetString.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Convert python string -> C++ string for py2 and py3.
 */


#ifndef ROOTUTILS_PYGETSTRING_H
#define ROOTUTILS_PYGETSTRING_H

#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE
#endif
#include "Python.h"
#include <string>
#include <utility>


namespace RootUtils {


/**
 * @brief Convert python string -> C++ string for py2 and py3.
 *
 * Returns a pair (string, bool); the second element is true of the conversion
 * succeeded.
 *
 * This should be kept as an inline function to avoid having to have direct
 * dependencies on python where it's not really needed.
 */
inline
std::pair<std::string, bool> PyGetString (PyObject* s)
{
  std::string sout;
  bool flag = false;
  bool own_s = false;
  if (s) {

    // Convert a unicode object to a (byte)string.
    if (PyUnicode_Check (s)) {
      PyObject* o = PyCodec_Encode (s, "utf-8", nullptr);
      s = o;
      own_s = true;
    }

#if PY_VERSION_HEX < 0x03000000
    // Compiled against python 2.7: Use PyString_AS_STRING.
    if (PyString_Check (s)) {
      // If we've been compiled against python 2.7 but we're running with python 3,
      // things almost work... but the start of the data of a python3 bytes
      // object is offset by 4 bytes from the python2 string object.
      static const bool ispy3 = Py_GetVersion()[0] >= '3';
      // Use the macro: PyString_AsString isn't in python3.
      const char* sptr = PyString_AS_STRING (s);
      if (ispy3) sptr -= 4;
      sout = sptr;
      flag = true;
    }
#else
    // python3 version.
    if (PyBytes_Check (s)) {
      sout = PyBytes_AsString (s);
      flag = true;
    }
#endif
  }

  if (own_s) {
    Py_XDECREF (s);
  }
  return std::make_pair (sout, flag);
}


} // namespace RootUtils

#endif // not ROOTUTILS_PYGETSTRING_H

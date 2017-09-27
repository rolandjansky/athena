// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file RootUtils/PyROOTPickle.h
 * @author Wim Lavrijsen
 * @date Apr 2008
 * @brief Port pickling functionality while awaiting newer release.
 *
 * PyROOT does not implement direct pickling of ObjectProxy's until release
 * 5.20 of ROOT; this code brings it forward for use in ATLAS.
 *
 * The code here will make the following changes to PyROOT's behavior:
 *
 *  - Add a method "_ObjectProxy__expand__" to libPyROOT that is stored by
 *     name by the pickler used by the unpickler. The expand function takes
 *     a class name and a python string, which are turned into an object
 *     using TBufferFile.
 *
 *  - Add a "__reduce__" method to ObjectProxy, which turns ObjectProxy's
 *     into python strings through serialization done by TBufferFile. The
 *     python string is subsequently pickled.
 */


#ifndef ROOTUTILS_PYROOTPICKLE_H
#define ROOTUTILS_PYROOTPICKLE_H


#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#include "Python.h"


namespace RootUtils {


class PyROOTPickle
{
public:
  /**
   * @brief Install the pickling of ObjectProxy's functionality.
   * @param libpyroot_pymodule The libPyROOT python module
   * @param objectproxy_pytype The ObjectProxy python type
   */
  static void Initialize (PyObject* libpyroot_pymodule,
                          PyObject* objectproxy_pytype);
};


} // namespace RootUtils


#endif // not ROOTUTILS_PYROOTPICKLE_H

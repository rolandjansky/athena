// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootUtils/PyROOTTTreePatch.h
 * @author scott snyder
 * @date Nov 2007, revised Aug 2020
 * @brief Additions to root's pythonized @c TTree.
 *
 * The code here will make the following changes to PyROOT's behavior:
 *
 *  - The @c SetNotify method now takes a Python object, and performs
 *     a Python call to the @c Notify method.  This allows for Python
 *     notification callbacks.  The Python object is saved in the
 *     @c __pynotify__ attribute of the tree Python object.  The @c GetNotify
 *     method is changed to return this Python object.
 *
 *  - A Pythonizations is provided for @c TBranch::SetAddress.
 */


#ifndef ROOTUTILS_PYROOTTTREEPATCH_H
#define ROOTUTILS_PYROOTTTREEPATCH_H

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE
#endif
#include "Python.h"


namespace RootUtils {


class PyROOTTTreePatch
{
public:
  /**
   * @brief Install the PyROOT patches.
   * @param tree_pyclass The @c TTree Python class.
   * @param chain_pyclass The @c TChain Python class.
   * @param branch_pyclass The @c TBranch Python class.
   */
  static void Initialize (PyObject* tree_pyclass,
                          PyObject* chain_pyclass,
                          PyObject* branch_pyclass);
};


} // namespace RootUtils


#endif // not ROOTUTILS_PYROOTTTREEPATCH_H

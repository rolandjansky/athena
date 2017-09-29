// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PyROOTTTreePatch.h,v 1.4 2007-12-13 20:15:30 ssnyder Exp $

/**
 * @file RootUtils/PyROOTTTreePatch.h
 * @author scott snyder
 * @date Nov 2007
 * @brief Performance improvements and fixes for root's pythonized @c TTree.
 *
 * PyROOT does a nice `pythonization' of @c TTree, such that `tree.foo' will
 * return the value of branch/leaf `foo' in `tree'.  There's a performance
 * issue, though --- each time `tree.foo' is executed, it does a
 * @c GetBranch/@c GetLeaf to find `foo' in the tree.  But these functions have
 * been observed to be bad for performance: they do a linear search
 * though the list of leaves/branches (in the case of @c TBranchElement,
 * which are in a tree, it's a depth-first search), so this kills you
 * if the tree is large (thousands of branches/leaves are not unusual
 * for Atlas).
 *
 * In the case of a branch containing an object, it's possible to avoid
 * most of this cost by holding onto the object, rather than re-evaluating
 * `tree.foo' on each event.  Note, though, that this doesn't work
 * for non-object branches/leaves --- these return unique values
 * or buffers each time they're called.  Also, if you're using
 * a @c TChain, the objects that are returned become invalid after
 * moving to a new file in the @c TChain.  This can be dealt with
 * in two ways.  One can set up a callback via @c SetNotify to call
 * when the file changes.  However, there's no way, given the tools
 * in PyROOT, to get @c Notify to call back to Python, without writing
 * C++ code.  The other is to use @c SetBranchAddress on the @c TChain
 * to fix the branch's buffer address.  However, PyROOT only declares
 * the @c SetBranchAddress pythonization for @c TTree --- it doesn't
 * work for @c TChain.  It also doesn't work to call @c SetAddress
 * directly on a @c TBranch.
 *
 * The code here will make the following changes to PyROOT's behavior:
 *
 *  - The lookups via @c GetBranch and GetLeaf are @c cached.  The cache is
 *     stored as a @c __elements__ attribute of the tree Python object.
 *     This cache is flushed on a @c TChain file change.  The notification
 *     object is stored in the __notifier__ object of the tree
 *     Python object.
 *
 *  - The @c SetNotify method now takes a Python object, and performs
 *     a Python call to the @c Notify method.  This allows for Python
 *     notification callbacks.  The Python object is saved in the
 *     @c __pynotify__ attribute of the tree Python object.  The @c GetNotify
 *     method is changed to return this Python object.
 *
 *  - Pythonizations are provided for @c TChain::SetBranchAddress and
 *     @c TBranch::SetAddress so that they'll work correctly.
 *
 *  - As a final help for making things work efficiently, if a branch
 *     referenced via `tree.foo' is disabled, it is automatically enabled.
 *     This allows one to disable all branches at the start, and allow
 *     branches to be enabled lazily.
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

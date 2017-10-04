// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PyROOTTypePatch.h,v 1.1 2007-12-13 20:01:56 ssnyder Exp $

/**
 * @file RootUtils/PyROOTTTypePatch.h
 * @author scott snyder
 * @date Nov 2007
 * @brief setattr hook for PyROOT classes to apply container fixes.
 *
 * PyROOT (as of root 5.16.00) has a bug that bites us when accessing
 * @c DataVector classes.
 * 
 *  - Attempts to use element access via [] on a class deriving from DataVector
 *    will cause an infinite recursion.
 *
 * This patch sets up to automatically apply fixes for this for all
 * container classes.
 *
 * We were doing this by making explicit calls during job configuration.
 * This was unsatisfactory for two reasons:
 *
 *  - One needs to explicitly list the classes to fix up.  If you add a new
 *    DV class, you need to remember to add a fixup for this.
 *  - Applying the fixups means that we were force-loading all of the
 *    affected classes at that time.  Not only can this be wasteful
 *    (loading more stuff than may be needed), but it was causing some
 *    initialization ordering problems.  (Sometimes, the message service
 *    was being implicitly created by the libraries being loaded; this
 *    then caused the ApplicationMgr initialization to fail.)  So,
 *    we want to be able to defer applying the fixups until the python
 *    classes are created.
 *
 * PyROOT doesn't provide any way to hook into class creation.
 * However, for the container types, the `Pythonization' process
 * will be assigning attributes of the class, including @c _getitem__unchecked.
 * So what we do is we patch @c PyRootType (the metaclass of all
 * PyROOT classes) to add our own setattr method.  This will watch
 * for assignments of @c _getitem__unchecked; when this is seen,
 * the workarounds will be applied.  Otherwise, it just calls the
 * generic setattr.
 */


#ifndef ROOTUTILS_PYROOTTYPEPATCH_H
#define ROOTUTILS_PYROOTTYPEPATCH_H

// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#include "Python.h"


namespace RootUtils {


class PyROOTTypePatch
{
public:
  /**
   * @brief Install the fixup.
   * @param pyroottype The Python PyRootType object.
   */
  static void initialize (PyObject* pyroottype);
};


} // namespace RootUtils


#endif // not ROOTUTILS_PYROOTTYPEPATCH_H

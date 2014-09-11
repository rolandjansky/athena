// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClearCINTMessageCallback.h,v 1.2 2007-10-30 16:49:30 ssnyder Exp $

/**
 * @file RootUtils/ClearCINTMessageCallback.h
 * @author scott snyder
 * @date Oct 2007
 * @brief Clear Cint's error message callback.
 */


#ifndef ROOTUTILS_CLEARCINTMESSAGECALLBACK_H
#define ROOTUTILS_CLEARCINTMESSAGECALLBACK_H


namespace RootUtils {


/**
 * @brief Clear Cint's error message callback.
 *
 * PyROOT sets up a callback to intercept Cint's error messages
 * and turn them into Python errors.  That's fine if you're using
 * Python as a shell.  But if you're using Cint as a shell
 * (and calling python through TPython), then this will result
 * in Cint's error messages being lost.
 *
 * Now, PyROOT itself is smart enough to only install the callback
 * if python is actually being used as a shell.  However, PyCintex
 * installs the callback unconditionally.  This function can be
 * used to undo the damage.
 */
class ClearCINTMessageCallback
{
public:
  /**
   * @brief Clear Cint's error message callback.
   *
   * This could (almost) just as well be a free function.
   * The only reason to put it inside a class is that Root doesn't
   * auto-load functions, only classes.
   */
  static void initialize();
};


} // namespace RootUtils


#endif // not ROOTUTILS_CLEARCINTMESSAGECALLBACK_H

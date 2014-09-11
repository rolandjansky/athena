// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PyLogger.h,v 1.1 2007-12-15 06:54:27 ssnyder Exp $

/**
 * @file  RootUtils/PyLogger.h
 * @author scott snyder
 * @date Dec 2007
 * @brief A concrete implementation of ILogger that calls back to Python.
 */


#ifndef ROOTUTILS_PYLOGGER_H
#define ROOTUTILS_PYLOGGER_H


#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#include "Python.h"

#include "RootUtils/ILogger.h"


namespace RootUtils {


/**
 * @brief A concrete implementation of ILogger that calls back to Python.
 */
class PyLogger
  : public RootUtils::ILogger
{
public:
  /**
   * @brief Constructor.
   * @param debugfn Function to call for a debug message.
   * @param errorfn Function to call for an error message.
   *
   * The functions should take a single string parameter.
   * Passing None means to skip the call.
   */
  PyLogger (PyObject* debugfn, PyObject* errorfn);


  /// Destructor.
  ~PyLogger();


  /**
   * @brief Log a debugging message.
   * @param msg The message to log.
   */
  virtual void debug (const char* msg);


  /**
   * @brief Log an error message.
   * @param msg The message to log.
   */
  virtual void error (const char* msg);


private:
  /**
   * @brief Common code to call Python callback.
   * @param fn The Python function to call (or None).
   * @param msg The string to pass to it.
   *
   * May raise an exception, if the Python code does.
   */
  static void call (PyObject* fn, const char* msg);


  /// The debug message function.
  PyObject* m_debugfn;

  /// The error message function.
  PyObject* m_errorfn;
};


} // namespace RootUtils


#endif // not ROOTUTILS_PYLOGGER_H

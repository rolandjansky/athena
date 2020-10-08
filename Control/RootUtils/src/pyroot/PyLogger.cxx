/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  RootUtils/src/pyroot/PyLogger.cxx
 * @author scott snyder
 * @date Dec 2007
 * @brief A concrete implementation of ILogger that calls back to Python.
 */


#include "RootUtils/PyLogger.h"
#include "Utility.h"


namespace RootUtils {


/**
 * @brief Constructor.
 * @param debugfn Function to call for a debug message.
 * @param errorfn Function to call for an error message.
 *
 * The functions should take a single string parameter.
 * Passing None means to skip the call.
 */
PyLogger::PyLogger (PyObject* debugfn, PyObject* errorfn)
  : m_debugfn (debugfn),
    m_errorfn (errorfn)
{
  Py_INCREF (debugfn);
  Py_INCREF (errorfn);
}


/**
 * @brief Destructor.
 */
PyLogger::~PyLogger()
{
  Py_DECREF (m_debugfn);
  Py_DECREF (m_errorfn);
}


/**
 * @brief Log a debugging message.
 * @param msg The message to log.
 */
void PyLogger::debug (const char* msg)
{
  call (m_debugfn, msg);
}


/**
 * @brief Log an error message.
 * @param msg The message to log.
 */
void PyLogger::error (const char* msg)
{
  call (m_errorfn, msg);
}


/**
 * @brief Common code to call Python callback.
 * @param fn The Python function to call (or None).
 * @param msg The string to pass to it.
 *
 * May raise an exception, if the Python code does.
 */
void PyLogger::call (PyObject* fn, const char* msg)
{
  if (!fn || fn == Py_None)
    return;

  PyObject* res = PyObject_CallFunction (fn, (char*)"s", msg);
  if (res == 0)
    throw RootUtils::PyException();
  Py_DECREF (res);
}


} // namespace RootUtils

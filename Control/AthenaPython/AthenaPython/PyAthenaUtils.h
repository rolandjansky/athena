///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaUtils.h 
// Header file for various PyAthena helpers and utils
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYATHENAUTILS_H 
#define ATHENAPYTHON_PYATHENAUTILS_H 

// STL includes

// FrameWork includes
#include "GaudiKernel/StatusCode.h"
#include "CxxUtils/checker_macros.h"

// Forward declaration
class InterfaceID;
class StatusCode;
// Python
struct _object;
typedef _object PyObject;

namespace PyAthena {

  /** @brief returns the string representation of a python object
   *  equivalent of calling `repr(o)` in python
   */
  std::string repr (PyObject* o);

  /** @brief returns the string representation of a python object
   *  equivalent of calling `str(o)` in python
   */
  std::string str (PyObject* o);

  /// call the python method
  StatusCode callPyMethod ATLAS_NOT_THREAD_SAFE ( PyObject* self,
                                                  const char* method,
                                                  PyObject* arg = nullptr );

  /// query interface binding
  StatusCode queryInterface ATLAS_NOT_THREAD_SAFE ( PyObject* self,
                                                    const InterfaceID& riid,
                                                    void** ppvInterface );

  /// helper function for PyAthena::Aud
  void pyAudit ATLAS_NOT_THREAD_SAFE ( PyObject* self, 
                                       const char* method,
                                       const char* evt, const char* component );

  /// helper function for PyAthena::Aud
  void pyAudit ATLAS_NOT_THREAD_SAFE ( PyObject* self, 
                                       const char* method,
                                       const char* evt, const char* component,
                                       const StatusCode& sc );

  /// helper function to capture the boilerplate code for user friendly
  /// stack trace display
  void throw_py_exception (bool display = true);

}

#endif // ATHENAPYTHON_PYATHENAUTILS_H 

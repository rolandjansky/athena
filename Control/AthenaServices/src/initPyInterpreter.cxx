///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// initPyInterpreter.cxx 
// state of the art initialization of the python interpreter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Python
#include "Python.h"
// fixes 'dereferencing type-punned pointer will break strict-aliasing rules'
#ifdef Py_True
#undef Py_True
#define Py_True ( (PyObject*)(void*)&_Py_TrueStruct )
#endif
#ifdef Py_False
#undef Py_False
#define Py_False ( (PyObject*)(void*)&_Py_ZeroStruct )
#endif

// gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/StatusCode.h"

namespace {
  /// helper function to capture the boilerplate code for user friendly
  /// stack trace display
  void report_py_exception (bool display = true)
  {
    if (display) {
      // fetch error
      PyObject* pytype = 0, *pyvalue = 0, *pytrace = 0;
      PyErr_Fetch (&pytype, &pyvalue, &pytrace);
      Py_XINCREF  (pytype);
      Py_XINCREF  (pyvalue);
      Py_XINCREF  (pytrace);
      // restore...
      PyErr_Restore (pytype, pyvalue, pytrace);
      // and print
      PyErr_Print();
    }
  }
}

namespace AthenaInternal {
/** @brief correctly initialize the python interpreter in case it hasn't
 *         been done yet.
 */
StatusCode initPyInterpreter();

StatusCode 
initPyInterpreter()
{
  if (Py_IsInitialized()) {
    return StatusCode::SUCCESS;
  }

  if (!PyEval_ThreadsInitialized()) {
    PyEval_InitThreads();
  }

  Py_Initialize();

  if (!Py_IsInitialized()) {
    ::report_py_exception();
    return StatusCode::FAILURE;
  }

  // init the sys.argv...
  PySys_SetArgv(System::argc(),System::argv());
  return StatusCode::SUCCESS;
}

} //> !AthenaInternal


///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "CxxUtils/checker_macros.h"

// gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/StatusCode.h"

#include <locale>
#include <codecvt>

namespace {
  /// helper function to capture the boilerplate code for user friendly
  /// stack trace display
  void report_py_exception (bool display = true)
  {
    if (display) {
      // fetch error
      PyObject* pytype = nullptr, *pyvalue = nullptr, *pytrace = nullptr;
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

  /*
   * The GIL is initialized by Py_Initialize() since Python 3.7."
   */

  Py_Initialize();

  if (!Py_IsInitialized()) {
    ::report_py_exception();
    return StatusCode::FAILURE;
  }

  // init the sys.argv...
#if PY_MAJOR_VERSION < 3
  PySys_SetArgv(System::argc(), System::argv());
#else
  auto wargsinit = 
    []() { std::vector<std::wstring> wargs;
           int argc = System::argc();
           char** argv = System::argv();
           wargs.reserve (argc);
           using convert_t = std::codecvt_utf8<wchar_t>;
           std::wstring_convert<convert_t, wchar_t> strconverter;
           for (int i=0; i < argc; ++i) {
             wargs.push_back (strconverter.from_bytes (argv[i]));
           }
           return wargs;
  };
  static const std::vector<std::wstring> wargs = wargsinit();

  auto wargvinit =
    [](const std::vector<std::wstring>& wargs)
      { std::vector<const wchar_t*> wargv;
        int argc = System::argc();
        for (int i=0; i < argc; ++i) {
          wargv.push_back (wargs[i].data());
        }
        return wargv;
  };
  static const std::vector<const wchar_t*> wargv = wargvinit (wargs);

  // Bleh --- python takes non-const argv pointers.
  wchar_t** wargv_nc ATLAS_THREAD_SAFE = const_cast<wchar_t**> (wargv.data());
  PySys_SetArgv(System::argc(), wargv_nc);
#endif
  return StatusCode::SUCCESS;
}

} //> !AthenaInternal


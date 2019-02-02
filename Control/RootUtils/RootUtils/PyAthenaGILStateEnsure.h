///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaGILStateEnsure.h 
// Header file for convenience class to ensure GIL
// Author: Wim Lavrijsen <WLavrijsen@lbl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef ROOTUTILS_PYGILSTATEENSURE_H
#define ROOTUTILS_PYGILSTATEENSURE_H 

// Python includes
#include "Python.h"

namespace RootUtils {

class PyGILStateEnsure
{ 
 public: 
  PyGILStateEnsure();
  PyGILStateEnsure( const PyGILStateEnsure& ) = delete;
  PyGILStateEnsure& operator=( const PyGILStateEnsure& ) = delete;
  ~PyGILStateEnsure(); 

 private: 
  PyGILState_STATE m_gstate;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline PyGILStateEnsure::PyGILStateEnsure()
  : m_gstate (PyGILState_Ensure())
{ 
}

inline PyGILStateEnsure::~PyGILStateEnsure()
{ 
   PyGILState_Release( m_gstate );
}

} //> namespace RootUtils

#endif //> ROOTUTILS_PYATHENA_PYGILSTATEENSURE_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootUtils/src/pyroot/TCustomPyTypes.h
 * @author scott snyder, Wim Lavrijsen
 * @date Jul 2015
 * @brief Copied from pyroot.
 */


// Author: Wim Lavrijsen, Dec 2006

#ifndef ROOTUTILS_TCUSTOMPYTYPES_H
#define ROOTUTILS_TCUSTOMPYTYPES_H

#include "Python.h"
#include "DllImport.h"
#include "TClass.h"


namespace RootUtils {

/** Custom builtins, detectable by type, for pass by ref
      @author  WLAV
      @date    12/13/2006
      @version 1.0
 */

//- custom float object type and type verification ---------------------------
   R__EXTERN PyTypeObject TCustomFloat_Type;

   template< typename T >
   inline Bool_t TCustomFloat_Check( T* object )
   {
      return object && PyObject_TypeCheck( object, &TCustomFloat_Type );
   }

   template< typename T >
   inline Bool_t TCustomFloat_CheckExact( T* object )
   {
      return object && Py_TYPE(object) == &TCustomFloat_Type;
   }

//- custom long object type and type verification ----------------------------
   R__EXTERN PyTypeObject TCustomInt_Type;

   template< typename T >
   inline Bool_t TCustomInt_Check( T* object )
   {
      return object && PyObject_TypeCheck( object, &TCustomInt_Type );
   }

   template< typename T >
   inline Bool_t TCustomInt_CheckExact( T* object )
   {
      return object && Py_TYPE(object) == &TCustomInt_Type;
   }

//- custom instance method object type and type verification -----------------
   R__EXTERN PyTypeObject TCustomInstanceMethod_Type;

   template< typename T >
   inline Bool_t TCustomInstanceMethod_Check( T* object )
   {
      return object && PyObject_TypeCheck( object, &TCustomInstanceMethod_Type );
   }

   template< typename T >
   inline Bool_t TCustomInstanceMethod_CheckExact( T* object )
   {
      return object && Py_TYPE(object) == &TCustomInstanceMethod_Type;
   }

   PyObject* TCustomInstanceMethod_New( PyObject* func, PyObject* self, PyObject* pyclass );

} // namespace RootUtils

#endif // !ROOTUTILS_TCUSTOMPYTYPES_H

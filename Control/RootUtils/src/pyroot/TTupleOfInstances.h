// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootUtils/src/pyroot/TTupleOfInstances.h
 * @author scott snyder, Wim Lavrijsen
 * @date Jul 2015
 * @brief Copied from pyroot.
 */


#ifndef ROOTUTILS_TTUPLEOFINSTANCES_H
#define ROOTUTILS_TTUPLEOFINSTANCES_H

// ROOT
#include "Python.h"
#include "DllImport.h"
#include "TClass.h"


namespace RootUtils {

/** Representation of C-style array of instances
      @author  WLAV
      @date    02/10/2014
      @version 1.0
 */

//- custom tuple type that can pass through C-style arrays -------------------
   R__EXTERN PyTypeObject TTupleOfInstances_Type;

   template< typename T >
   inline Bool_t TTupleOfInstances_Check( T* object )
   {
      return object && PyObject_TypeCheck( object, &TTupleOfInstances_Type );
   }

   template< typename T >
   inline Bool_t TTupleOfInstances_CheckExact( T* object )
   {
      return object && Py_TYPE(object) == &TTupleOfInstances_Type;
   }

   PyObject* TTupleOfInstances_New( void* address, TClass* klass, Py_ssize_t size );

} // namespace RootUtils

#endif // !ROOTUTILS_TTUPLEOFINSTANCES_H

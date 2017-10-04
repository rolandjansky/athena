// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootUtils/src/pyroot/TPyBufferFactory.h
 * @author scott snyder, Wim Lavrijsen
 * @date Jul 2015
 * @brief C++ -> root converters
 *
 * This is mostly copied from pyroot (which unfortunately doesn't
 * export this functionality).
 */


// @(#)root/pyroot:$Id$
// Author: Wim Lavrijsen, Apr 2004

#ifndef ROOTUTILS_TPYBUFFERFACTORY_H
#define ROOTUTILS_TPYBUFFERFACTORY_H


// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

namespace RootUtils {

/** Factory for python buffers of non-string type
      @author  WLAV
      @date    10/28/2004
      @version 1.5
*/

class TPyBufferFactory {
public:
   static TPyBufferFactory* Instance();

   PyObject* PyBuffer_FromMemory( Bool_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( Bool_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( Short_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( Short_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( UShort_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( UShort_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( Int_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( Int_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( UInt_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( UInt_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( Long_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( Long_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( ULong_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( ULong_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( Float_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( Float_t* buf, PyObject* sizeCallback );
   PyObject* PyBuffer_FromMemory( Double_t* buf, Py_ssize_t size = -1 );
   PyObject* PyBuffer_FromMemory( Double_t* buf, PyObject* sizeCallback );

protected:
   TPyBufferFactory();
   ~TPyBufferFactory();
};

typedef TPyBufferFactory BufFac_t;

} // namespace RootUtils


#endif // !ROOTUTILS_TPYBUFFERFACTORY_H

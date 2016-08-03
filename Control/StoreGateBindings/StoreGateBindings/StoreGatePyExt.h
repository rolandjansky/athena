/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATEBINDINGS_STOREGATEPYEXT_H
#define STOREGATEBINDINGS_STOREGATEPYEXT_H

///These methods provide a Python Extension for StoreGate
///Client is POOLRootAccess package, for example
///You must "#include "Python.h" before including this header
///this will give you the correct PyObject


class StoreGateSvc;

struct _object;
typedef _object PyObject;

namespace AthenaInternal {

  ///retrieve object of specified type from storegate
PyObject* 
 retrieveObjectFromStore( StoreGateSvc* store, 
			  PyObject* tp, PyObject* pykey );

///typeless retrieve ... slower than above, and potential to return unexpected type if objects of different type but same key

PyObject* 
py_sg_getitem (StoreGateSvc* self,
                               PyObject* pykey);

///check if object of specified type is in storegate
PyObject*
  py_sg_contains (StoreGateSvc* store,
		 PyObject* tp, PyObject* pykey);


///record object to storegate
PyObject* 
recordObjectToStore( StoreGateSvc* store,
                                     PyObject* obj,
                                     PyObject* pykey,
                                     bool allowMods = true,
                                     bool resetOnly = true,
                                     bool noHist = false);

}//AthenaInternal namespace


#endif

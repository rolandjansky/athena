///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATEBINDINGS_STOREGATEBINDINGSDICT_H 
#define STOREGATEBINDINGS_STOREGATEBINDINGSDICT_H  

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/IThinningSvc.h"

struct _object;
typedef _object PyObject;

namespace AthenaInternal {

  class ROOT6_StoreGateBindings_WorkAround_Dummy {};

  PyObject*
  retrieveObjectFromStore( PyObject* storeGateSvc, 
                           PyObject* tp, PyObject* key );

  PyObject* 
  recordObjectToStore( PyObject* storeGateSvc, 
                       PyObject* obj,
                       PyObject* key,
                       bool allowMods = true,
                       bool resetOnly = true,
                       bool noHist = false);

  PyObject* 
  py_sg_contains(PyObject* storeGateSvc,
                 PyObject* tp, PyObject* key);

  PyObject*
  py_sg_getitem(PyObject* storeGateSvc,
                PyObject* key);

  void
  py_sg_clearProxyPayload(StoreGateSvc*, SG::DataProxy*);

  PyObject* thinContainer( IThinningSvc*,
                           PyObject* container,
                           PyObject* filter,
                           int op );

  std::size_t thinIdxContainer( IThinningSvc*,
                                PyObject* container, 
                                std::size_t idx );

  std::size_t thinRemovedIdx();
}

#endif // not STOREGATEBINDINGS_STOREGATEBINDINGSDICT_H 

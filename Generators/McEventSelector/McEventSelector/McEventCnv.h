/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCEVENTSELECTOR_MCEVENTCNV_H
#define MCEVENTSELECTOR_MCEVENTCNV_H
#include "GaudiKernel/Converter.h"

class IOpaqueAddress;
class DataObject;
class StatusCode;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 

class McEventCnv: public Converter {
  friend class CnvFactory<McEventCnv>;

 protected:
  McEventCnv(ISvcLocator* svcloc);

 public:
  StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 

  /// Storage type and class ID
  static long storageType();    
  static const CLID& classID();                 

  virtual long repSvcType() const  {
    return i_repSvcType();
  }
  
};
#endif

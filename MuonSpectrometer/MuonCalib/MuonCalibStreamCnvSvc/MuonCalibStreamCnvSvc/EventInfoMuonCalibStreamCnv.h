/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMCNVSVC_EVENTINFOMUONCALIBSTREAMCNV_H
#define MUONCALIBSTREAMCNVSVC_EVENTINFOMUONCALIBSTREAMCNV_H

#include "GaudiKernel/Converter.h"
#include "MuCalDecode/CalibEvent.h"

class IOpaqueAddress;
class DataObject;
class StatusCode;
class MuonCalibStreamCnvSvc; 
class MuonCalibStreamDataProviderSvc; 

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long MuonCalibStream_StorageType;

class EventInfoMuonCalibStreamCnv: public Converter {
  friend class CnvFactory<EventInfoMuonCalibStreamCnv>;

 public:
  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const    { return MuonCalibStream_StorageType; }
  static long storageType()     { return MuonCalibStream_StorageType; }
  static const CLID& classID();

 protected:
  EventInfoMuonCalibStreamCnv(ISvcLocator* svcloc);

 private: 
  const char* ascTime( unsigned int t) ;
  MuonCalibStreamCnvSvc* m_MuonCalibStreamCnvSvc; 
  MuonCalibStreamDataProviderSvc* m_dataProvider; 
};
#endif

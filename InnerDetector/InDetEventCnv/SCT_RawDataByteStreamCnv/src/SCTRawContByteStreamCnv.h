/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTCNV_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTCNV_H

// Gaudi
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"

// C++ Standard Library
#include <string>

// Athena
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

class DataObject;
class IByteStreamEventAccess;
class ISCTRawContByteStreamService;

/** Externals */ 
extern long ByteStream_StorageType;

/** the converter for writing BS from SCT Raw Data 
 * This will do the conversion on demand, triggered by the 
 * ByteStreamAddressProviderSvc. 
 * Since it is not possible to configure a Converter with
 * Python Configurables, we use a service (SCTRawContByteStreamService)
 * which in turn uses the  lightweight SCT_RodEncoder class, 
 * to do the actual converting. */

class SCTRawContByteStreamCnv: public Converter {
  friend class CnvFactory<SCTRawContByteStreamCnv>;
  
  ~SCTRawContByteStreamCnv();

 protected:

  SCTRawContByteStreamCnv(ISvcLocator* svcloc);
  
 public:
  typedef SCT_RDO_Container SCTRawContainer; 

  /** Storage type and class ID */
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static long storageType() { return ByteStream_StorageType; } 
  static const CLID& classID() { return ClassID_traits<SCTRawContainer>::ID(); }
  
  /** initialize */
  virtual StatusCode initialize();
  
  /** create Obj is not used ! */
  virtual StatusCode createObj(IOpaqueAddress*, DataObject*&)
  { return StatusCode::FAILURE;}

  /** this creates the RawEvent fragments for the SCT */
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

 private: 
  /** for BS infrastructure */
  ServiceHandle<ISCTRawContByteStreamService> m_service;
  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess; 
  MsgStream m_log;
};
#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTCNV_H

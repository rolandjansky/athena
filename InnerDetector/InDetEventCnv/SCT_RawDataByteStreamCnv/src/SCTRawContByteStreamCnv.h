/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMCNV_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMCNV_H

#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"

class DataObject;
class ISCTRawContByteStreamTool;
class MsgStream;

extern long ByteStream_StorageType;

/** the converter for writing BS from SCT Raw Data 
 * This will do the conversion on demand, triggered by the 
 * ByteStreamAddressProviderSvc. 
 * Since it is not possible to configure a Converter with
 * Python Configurables, we use a service (SCTRawContByteStreamService)
 * which in turn uses the  lightweight SCT_RodEncoder class, 
 * to do the actual converting. */

class SCTRawContByteStreamCnv: public Converter {

 public:
  SCTRawContByteStreamCnv(ISvcLocator* svcLoc);

  virtual ~SCTRawContByteStreamCnv() = default;
  
  /** Storage type and class ID */
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static long storageType() { return ByteStream_StorageType; } 
  static const CLID& classID() { return ClassID_traits<SCT_RDO_Container>::ID(); }
  
  /** initialize */
  virtual StatusCode initialize();
  
  /** create Obj is not used ! */
  virtual StatusCode createObj(IOpaqueAddress*, DataObject*&)
  { return StatusCode::FAILURE;}

  /** this creates the RawEvent fragments for the SCT */
  virtual StatusCode createRep(DataObject* pDataObject, IOpaqueAddress*& pOpaqueAddress);

 private: 
  /** for BS infrastructure */
  ToolHandle<ISCTRawContByteStreamTool> m_rawContByteStreamTool;
  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess; 
  MsgStream m_log;
};
#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMCNV_H

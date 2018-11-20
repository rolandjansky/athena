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

/** 
 * @class SCTRawContByteStreamCnv
 *
 * @brief Converter for writing ByteStream from SCT Raw Data
 *
 * This will do the conversion on demand, triggered by the ByteStreamAddressProviderSvc. 
 * Since it is not possible to configure a Converter with Python Configurables, 
 * we use a service (SCTRawContByteStreamService) which in turn uses the lightweight 
 * SCT_RodEncoder class, to do the actual converting. 
 */
class SCTRawContByteStreamCnv: public Converter {

 public:

  /** Constructor */
  SCTRawContByteStreamCnv(ISvcLocator* svcLoc);

  /** Destructor */
  virtual ~SCTRawContByteStreamCnv() = default;
  
  /** Storage type and class ID */
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static long storageType() { return ByteStream_StorageType; } 
  static const CLID& classID() { return ClassID_traits<SCT_RDO_Container>::ID(); }
  
  /** Initialize */
  virtual StatusCode initialize();
  
  /** createObj method (not used!) */
  virtual StatusCode createObj(IOpaqueAddress*, DataObject*&)
  { return StatusCode::FAILURE;}

  /** 
   * @brief Method to convert SCT Raw Data into ByteStream
   *
   * Gets pointer to RawEvent, get ID container of SCT RDO.
   * Sets up opaque address for Storegate.
   *
   * Uses SCT RawContByteStreamTool to convert Raw Data to ByteStream.
   *
   * @param pDataObject Pointer to data object
   * @param pOpaqueAddress Opaque address to object
   */
  virtual StatusCode createRep(DataObject* pDataObject, IOpaqueAddress*& pOpaqueAddress);

 private: 

  /** Tool to do coversion from SCT RDO container to ByteStream */
  ToolHandle<ISCTRawContByteStreamTool> m_rawContByteStreamTool;

  /** Interface for accessing raw data */
  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess; 

  /** Object used to transmit messages and log errors */
  MsgStream m_log;
};
#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMCNV_H

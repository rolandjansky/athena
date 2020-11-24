// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMCNV_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMCNV_H

#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "AthenaBaseComps/AthConstConverter.h"

#include "GaudiKernel/ServiceHandle.h"

class DataObject;
class ISCTRawContByteStreamTool;

/**
 * @class SCTRawContByteStreamCnv
 *
 * @brief Converter for writing ByteStream from SCT Raw Data
 *
 * This will do the conversion on demand, triggered by the ByteStreamAddressProviderSvc. 
 * Since it is not possible to configure a Converter with Python Configurables, 
 * we use a tool (SCTRawContByteStreamTool) which in turn uses the lightweight 
 * SCT_RodEncoder class, to do the actual converting. 
 */
class SCTRawContByteStreamCnv : public AthConstConverter 
{
 public:

  /** Constructor */
  SCTRawContByteStreamCnv(ISvcLocator* svcLoc);

  /** Destructor */
  virtual ~SCTRawContByteStreamCnv() = default;
  
  /** Initialize */
  virtual StatusCode initialize() override;

  /** Retrieve the class type of the data store the converter uses. */
  virtual long repSvcType() const override { return i_repSvcType(); }
  /** Storage type */
  static long storageType() { return ByteStreamAddress::storageType(); }
  /** Class ID */
  static const CLID& classID() { return ClassID_traits<SCT_RDO_Container>::ID(); }
  
  /** createObj method (not used!) */
  virtual StatusCode createObjConst(IOpaqueAddress*, DataObject*&) const override { return StatusCode::FAILURE; }

  /** 
   * @brief Method to convert SCT Raw Data into ByteStream
   *
   * Gets pointer to RawEvent, get ID container of SCT RDO.
   * Sets up opaque address for Storegate.
   *
   * Uses SCT RawContByteStreamTool to convert Raw Data to ByteStream.
   *
   * @param pDataObject Pointer to data object.
   * @param pOpaqueAddress Opaque address to object.
   */
  virtual StatusCode createRepConst(DataObject* pDataObject, IOpaqueAddress*& pOpaqueAddress) const override;

 private: 

  /** Tool to do coversion from SCT RDO container to ByteStream */
  ToolHandle<ISCTRawContByteStreamTool> m_rawContByteStreamTool;

  /** Interface for accessing raw data */
  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess; 
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMCNV_H

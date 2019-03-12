/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class CpmRoiByteStreamTool;

/** ByteStream converter for Cluster Processor Module RoIs.
 *
 *  @author Peter Faulkner
 */

class CpmRoiByteStreamCnv: public Converter {

  friend class CnvFactory<CpmRoiByteStreamCnv>;

protected:

  CpmRoiByteStreamCnv(ISvcLocator* svcloc);

public:

  ~CpmRoiByteStreamCnv();

  virtual StatusCode initialize();
  /// Create CPM RoIs from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Create ByteStream from CPM RoIs
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::CpmRoiByteStreamTool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  /// Service for writing bytestream
  ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess;

  /// Message log
  mutable MsgStream m_log;
  bool m_debug;

};

} // end namespace

#endif

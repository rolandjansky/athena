/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV2CNV_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV2CNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;

// Externals
extern long ByteStream_StorageType;

namespace LVL1BS {

class PpmByteStreamV2Tool;

/** ByteStream converter for Pre-processor Module DAQ data / TriggerTowers.
 *
 *  @author alexander.mazurov@cern.ch
 *  @author Peter Faulkner
 */

class PpmByteStreamV2Cnv: public Converter, public ::AthMessaging {

  friend class CnvFactory<PpmByteStreamV2Cnv>;

protected:

  PpmByteStreamV2Cnv(ISvcLocator* svcloc);

public:

  virtual ~PpmByteStreamV2Cnv();

  virtual StatusCode initialize();
  /// Create TriggerTowers from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Create ByteStream from TriggerTowers
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static  long storageType(){ return ByteStream_StorageType; }
  static const CLID& classID();

private:
  void _reserveMemory();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::PpmByteStreamV2Tool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  /// Service for writing bytestream
  ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess;
};

} // end namespace

#endif

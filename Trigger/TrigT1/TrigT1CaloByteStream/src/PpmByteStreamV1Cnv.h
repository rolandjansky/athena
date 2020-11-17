/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV1CNV_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMV1CNV_H

#include <string>

#include "AthenaBaseComps/AthConstConverter.h"
#include "GaudiKernel/ClassID.h"
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

class PpmByteStreamV1Tool;

/** ByteStream converter for Pre-processor Module DAQ data / TriggerTowers.
 *
 *  @author Peter Faulkner
 */

class PpmByteStreamV1Cnv: public AthConstConverter {

public:
  PpmByteStreamV1Cnv(ISvcLocator* svcloc);

  virtual ~PpmByteStreamV1Cnv();

  virtual StatusCode initialize() override;
  /// Create TriggerTowers from ByteStream
  virtual StatusCode createObjConst (IOpaqueAddress* pAddr, DataObject*& pObj) const override;
  /// Create ByteStream from TriggerTowers
  virtual StatusCode createRepConst (DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::PpmByteStreamV1Tool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  /// Service for writing bytestream
  ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess;
};

} // end namespace

#endif

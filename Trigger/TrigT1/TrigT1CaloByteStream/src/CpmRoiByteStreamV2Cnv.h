/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV2CNV_H
#define TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV2CNV_H

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

class CpmRoiByteStreamV2Tool;

/** ByteStream converter for Cluster Processor Module RoIs post LS1.
 *
 *  @author Peter Faulkner
 */

class CpmRoiByteStreamV2Cnv: public AthConstConverter {
public:
  CpmRoiByteStreamV2Cnv(ISvcLocator* svcloc);

  virtual ~CpmRoiByteStreamV2Cnv();

  virtual StatusCode initialize() override;
  /// Create CPM RoIs from ByteStream
  virtual StatusCode createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const override;
  /// Create ByteStream from CPM RoIs
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::CpmRoiByteStreamV2Tool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
};

} // end namespace

#endif

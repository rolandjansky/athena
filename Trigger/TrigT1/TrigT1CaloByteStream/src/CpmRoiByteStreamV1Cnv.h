/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV1CNV_H
#define TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV1CNV_H

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

class CpmRoiByteStreamV1Tool;

/** ByteStream converter for Cluster Processor Module RoIs.
 *
 *  @author Peter Faulkner
 */

class CpmRoiByteStreamV1Cnv: public AthConstConverter {
public:
  CpmRoiByteStreamV1Cnv(ISvcLocator* svcloc);

  virtual ~CpmRoiByteStreamV1Cnv();

  virtual StatusCode initialize() override;
  /// Create CPM RoIs from ByteStream
  virtual StatusCode createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const override;
  /// Create ByteStream from CPM RoIs
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override ;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::CpmRoiByteStreamV1Tool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
};

} // end namespace

#endif

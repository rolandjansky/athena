/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPREADBYTESTREAMV1V2CNV_H
#define TRIGT1CALOBYTESTREAM_CPREADBYTESTREAMV1V2CNV_H

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

class CpByteStreamV1Tool;
class CpByteStreamV2Tool;

/** ByteStream converter for Cluster Processor Module Towers
 *  allowing for data containing pre-LS1 or post-LS1 format sub-blocks.
 *  For reading only.
 *
 *  @author Peter Faulkner
 */

class CpReadByteStreamV1V2Cnv: public AthConstConverter {

public:
  CpReadByteStreamV1V2Cnv(ISvcLocator* svcloc);

  virtual ~CpReadByteStreamV1V2Cnv();

  virtual StatusCode initialize() override;
  /// Create CPM Towers from ByteStream
  virtual StatusCode createObjConst (IOpaqueAddress* pAddr, DataObject*& pObj) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work pre-LS1
  ToolHandle<LVL1BS::CpByteStreamV1Tool> m_tool1;
  /// Tool that does the actual work post-LS1
  ToolHandle<LVL1BS::CpByteStreamV2Tool> m_tool2;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
};

} // end namespace

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMCNV_H

#include <string>

#include "AthenaBaseComps/AthConstConverter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IOpaqueAddress;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class RodHeaderByteStreamTool;

/** ByteStream converter for L1Calo ROD header info
 *
 *  @author Peter Faulkner
 */

class RodHeaderByteStreamCnv: public AthConstConverter {

public:
  RodHeaderByteStreamCnv(ISvcLocator* svcloc);

  virtual ~RodHeaderByteStreamCnv();

  virtual StatusCode initialize() override;
  /// Create RodHeaders from ByteStream
  virtual StatusCode createObjConst (IOpaqueAddress* pAddr, DataObject*& pObj) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::RodHeaderByteStreamTool> m_tool;
};

} // end namespace

#endif

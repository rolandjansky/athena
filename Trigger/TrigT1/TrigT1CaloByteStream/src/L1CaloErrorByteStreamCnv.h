/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_L1CALOERRORBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_L1CALOERRORBYTESTREAMCNV_H

#include <string>

#include "AthenaBaseComps/AthConstConverter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IOpaqueAddress;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class L1CaloErrorByteStreamTool;

/** Returns vector of errors detected during data unpacking
 *
 *  @author Peter Faulkner
 */

class L1CaloErrorByteStreamCnv: public AthConstConverter {

public:
  L1CaloErrorByteStreamCnv(ISvcLocator* svcloc);

  virtual ~L1CaloErrorByteStreamCnv();

  virtual StatusCode initialize() override;
  /// Create error vector from ByteStream
  virtual StatusCode createObjConst (IOpaqueAddress* pAddr, DataObject*& pObj) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_tool;
};

} // end namespace

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_CPBYTESTREAMCNV_H

#include <string>

#include "AthenaBaseComps/AthConstConverter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class CpByteStreamTool;

/** ByteStream converter for CP container
 *
 *  @author Peter Faulkner
 */

class CpByteStreamCnv: public AthConstConverter {

  friend class CnvFactory<CpByteStreamCnv>;

protected:

  CpByteStreamCnv(ISvcLocator* svcloc);

public:

  virtual ~CpByteStreamCnv();

  virtual StatusCode initialize() override;
  /// Create ByteStream from Cp Container
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::CpByteStreamTool> m_tool;
};

} // end namespace

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMCNV_H

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

class JepRoiByteStreamTool;

/** ByteStream converter for JEP RoI container
 *
 *  @author Peter Faulkner
 */

class JepRoiByteStreamCnv: public AthConstConverter {

  friend class CnvFactory<JepRoiByteStreamCnv>;

protected:

  JepRoiByteStreamCnv(ISvcLocator* svcloc);

public:

  virtual ~JepRoiByteStreamCnv();

  virtual StatusCode initialize() override;
  /// Create ByteStream from JEP Container
  virtual StatusCode createRepConst (DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::JepRoiByteStreamTool> m_tool;
};

} // end namespace

#endif

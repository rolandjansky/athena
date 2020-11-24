/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMV2CNV_H
#define TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMV2CNV_H

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

class JepRoiByteStreamV2Tool;

/** ByteStream converter for JEP RoI container post LS1
 *
 *  @author Peter Faulkner
 */

class JepRoiByteStreamV2Cnv: public AthConstConverter {
public:
  JepRoiByteStreamV2Cnv(ISvcLocator* svcloc);

  virtual ~JepRoiByteStreamV2Cnv();

  virtual StatusCode initialize() override;
  /// Create ByteStream from JEP Container
  virtual StatusCode createRepConst (DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<LVL1BS::JepRoiByteStreamV2Tool> m_tool;
};

} // end namespace

#endif

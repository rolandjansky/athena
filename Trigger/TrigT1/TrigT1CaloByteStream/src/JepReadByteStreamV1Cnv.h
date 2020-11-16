/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPREADBYTESTREAMV1CNV_H
#define TRIGT1CALOBYTESTREAM_JEPREADBYTESTREAMV1CNV_H

#include <string>

#include "AthenaBaseComps/AthConstConverter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class JepByteStreamV1Tool;

/** ByteStream converter for JEP component containers.
 *
 *  @author Peter Faulkner
 */

template <typename Container>
class JepReadByteStreamV1Cnv: public AthConstConverter {

public:
  JepReadByteStreamV1Cnv(ISvcLocator* svcloc);

  virtual ~JepReadByteStreamV1Cnv();

  virtual StatusCode initialize() override;
  /// Create Container from ByteStream
  virtual StatusCode createObjConst (IOpaqueAddress* pAddr, DataObject*& pObj) const override;

  //  Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:
  /// Tool that does the actual work
  ToolHandle<JepByteStreamV1Tool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
};

} // end namespace

#include "JepReadByteStreamV1Cnv.icc"

#endif

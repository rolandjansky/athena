/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPREADBYTESTREAMV1CNV_H
#define TRIGT1CALOBYTESTREAM_JEPREADBYTESTREAMV1CNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
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
class JepReadByteStreamV1Cnv: public Converter {

public:
  JepReadByteStreamV1Cnv(ISvcLocator* svcloc);

  ~JepReadByteStreamV1Cnv();

  virtual StatusCode initialize();
  /// Create Container from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<JepByteStreamV1Tool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  /// Message log
  mutable MsgStream m_log;
  bool m_debug;

};

} // end namespace

#include "JepReadByteStreamV1Cnv.icc"

#endif

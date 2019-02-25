/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPBYTESTREAMV2CNV_H
#define TRIGT1CALOBYTESTREAM_CPBYTESTREAMV2CNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class CpByteStreamV2Tool;

/** ByteStream converter for CP container post LS1
 *
 *  @author Peter Faulkner
 */

class CpByteStreamV2Cnv: public Converter {
public:
  CpByteStreamV2Cnv(ISvcLocator* svcloc);

  ~CpByteStreamV2Cnv();

  virtual StatusCode initialize();
  /// Create ByteStream from Cp Container
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::CpByteStreamV2Tool> m_tool;

  /// Service for writing bytestream
  ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess;

  /// Message log
  mutable MsgStream m_log;
  bool m_debug;

};

} // end namespace

#endif

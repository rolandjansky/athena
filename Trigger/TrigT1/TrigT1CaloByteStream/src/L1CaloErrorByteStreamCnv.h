/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_L1CALOERRORBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_L1CALOERRORBYTESTREAMCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
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

class L1CaloErrorByteStreamCnv: public Converter {

public:
  L1CaloErrorByteStreamCnv(ISvcLocator* svcloc);

  ~L1CaloErrorByteStreamCnv();

  virtual StatusCode initialize();
  /// Create error vector from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::L1CaloErrorByteStreamTool> m_tool;

  /// Message log
  mutable MsgStream m_log;
  bool m_debug;

};

} // end namespace

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_L1TOPOBYTESTREAMAUXCNV_H
#define TRIGT1CALOBYTESTREAM_L1TOPOBYTESTREAMAUXCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename>
class CnvFactory;

class L1TopoByteStreamTool;

namespace LVL1BS {

/** ByteStream converter for Pre-processor Module DAQ data / TriggerTowers.
 *
 *  @author alexander.mazurov@cern.ch
 */

class L1TopoByteStreamAuxCnv : public Converter, public ::AthMessaging {

public:
  L1TopoByteStreamAuxCnv(ISvcLocator* svcloc);

 public:
  virtual ~L1TopoByteStreamAuxCnv(){};

  virtual StatusCode initialize();
  /// Create TriggerTowers from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Create ByteStream from TriggerTowers
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static long storageType();

  static const CLID& classID();

 private:
  /// Converter name
  std::string m_name;

  /// Do the main job - retrieve xAOD Topo from robs
  ToolHandle<L1TopoByteStreamTool> m_readTool;
};

}  // end namespace
#endif

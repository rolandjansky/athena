/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_RPCPADCONTBYTESTREAMCNV_H
#define MUONBYTESTREAM_RPCPADCONTBYTESTREAMCNV_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 

#include "MuonRPC_CnvTools/IRPC_RDOtoByteStreamTool.h"

class DataObject;
class StatusCode;
class IByteStreamEventAccess;
class StoreGateSvc; 

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class RpcPadContByteStreamCnv: public Converter {

 public:
  RpcPadContByteStreamCnv(ISvcLocator* svcloc);

  typedef Muon::IRPC_RDOtoByteStreamTool  BYTESTREAMTOOL ; 

  virtual StatusCode initialize() override;
  virtual StatusCode createObj(IOpaqueAddress* /*pAddr*/, DataObject*& /*pObj*/) override {
    return StatusCode::FAILURE;
  }
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private: 
   ToolHandle<BYTESTREAMTOOL> m_tool ; 
   ServiceHandle<IByteStreamEventAccess>    m_byteStreamEventAccess; 
   ServiceHandle<StoreGateSvc>              m_storeGate;
};
#endif




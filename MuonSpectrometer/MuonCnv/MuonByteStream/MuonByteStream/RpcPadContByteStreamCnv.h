/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_RPCPADCONTBYTESTREAMCNV_H
#define MUONBYTESTREAM_RPCPADCONTBYTESTREAMCNV_H

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "MuonRPC_CnvTools/IRPC_RDOtoByteStreamTool.h"
#include "AthenaBaseComps/AthConstConverter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class DataObject;
class StatusCode;


class RpcPadContByteStreamCnv: public AthConstConverter {

 public:
  RpcPadContByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;
  virtual StatusCode createObjConst(IOpaqueAddress* /*pAddr*/, DataObject*& /*pObj*/) const override {
    return StatusCode::FAILURE;
  }
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private: 
   ToolHandle<Muon::IRPC_RDOtoByteStreamTool> m_tool ; 
};
#endif




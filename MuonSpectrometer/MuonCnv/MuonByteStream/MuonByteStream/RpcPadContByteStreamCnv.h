/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_RPCPADCONTBYTESTREAMCNV_H
#define MUONBYTESTREAM_RPCPADCONTBYTESTREAMCNV_H

#include <stdint.h>
#include <map>
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 

//#include "MuonByteStream/RPC_Hid2RESrcID.h"
//#include "MuonByteStream/RpcROD_Decoder.h"
#include "MuonRPC_CnvTools/IRPC_RDOtoByteStreamTool.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class MsgStream; 

// namespace Muon {
// class IRPC_RDOtoByteStreamTool; 
// }
#include <string>

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
   //const IRPCcablingSvc* m_cabling;
   //RPC_Hid2RESrcID* m_hid2re  ; 

   ServiceHandle<IByteStreamEventAccess>    m_byteStreamEventAccess; 
   //RpcPadContainer* m_container ; 

   //RpcROD_Decoder* m_decoder;

   //bool m_SLbuilt;
   
   ServiceHandle<StoreGateSvc>              m_storeGate;
};
#endif




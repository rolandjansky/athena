/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPC_CNVTOOLS_RPCPADCONTRAWEVENTTOOL_H
#define MUONRPC_CNVTOOLS_RPCPADCONTRAWEVENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonRPC_CnvTools/IRPC_RDOtoByteStreamTool.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "RPC_Hid2RESrcID.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

class RpcPadContainer;

#include <string>

namespace Muon {

/** An AlgTool class to provide conversion from LArRawChannelContainer
  *  to ByteStream, and fill it in RawEvent
  *  created:  Sept 25, 2002, by Hong Ma 
  *  requirements:   typedef for CONTAINER class method 
  *  statusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
  *
  * Adapted for Muons by Ketevi A. Assamagan
  * Jan-14-2003, BNL
  * Conversion of Rpc Pad Container to byte stream
  */

class RpcPadContByteStreamTool : public AthAlgTool, virtual public IRPC_RDOtoByteStreamTool 
{

public:

  typedef RpcPadContainer CONTAINER ; 

  /** constructor
  */
  RpcPadContByteStreamTool( const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~RpcPadContByteStreamTool()=default;

  /** AlgTool InterfaceID
  */
  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() override;

  virtual StatusCode convert(CONTAINER* cont, RawEventWrite* re) const override;

private:
   SG::ReadCondHandleKey<RpcCablingCondData> m_readKey{this, "ReadKey", "RpcCablingCondData", "Key of RpcCablingCondData"};
   RPC_Hid2RESrcID m_hid2re; 
   ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc
    { this, "ByteStreamCnvSvc", "ByteStreamCnvSvc" };

};
}

#endif




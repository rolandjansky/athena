/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPC_CNVTOOLS_RPCPADCONTRAWEVENTTOOL_H
#define MUONRPC_CNVTOOLS_RPCPADCONTRAWEVENTTOOL_H

#include <stdint.h>
#include <map>
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRPC_CnvTools/IRPC_RDOtoByteStreamTool.h"
#include "RPC_Hid2RESrcID.h"

#include "ByteStreamData/RawEvent.h" 

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

class RpcPadContainer; 
class MsgStream ; 
class RpcIdHelper;
class IRPCcablingSvc;
//class RPC_Hid2RESrcID;


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
   RpcPadContByteStreamTool( const std::string& type, const std::string& name,
        const IInterface* parent ) ;

  /** destructor 
  */ 
  virtual ~RpcPadContByteStreamTool() ;

  /** AlgTool InterfaceID
  */
  static const InterfaceID& interfaceID( ) ;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convert(CONTAINER* cont, RawEventWrite* re, MsgStream& log ); 

private: 

   const IRPCcablingSvc* m_cabling;
   RPC_Hid2RESrcID m_hid2re; 
   const RpcIdHelper * m_rpcHelper;

   FullEventAssembler<RPC_Hid2RESrcID> m_fea ;    
};
}

#endif




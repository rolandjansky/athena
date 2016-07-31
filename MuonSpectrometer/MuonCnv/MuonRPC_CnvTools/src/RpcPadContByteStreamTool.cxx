/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcPadContByteStreamTool.h"
#include "RpcROD_Encoder.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "RPC_Hid2RESrcID.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

//#include "StoreGate/StoreGate.h"
//#include "StoreGate/tools/ClassID_traits.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <map> 

static const InterfaceID IID_IRpcPadContByteStreamTool
            ("Muon::RpcPadContByteStreamTool", 1, 0);

const InterfaceID& Muon::RpcPadContByteStreamTool::interfaceID( ) { 
  return IID_IRpcPadContByteStreamTool; 
}

// default contructor
 
Muon::RpcPadContByteStreamTool::RpcPadContByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
:  AthAlgTool(type,name,parent) {
  declareInterface< IRPC_RDOtoByteStreamTool  >( this );
}

// destructor 
 
Muon::RpcPadContByteStreamTool::~RpcPadContByteStreamTool() 
{}
  
StatusCode Muon::RpcPadContByteStreamTool::initialize() {

  //   MsgStream log(msgSvc(), "RpcROD_Decoder::fillCollection");  

   StatusCode sc = AlgTool::initialize(); 

   if (StatusCode::SUCCESS !=sc) {
     msg(MSG::ERROR) << " Base class initialisation failed " << endmsg;
     return StatusCode::FAILURE; 
   }  
  
  const IRPCcablingServerSvc* RpcCabGet = 0;
  
  if (StatusCode::SUCCESS != service("RPCcablingServerSvc", RpcCabGet,true)) {
    msg(MSG::ERROR) << " Can't get RPCcablingServerSvc " << endmsg;
    return StatusCode::FAILURE; 
  }
   
  if (StatusCode::SUCCESS != RpcCabGet->giveCabling(m_cabling)) {
    msg(MSG::ERROR) << " Can't get RPCcablingSvc from Server" << endmsg;
    return StatusCode::FAILURE; 
  }  

//   StoreGateSvc * detStore;
//   StatusCode status = service("DetectorStore", detStore);
//   if (status.isFailure()) {
//     msg(MSG::FATAL) << "DetectorStore service not found !" << endmsg;
//     return StatusCode::FAILURE;
//   } else {}
 
  // Get the RPC id helper from the detector store
  const RpcIdHelper* rpc_id;
  StatusCode status = detStore()->retrieve(rpc_id, "RPCIDHELPER");
  if (status.isFailure()) {
    msg(MSG::FATAL) << "Could not get RpcIdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg(MSG::DEBUG) << " Found the RpcIdHelper. " << endmsg;
  }
 
  m_hid2re.set(m_cabling, rpc_id);
  m_rpcHelper = rpc_id;
  return sc;
}

 
StatusCode Muon::RpcPadContByteStreamTool::finalize() {
   StatusCode sc = AlgTool::finalize(); 

   return sc;
}


StatusCode Muon::RpcPadContByteStreamTool::convert(CONTAINER* cont, RawEventWrite* re, 
					     MsgStream& log ) {

   m_fea.clear();
   m_fea.idMap().set(m_cabling,m_rpcHelper);

   FullEventAssembler<RPC_Hid2RESrcID>::RODDATA*  theROD ;
 
   RpcPadContainer::const_iterator it_coll = cont->begin(); 
   RpcPadContainer::const_iterator it_coll_end = cont->end(); 

   std::map<uint32_t, RpcROD_Encoder> mapEncoder; 

   //log <<MSG::DEBUG<<" number of collections "<< cont->size() << endmsg;
   for( ; it_coll!=it_coll_end;++it_coll) {
      const RpcPad* coll = (*it_coll);

      int sector = coll->sector();
      uint32_t rodId = m_hid2re.getRodID(sector); 
      mapEncoder[rodId].add(coll);
    } 

    std::map<uint32_t,RpcROD_Encoder>::iterator it     = mapEncoder.begin(); 
    std::map<uint32_t,RpcROD_Encoder>::iterator it_end = mapEncoder.end();

    // RpcROD_Encoder has collected all the pads, now can fill the
    // ROD block data. 

    for (; it!=it_end;++it) { 
      theROD  = m_fea.getRodData( (*it).first ); 
      ((*it).second).set( &m_hid2re ) ; 
      ((*it).second).fillROD( *theROD ) ; 
    } 

    m_fea.fill(re,log); 

    return StatusCode::SUCCESS; 

}







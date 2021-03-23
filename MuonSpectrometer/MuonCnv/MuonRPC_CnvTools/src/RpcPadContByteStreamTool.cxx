/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcPadContByteStreamTool.h"
#include "RpcROD_Encoder.h"

#include "RPC_Hid2RESrcID.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <map> 

static const InterfaceID IID_IRpcPadContByteStreamTool
            ("Muon::RpcPadContByteStreamTool", 1, 0);

const InterfaceID& Muon::RpcPadContByteStreamTool::interfaceID( ) { 
  return IID_IRpcPadContByteStreamTool; 
}

Muon::RpcPadContByteStreamTool::RpcPadContByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
:  AthAlgTool(type,name,parent) {
  declareInterface< IRPC_RDOtoByteStreamTool  >( this );
}
  
StatusCode Muon::RpcPadContByteStreamTool::initialize() {
  ATH_CHECK(AlgTool::initialize()); 
  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  m_hid2re.set(&m_idHelperSvc->rpcIdHelper());

  ATH_CHECK( m_byteStreamCnvSvc.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcPadContByteStreamTool::convert(CONTAINER* cont, RawEventWrite* /*re*/) const
{
   FullEventAssembler<RPC_Hid2RESrcID>* fea = nullptr;
   ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler (fea,
                                                         "RpcRdoContByteStream")
              );

   fea->idMap().set(&m_idHelperSvc->rpcIdHelper());

   FullEventAssembler<RPC_Hid2RESrcID>::RODDATA* theROD;
 
   RpcPadContainer::const_iterator it_coll = cont->begin(); 
   RpcPadContainer::const_iterator it_coll_end = cont->end(); 

   std::map<uint32_t, RpcROD_Encoder> mapEncoder; 

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
      theROD  = fea->getRodData( (*it).first ); 
      ((*it).second).set( &m_hid2re ) ; 
      ((*it).second).fillROD( *theROD ) ; 
    } 
    
    return StatusCode::SUCCESS; 
}


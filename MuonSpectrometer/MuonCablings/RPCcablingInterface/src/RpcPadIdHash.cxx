/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCcablingInterface/RpcPadIdHash.h" 
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/RpcIdHelper.h"


#include <iostream> 

// This class converts a Rpc Identifier into a integer, according to its
// fields 


// default contructor 
RpcPadIdHash::RpcPadIdHash( ) {

  IMessageSvc*  msgSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
  if (sc.isFailure()) 
  {
     throw GaudiException("Cannot retrieve the Message Service",
                         "RpcPadIdHash::RpcPadIdHash( )", StatusCode::FAILURE); 
  }
  
  MsgStream log(msgSvc, "RpcPadIdHash" );

  log << MSG::DEBUG << "Into RpcPadIdHash Constructor " << endmsg; 

  // initialize RPC cabling service
  const IRPCcablingServerSvc* RpcCabGet = 0;
  sc = svcLoc->service("RPCcablingServerSvc", RpcCabGet, true);
  if (sc != StatusCode::SUCCESS ) 
  {
      throw GaudiException("Cannot retrieve the RPCcabling server",
                         "RpcPadIdHash::RpcPadIdHash( )", StatusCode::FAILURE);
  }

  sc = RpcCabGet->giveCabling(m_cabling);
  if (sc != StatusCode::SUCCESS ) 
  {
     throw GaudiException("Cannot retrieve the RPCcabling from the server",
                         "RpcPadIdHash::RpcPadIdHash( )", StatusCode::FAILURE);

  }

  for(int side=0;side<2;++side)
      for(int rod=0;rod<16;++rod)
          (m_rod2hash[side][rod]).clear();

  // loop over side, sector logic and pad Id to find the correct pad!
  const CablingRPCBase::RDOmap& pad_map = m_cabling->give_RDOs();

  CablingRPCBase::RDOmap::const_iterator pad_beg = (pad_map).begin(); 
  CablingRPCBase::RDOmap::const_iterator pad_end = (pad_map).end();

  for (; pad_beg!=pad_end; ++pad_beg) 
  {
    RDOindex index = (*pad_beg).second;
    Identifier id;
    index.pad_identifier(id);
    
    m_int2id.push_back(id);
    
    if( m_int2id.size() != index.hash()+1 )
    {
        log << MSG::FATAL << "Inconsistence between PAD hash and RpcPadIdHash"
	    << endmsg;
	log << MSG::FATAL << index << endmsg;
	log << MSG::FATAL << "Position into RpcPadIdHash map is " 
	    << m_int2id.size() - 1 << endmsg;
	    
	throw GaudiException("Cannot build the map for RpcPadIdHash ",
                         "RpcPadIdHash::RpcPadIdHash( )", StatusCode::FAILURE);
    }

    unsigned short int side = index.side();
    unsigned short int rod  = index.RODid();
    
    if( rod > 15 )
    {
        log << MSG::FATAL << "RPC ROD greater than 15" << endmsg;
	throw GaudiException("Cannot build the map for Rod2Hash ",
                         "RpcPadIdHash::RpcPadIdHash( )", StatusCode::FAILURE);
    }
    
    IdentifierHash HashID = index.hash();
    
    (m_rod2hash[(side==0x66)? 0 : 1][rod]).push_back(HashID);

    m_lookup[id]=index.hash(); 
 }

  log << MSG::DEBUG << "Number of valid RPC Pad IDs " << m_int2id.size() 
      << endmsg ; 
  
  m_size = m_int2id.size(); 
}

RpcPadIdHash::ID RpcPadIdHash::identifier(int index) const {

  if(index>=0 && index < m_size) { 
   return m_int2id[index] ; 
  }

  // return blank id for invalid index 
  std::cout <<"RpcPadIdHash:: ERROR : identifier invalid ID "<<std::endl; 
  Identifier id;
  return id; 

}

int RpcPadIdHash::operator() (const ID& id) const {

 std::map<ID,int>::const_iterator it = m_lookup.find(id); 
 if(it!=m_lookup.end()){
	return (*it).second; 
 }

 // FIXME  -1 for invalid ID?
 return -1; 

}

int RpcPadIdHash::max() const {

return m_size; 

}

int RpcPadIdHash::offset() const {

return 0;

}

std::vector<IdentifierHash> 
RpcPadIdHash::rod2hash(uint16_t subsystem_id, uint16_t rod_id) const {

    if( (subsystem_id!=0x66 && subsystem_id!=0x65) ||
        (rod_id>=16) ) return std::vector<IdentifierHash>();
	
    return m_rod2hash[(subsystem_id==0x66)? 0 : 1][rod_id];
}

uint32_t 
RpcPadIdHash::hash2source(unsigned int h) const
{
    unsigned short int ROBid;
    unsigned short int RODid;
    unsigned short int side;
    unsigned short int SLid;
    unsigned short int RXid;
    unsigned short int PADid;
    
    if( m_cabling->giveOnlineID(h,ROBid,RODid,side,SLid,RXid,PADid) )
    {
        uint32_t sub = side;
	uint32_t rod = RODid;
	return (sub << 16) | rod;
    }
    return 0;
}



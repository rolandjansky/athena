/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRDO/MdtCsmIdHash.h"

#include <cassert> 
#include <iostream> 

// This class converts a Mdt Identifier into a integer, according to its
// fields 


// default contructor 
MdtCsmIdHash::MdtCsmIdHash( )   
{
  IMessageSvc*  msgSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  assert(svcLoc);
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
  if (sc.isFailure()) std::cout << "Fail to locate Message Service" << std::endl;

  MsgStream log(msgSvc, "MuonDigitContainer" );

  log << MSG::DEBUG << " MdtCsmIdHash Constructor "<<endmsg; 

  const MdtIdHelper* mdtHelper=nullptr;
  StoreGateSvc* detStore=nullptr;
  sc = svcLoc->service("DetectorStore", detStore);

  if (sc.isSuccess()) {
    sc = detStore->retrieve( mdtHelper, "MDTIDHELPER" );
    if (sc.isFailure())
      log << MSG::ERROR << " Cannot retrieve MdtIdHelper " << endmsg;
  } else 
    log << MSG::ERROR << " MdtIdHelper not found in DetectorStore " << endmsg;
  //too bad, we bail out (PC)
  if (!sc.isSuccess()) throw GaudiException("can not get MdtIdHelper", 
					    "MdtCsmIdHash::MdtCsmIdHash()", sc);

 
  unsigned int used = mdtHelper->module_hash_max();
  IdContext context = mdtHelper->module_context();

  if( mdtHelper->stationNameIndex("BME") != -1 ) {
    used = mdtHelper->detectorElement_hash_max();
    context = mdtHelper->detectorElement_context();
  }

  for(unsigned int hash=0; hash < used; ++hash){
    Identifier id;
    if (!mdtHelper->get_id(hash,id,&context)) {
      m_lookup[id]=(int) hash;
      m_int2id.push_back(id); 
    } else log << MSG::ERROR << "MDT hash constructor failed!" << endmsg;
  } 

  log << MSG::DEBUG << "Number of valid MDT Element IDs " << used << endmsg; 
  m_size = (int) used; 
}

MdtCsmIdHash::ID MdtCsmIdHash::identifier(int index) const {

  if(index>=0 && index < m_size) { 
   return m_int2id[index] ; 
  }
  // return blank id for invalid index 
  std::cout <<" MdtCsmIdHash::ERROR : identifier invalid ID "<<std::endl; 
  Identifier id;
  return id; 

}

int MdtCsmIdHash::operator() (const ID& id) const {

 std::map<ID,int>::const_iterator it = m_lookup.find(id); 
 if(it!=m_lookup.end()){
	return (*it).second; 
 }

 // FIXME  -1 for invalid ID?
 return -1; 

}

int MdtCsmIdHash::max() const {

return m_size; 

}

int MdtCsmIdHash::offset() const {

return 0;

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*

  Kin Yip : May 22, 2002

  barrel_ec in software description has changed the definition.
  
  Now, we have  +-1 for Barrel
                +-2,3 for EndCap

  "pos_neg" in the ntuple is equivalent to "barrel_ec" in software description.

  Hong Ma : Aug 30, 2002 
    Add testing of RoI. 
 */


//#include <strstream>

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "LArCabling/LArCablingService.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArEM_ID.h"

#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
//#include "LArROD/LArRawChannelBuilder.h"
#include "LArROD/ReadLArRaw.h"

// access all RawChannel  
//#include "EventContainers/SelectAllObject.h" 
// access RawChannel in TrigTower 
//#include "LArRawUtils/LArRoI_Map.h"
// #include "LArRawUtils/LArRawRoISelector.h"
//#include "LArRawUtils/LArTT_Selector.h"

#include "CaloIdentifier/CaloIdManager.h"

using namespace std ;

//const int max_chan = 2800 ;

/////////////////////////////////////////////////////////////////////////////

ReadLArRaw::ReadLArRaw(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_ChannelContainerName("LArRawChannelContainer"), 
  m_cablingService("LArCablingService"),
  m_onlineID(0),
  m_larem_id(0)
{
  // Declare the properties
  declareProperty("LArRawChannelContainerName",m_ChannelContainerName);
  declareProperty("DumpFile",m_dumpFile="");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadLArRaw::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endmsg;
  
  if (m_cablingService.retrieve().isFailure()) {
    log << MSG::FATAL
	<< " CablingService not found " 
	<< endmsg; 
    return StatusCode::FAILURE;
  }
  
  StoreGateSvc* detStore;
   if (service("DetectorStore", detStore).isFailure()) {
     log << MSG::ERROR
         << "Unable to access DetectoreStore" << endmsg; 
     return StatusCode::FAILURE;
   }

// retrieve OnlineID helper from detStore
   if (detStore->retrieve(m_onlineID, "LArOnlineID").isFailure()) {
      log << MSG::FATAL << "Could not get LArOnlineID helper !" << endmsg;
      return StatusCode::FAILURE;
   }

// retrieve helpers for identifier
  const DataHandle<CaloIdManager> caloIdMgr;
  if (detStore->retrieve(caloIdMgr).isFailure()) {
      log << MSG::FATAL << "Could not get CaloIdManager " << endmsg;
      return StatusCode::FAILURE;
  } 
  m_larem_id   = caloIdMgr->getEM_ID();



  if (m_dumpFile.size()) {
    m_outFile.open(m_dumpFile.c_str());
  }

  // after all these ...
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadLArRaw::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endmsg;
  const DataHandle<LArRawChannelContainer> LArRaw ;
  //  StatusCode sc_read = StoreGate::retrieve( LArRaw, string( "LArRawChannelContainer" ) ) ;
  StatusCode sc_read = StoreGate::instance().retrieve( LArRaw, m_ChannelContainerName ) ;

  if ( !sc_read.isSuccess() ) {
    log << MSG::FATAL << "Could not find event" << endmsg;
    return StatusCode::FAILURE;
  }


 int emax= 0; 
 HWIdentifier maxId;

 LArRawChannelContainer::const_iterator it1 = LArRaw->begin(); 
 LArRawChannelContainer::const_iterator it2 = LArRaw->end(); 
 for(; it1!=it2; ++it1){
   HWIdentifier ch_id=it1->identify();
   if (m_outFile.is_open()) {
     if(m_cablingService->isOnlineConnected(ch_id))
       m_outFile << "LArRawChannelDump: 0x" << std::hex << ch_id.get_compact() << std::dec << " E=" << it1->energy() 
		 << " t="  << it1->time() << " g=" << (int)it1->gain()  << " Q=" << it1->quality() << std::endl;
   }

   if(emax<it1->energy() ) { 
     emax=it1->energy() ; 
     maxId =ch_id;
     //  use LArOnlineID methods
     log << MSG::VERBOSE
	 << "--barrel_ec : " << m_onlineID->barrel_ec(ch_id) 
	 << " Pos_neg : " << m_onlineID->pos_neg(ch_id)
	 << " FeedThrough no. : " << m_onlineID->feedthrough(ch_id)
	 << " slot no. : " << m_onlineID->slot(ch_id)
	 << " channel no. : " << m_onlineID->channel(ch_id)
	 << endmsg;
     
     log << MSG::DEBUG << "Energy = " << it1->energy() << "; Time = " 
	 << it1->time()
	 << "; Chi-Square = " << it1->quality() << endmsg ;
   }     
   log <<MSG::DEBUG<< " Channel with max energy , maxID =  "<< emax<<" "<<maxId.getString() <<endmsg; 
 }

 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadLArRaw::finalize() {

  MsgStream log(msgSvc(), name());
  if (m_outFile.is_open()) 
    m_outFile.close();
  log << MSG::INFO << "in finalize()" << endmsg;

  return StatusCode::SUCCESS;
}



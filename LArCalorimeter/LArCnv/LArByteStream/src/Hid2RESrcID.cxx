/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/Hid2RESrcID.h" 
#include "CaloIdentifier/LArID_Exception.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <iostream> 

#include "eformat/SourceIdentifier.h" 

using eformat::helper::SourceIdentifier; 

// implementation of LArOnlineID to RESrcID conversion. 
// 
// FIXME:  need to check the convention for posNeg vs left-right. 
//   DET id assumes left-right
//   FEB_ID assumes pos-neg. 
//   I have assumed here, left  for pn=0  (negative?) 
//                        right for pn=1 (positive?) 

// using ByteStream::Nat8; 

Hid2RESrcID::Hid2RESrcID():
  m_initialized(false),
  m_cablingSvc(0),
  m_onlineHelper(0)
{  

} 

StatusCode Hid2RESrcID::initialize()
{  
  // Message service
  IMessageSvc*  msgSvc;
  StatusCode sc = Gaudi::svcLocator()->service( "MessageSvc", msgSvc  );
  MsgStream log(msgSvc, "Hid2RESrcID");

  // Cabling Service
  IToolSvc* toolSvc;
  sc   = Gaudi::svcLocator()->service("ToolSvc",toolSvc  );
  if(sc.isSuccess())
    {
      sc = toolSvc->retrieveTool("LArCablingLegacyService",m_cablingSvc);
      if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get LArCablingLegacyService !" << endmsg;
	exit(1);
      }
    } else {  // check if it fails
        // what do you want to do if it fails...
      log << MSG::FATAL << "Could not get ToolSvc !" << endmsg;
      exit(1);
    }

  //m_cablingSvc = LArCablingService::getInstance(); 
 
  // retrieve onlineHelper
  const LArOnlineID* online_id = 0;
  StoreGateSvc* detStore = 0;
  sc =Gaudi::svcLocator()->service( "DetectorStore", detStore );
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to locate DetectorStore" << endmsg;
    exit(1);
  } else {
    log << MSG::VERBOSE << "Successfully located DetectorStore" << endmsg;
  }     
  sc = detStore->retrieve(online_id, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get LArOnlineID helper !" << endmsg;
    exit(1);
  } 
  else {
    m_onlineHelper=online_id;
    log << MSG::VERBOSE << " Found the LArOnlineID helper. " << endmsg;
  }

  // make internal maps 
  
  eformat::SubDetector detid ;
  
  

  const std::vector<HWIdentifier>& roms = m_cablingSvc->getLArRoModIDvec();
  std::vector<HWIdentifier>::const_iterator it =  roms.begin(); 
  std::vector<HWIdentifier>::const_iterator it_end =  roms.end(); 

  for(; it!=it_end; ++it)
    { 
      HWIdentifier mId(*it); 
      detid = (eformat::SubDetector) m_readoutModuleSvc.subDet(mId); 
      uint8_t m = m_readoutModuleSvc.rodFragId(mId); 

      SourceIdentifier sid = SourceIdentifier(detid,m); 
      uint32_t rod_id =  sid.code(); 

      assert( m_coll2ROD.count(mId) == 0 );
      m_coll2ROD[ mId ]=rod_id;        

    } 


  m_initialized = true ; 

  return StatusCode::SUCCESS;
}


uint32_t  Hid2RESrcID::getRodIDFromROM(const COLLECTION_ID& id) const
{ // this method returns a RESrcID for the ROD, for a given COLLECTION_ID

  COLL_MAP::const_iterator it = m_coll2ROD.find( id ); 
  if(it == m_coll2ROD.end()){
    std::cout <<" H2d2RESrcID invalid COLL ID in hex "<<std::hex<<id.get_compact()<<std::dec<<std::endl;
    assert(0); 
  }	

  return  (*it).second ;
}


uint32_t  Hid2RESrcID::getRodID(const HWIdentifier& hid) const
{ // this method returns a RESrcID for the ROD, for a given LArOnlineID
  // channel number is ignored.
  HWIdentifier febId =  m_onlineHelper->feb_Id(hid) ;
  //LArFEB_ID febId = m_cablingSvc->createFEB_ID(hid); 
  HWIdentifier romId = m_cablingSvc->getReadoutModuleID(febId);
  
  return getRodIDFromROM(romId); 
}

/** mapping SrcID from ROD to ROB
 */ 
uint32_t Hid2RESrcID::getRobID( uint32_t rod_id) const
{
//  Change Module Type to ROB 

 SourceIdentifier  id  = SourceIdentifier(rod_id);
 SourceIdentifier  id2 = SourceIdentifier(id.subdetector_id(), id.module_id());
 return    id2.code();

}


/** mapping SrcID from ROB to ROS
 */ 
uint32_t Hid2RESrcID::getRosID( uint32_t rob_id) const
{
//  Change Module Type to ROS, moduleid = 0  

 SourceIdentifier  id  = SourceIdentifier(rob_id);
 SourceIdentifier  id2 = SourceIdentifier(id.subdetector_id(), 0);
 return    id2.code();

}

/** mapping SrcID from ROS to Det
 */ 
uint32_t Hid2RESrcID::getDetID  ( uint32_t ros_id) const
{
//  ROS to DET

 SourceIdentifier  id  = SourceIdentifier(ros_id);
 SourceIdentifier  id2 = SourceIdentifier(id.subdetector_id(), 0);
 return    id2.code();
}

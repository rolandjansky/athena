/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/Hid2RESrcID.h"
#include "LArRecConditions/LArFebRodMapping.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
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
  m_onlineHelper(0)
{  

} 

StatusCode Hid2RESrcID::initialize (const LArFebRodMapping& rodMapping)
{  
  ATH_CHECK( initialize (rodMapping.getLArRoModIDvec()) );
  return StatusCode::SUCCESS;
}


StatusCode Hid2RESrcID::initialize (const std::vector<HWIdentifier>& roms)
{  
  // retrieve onlineHelper
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "Hid2RESrcID");
  ATH_CHECK( detStore.retrieve() );
  ATH_CHECK( detStore->retrieve(m_onlineHelper, "LArOnlineID") );

  // make internal maps 
  
  eformat::SubDetector detid ;
  for (const HWIdentifier& mId : roms)
    { 
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
    std::abort();
  }	

  return  (*it).second ;
}


uint32_t  Hid2RESrcID::getRodID(const LArFebRodMapping& rodMapping,
                                const HWIdentifier& hid) const
{ // this method returns a RESrcID for the ROD, for a given LArOnlineID
  // channel number is ignored.
  HWIdentifier febId =  m_onlineHelper->feb_Id(hid) ;
  HWIdentifier romId = rodMapping.getReadoutModuleID(febId);
  
  return getRodIDFromROM(romId); 
}

/** mapping SrcID from ROD to ROB
 */ 
uint32_t Hid2RESrcID::getRobID( uint32_t rod_id) const
{
  return m_rodRobIdMap.getRobID (rod_id);
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

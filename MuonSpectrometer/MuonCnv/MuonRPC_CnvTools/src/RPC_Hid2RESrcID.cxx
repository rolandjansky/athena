/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_Hid2RESrcID.h" 

#include "eformat/SourceIdentifier.h" 

#include <stdexcept>
#include <cassert>

using eformat::helper::SourceIdentifier; 

// implementation of RpcPad to RESrcID conversion. 
// Ketevi A. Assamagan
// Jan-16-2003, BNL


RPC_Hid2RESrcID::RPC_Hid2RESrcID(): m_rpcIdHelper(nullptr), m_specialROBNumber(-1) {
}

RPC_Hid2RESrcID::RPC_Hid2RESrcID(int specialROBNumber) : m_rpcIdHelper(nullptr), m_specialROBNumber(specialROBNumber) { 
}

void RPC_Hid2RESrcID::set(const RpcIdHelper* rpdId) {
  m_rpcIdHelper=rpdId;
}

// This method is used by the RDO converter only
uint32_t RPC_Hid2RESrcID::getRodID(const Identifier& offlineId, const RpcCablingCondData* readCdo) const { 
  
  // this method returns a RESrcID for the ROD, for a 
  // given RPC pad ID offline ID
  
  unsigned short int RODid;
  unsigned short int Iside;
  unsigned short int SLid;
  
  if(readCdo->give_RODid(offlineId,RODid) && readCdo->give_side(offlineId,Iside) && readCdo->give_SLid(offlineId,SLid)) {
    
    uint8_t rodIndex = (uint8_t) RODid;
    uint16_t side    = Iside;
    
    eformat::SubDetector detid =  (side == eformat::MUON_RPC_BARREL_A_SIDE) ? 
      eformat::MUON_RPC_BARREL_A_SIDE
      : eformat::MUON_RPC_BARREL_C_SIDE ;
   
    
    SourceIdentifier sid (detid,rodIndex); 
    uint32_t rod_id = sid.code();
    
    // TEMP FIXME temp fix for the sector 13 pulse tests ---- added fix for M3 
    if ( (m_specialROBNumber>0) ) {
      if (side==0x65 && (SLid==23 || SLid == 24)) {
        //  rod_id=0x650001;
        rod_id = m_specialROBNumber;
      }
      else if (side==0x65 && SLid==7) {
        rod_id = 0x650001;
      }
      else if (side==0x65 && SLid==8) {
        rod_id = 0x650002;
        if(m_specialROBNumber == 0x650001)rod_id = 0x650001 ;//fix for M3 with runnumber up to 11533 (SFI)
      }
      else {
        rod_id = 0xffffffff;
      }
    }

    return rod_id; 
    
  }
  
  return 0xffffffff;
}


uint32_t RPC_Hid2RESrcID::getRodID(const int& side, const int& slogic, const int& padId, const RpcCablingCondData* readCdo) const {
  // this method returns a RESrcID for the ROD, for a given RPC pad ID
  const RpcCablingCondData::RDOmap& pad_map = readCdo->give_RDOs();
  int key = side*10000+slogic*100+padId;
  RDOindex index = (*pad_map.find(key)).second;
  uint8_t rodIndex = (uint8_t) index.RODid();

  eformat::SubDetector detid =  (side == 1) ? eformat::MUON_RPC_BARREL_A_SIDE
    : eformat::MUON_RPC_BARREL_C_SIDE ;

  SourceIdentifier sid(detid,rodIndex); 
  uint32_t rod_id =  sid.code();

  return rod_id; 
}

uint32_t RPC_Hid2RESrcID::getRodID(const int& sector) const { 
  // this method returns a RESrcID for the ROD, for a given RPC pad ID
  uint8_t rodIndex = (uint8_t) ((sector%32)/2);
  int side         = (sector < 32) ? 0:1;

  eformat::SubDetector detid = (side == 1) ? eformat::MUON_RPC_BARREL_A_SIDE
    : eformat::MUON_RPC_BARREL_C_SIDE ;

  SourceIdentifier sid(detid,rodIndex); 
  uint32_t rod_id = sid.code();

  return rod_id; 
}

uint32_t RPC_Hid2RESrcID::getRodID(const uint16_t& side, const uint16_t& rodIndex) const { 
  // this method returns a RESrcID for the ROD, for a given RPC pad ID
  eformat::SubDetector detid = (side == 1) ? eformat::MUON_RPC_BARREL_A_SIDE : eformat::MUON_RPC_BARREL_C_SIDE ;
  SourceIdentifier sid(detid,rodIndex); 
  uint32_t rod_id =  sid.code();
  return rod_id; 
}

/** mapping SrcID from ROD to ROB (called in ByteStreamCnvSvcBase/FullEventAssembler.icc)
 */
uint32_t RPC_Hid2RESrcID::getRobID(const uint32_t rod_id) const {
  return rod_id;
}

/** mapping SrcID from ROB to ROS
 */ 
uint32_t RPC_Hid2RESrcID::getRosID(const uint32_t rob_id) const {
//  Change Module Type to ROS, moduleid = 0  
  SourceIdentifier id (rob_id);
  SourceIdentifier id2(id.subdetector_id(),0);
  return id2.code(); 
}

/** mapping SrcID from ROS to Det
 */ 
uint32_t RPC_Hid2RESrcID::getDetID(const uint32_t ros_id) const {
//  ROS to DET
  SourceIdentifier id (ros_id);
  SourceIdentifier id2(id.subdetector_id(),0);
  return id2.code(); 
}

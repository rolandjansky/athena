/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_Hid2RESrcID.h" 

#include "eformat/SourceIdentifier.h" 

#include <stdexcept>
#include <cassert>

using eformat::helper::SourceIdentifier; 

// implementation of RpcPad to RESrcID conversion. 
// Ketevi A. Assamagan
// Jan-16-2003, BNL


RPC_Hid2RESrcID::RPC_Hid2RESrcID(): m_cabling(0), m_rpcHelper(0),m_specialROBNumber(-1)  { 
}

RPC_Hid2RESrcID::RPC_Hid2RESrcID(int specialROBNumber) : m_cabling(0), m_rpcHelper(0), m_specialROBNumber(specialROBNumber) { 
}

void RPC_Hid2RESrcID::set(const IRPCcablingSvc* p_cabling, const RpcIdHelper* rpcHelper) {
  m_cabling=p_cabling;
  m_rpcHelper=rpcHelper;
}

std::vector<uint32_t> RPC_Hid2RESrcID::getRodIDFromCollID(const Identifier& offlineId) { 

  // this method returns a vector of rod IDs , for a 
  // given RPC collection offline ID
  // An RpcDigitCollection can correspond to more than a single Rod Id

  std::vector<uint32_t> rodIDs;

  // Identifier elements
  int name         = 0;
  int eta          = 0;
  int phi          = 0;
  int doublet_r    = 0;
  int doublet_z    = 0;
  int doublet_phi  = 0;
  int gas_gap      = 0;
  int measures_phi = 0;
  int strip        = 0;

  // loop over side, sector logic and pad Id to find the correct pad!

  const CablingRPCBase::RDOmap& pad_map = m_cabling->give_RDOs();

  CablingRPCBase::RDOmap::const_iterator pad_beg = (pad_map).begin(); 
  CablingRPCBase::RDOmap::const_iterator pad_end = (pad_map).end();

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Looking for collection ID: " + std::string(m_rpcHelper->show_to_string(offlineId)) <<endmsg;

  int stationName = m_rpcHelper->stationName(offlineId);
  int stationEta  = m_rpcHelper->stationEta(offlineId);
  int stationPhi  = m_rpcHelper->stationPhi(offlineId);
  //int doubletR    = m_rpcHelper->doubletR(offlineId);
  bool addRod;


  for (; pad_beg!=pad_end; ++pad_beg) {

    RDOindex index = (*pad_beg).second;
    index.offline_indexes(name, eta, phi, doublet_r, 
      doublet_z, doublet_phi, gas_gap, 
      measures_phi, strip);

    if ( (stationEta == eta && stationPhi == phi) || 
    (stationName==10 && stationEta==0 && abs(eta)==1 && stationPhi == phi) ) {
      uint8_t rodIndex = (uint8_t) index.RODid();
      uint16_t side    = index.side();
      if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) <<"In getRODIdFromCollID .. The side is 0x" 
        << MSG::hex << side 
        << MSG::dec <<endmsg;

      eformat::SubDetector detid =  (side == eformat::MUON_RPC_BARREL_A_SIDE) ? 
        eformat::MUON_RPC_BARREL_A_SIDE
        : eformat::MUON_RPC_BARREL_C_SIDE ;

      SourceIdentifier sid(detid,rodIndex); 

      uint32_t rodId = sid.code();

      // fill it with RODid 
      addRod = true;
      std::vector<uint32_t>::const_iterator rod_it = rodIDs.begin();
      for ( ; rod_it != rodIDs.end() ; ++rod_it) 
      {
        if ( rodId == (*rod_it) ) 
          addRod = false;
      }

      if (addRod) {
        if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Found a RodId for this collection : 0x" 
          << MSG::hex
          << rodId 
          << MSG::dec <<endmsg;

        rodIDs.push_back(rodId); 
      }
    }

  }

  if (rodIDs.size() == 0) {
    msg(MSG::ERROR) 
      << "RPC_Hid2RESrcID: RPC_Could not find Rod ID for the collection "
      << m_rpcHelper->show_to_string(offlineId) << endmsg;
  }

  // FIXME Temp Fix for sector13 test data
  if ( (m_specialROBNumber>0) ) { 
    if (stationPhi==7 && stationEta>0 && stationEta<4 && (stationName==2 || stationName==4) ) {

      //std::cout << m_rpcHelper->show_to_string(offlineId) << std::endl;
      rodIDs.clear();
      rodIDs.push_back(m_specialROBNumber);

      if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Setting the special ROB Number: " 
        << MSG::hex 
        << m_specialROBNumber
        << MSG::dec 
        << " for this collection" <<endmsg;
    }
    else if ( stationPhi==3 && stationEta>0 && (stationName==2 || stationName==4) ) {
      rodIDs.clear();
      rodIDs.push_back(0x650001);
      rodIDs.push_back(0x650002);
    }
    else {
      rodIDs.clear();
      rodIDs.push_back(0xffffffff);
    }
  }


  return rodIDs;
}

// This method is used by the RDO converter only
uint32_t RPC_Hid2RESrcID::getRodID(const Identifier& offlineId) { 
  
  // this method returns a RESrcID for the ROD, for a 
  // given RPC pad ID offline ID
  
  unsigned short int ROBid;
  unsigned short int RODid;
  unsigned short int Iside;
  unsigned short int SLid;
  unsigned short int RXid;
  unsigned short int PADid;
  
  if( m_cabling->giveOnlineID(offlineId,ROBid,RODid,Iside,SLid,RXid,PADid) ) {
    
    uint8_t rodIndex = (uint8_t) RODid;
    uint16_t side    = Iside;
    
    if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "in getRodID, the side, sectorL, RX, pad are " 
        << MSG::hex 
        << side 
        << " " 
        << SLid 
        << " " 
        << RXid 
        << " " 
        << PADid 
        << MSG::dec <<endmsg;
    
    eformat::SubDetector detid =  (side == eformat::MUON_RPC_BARREL_A_SIDE) ? 
      eformat::MUON_RPC_BARREL_A_SIDE
      : eformat::MUON_RPC_BARREL_C_SIDE ;
   
    
    SourceIdentifier sid (detid,rodIndex); 
    uint32_t rod_id = sid.code();
    
    msg(MSG::DEBUG) << "Found RodID: " << MSG::hex 
        << rod_id << MSG::dec << endmsg;

    // TEMP FIXME temp fix for the sector 13 pulse tests ---- added fix for M3 
    if ( (m_specialROBNumber>0) ) {
      if (side==0x65 && (SLid==23 || SLid == 24)) {
        //  rod_id=0x650001;
        rod_id = m_specialROBNumber;
        if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Setting special ROD number"<<endmsg;
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
  
  if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "RPC_Hid2RESrcID: Could not find a pad with Id = "   
      << m_rpcHelper->show_to_string(offlineId)<<endmsg;
  return 0xffffffff;
}


uint32_t RPC_Hid2RESrcID::getRodID(const int& side, const int& slogic, 
           const int& padId) { 

  // this method returns a RESrcID for the ROD, for a given RPC pad ID

  const CablingRPCBase::RDOmap& pad_map = m_cabling->give_RDOs();
  int key = side*10000+slogic*100+padId;
  RDOindex index = (*pad_map.find(key)).second;
  uint8_t rodIndex = (uint8_t) index.RODid();

  eformat::SubDetector detid =  (side == 1) ? eformat::MUON_RPC_BARREL_A_SIDE
    : eformat::MUON_RPC_BARREL_C_SIDE ;

  SourceIdentifier sid(detid,rodIndex); 
  uint32_t rod_id =  sid.code();

  return rod_id; 
}

uint32_t RPC_Hid2RESrcID::getRodID(const int& sector) { 

  // this method returns a RESrcID for the ROD, for a given RPC pad ID

  uint8_t rodIndex = (uint8_t) ((sector%32)/2);
  int side         = (sector < 32) ? 0:1;

  eformat::SubDetector detid = (side == 1) ? eformat::MUON_RPC_BARREL_A_SIDE
    : eformat::MUON_RPC_BARREL_C_SIDE ;

  SourceIdentifier sid(detid,rodIndex); 
  uint32_t rod_id = sid.code();

  return rod_id; 
}

uint32_t RPC_Hid2RESrcID::getRodID(const uint16_t& side, const uint16_t& rodIndex) { 

  // this method returns a RESrcID for the ROD, for a given RPC pad ID

  eformat::SubDetector detid =  (side == 1) ? eformat::MUON_RPC_BARREL_A_SIDE
    : eformat::MUON_RPC_BARREL_C_SIDE ;

  SourceIdentifier sid(detid,rodIndex); 
  uint32_t rod_id =  sid.code();

  return rod_id; 
}

/** mapping SrcID from ROD to ROB
 */
uint32_t RPC_Hid2RESrcID::getRobID( uint32_t rod_id) {

  return rod_id;
  //return 1;
}

/** mapping SrcID from ROB to ROS
 */ 
uint32_t RPC_Hid2RESrcID::getRosID( uint32_t rob_id) {
//  Change Module Type to ROS, moduleid = 0  
  SourceIdentifier id (rob_id);
  SourceIdentifier id2(id.subdetector_id(),0);
  return id2.code(); 

}

/** mapping SrcID from ROS to Det
 */ 
uint32_t RPC_Hid2RESrcID::getDetID  ( uint32_t ros_id) {
//  ROS to DET

  SourceIdentifier id (ros_id);
  SourceIdentifier id2(id.subdetector_id(),0);
  return id2.code(); 
}

uint16_t RPC_Hid2RESrcID::findRODid(uint16_t side, uint16_t slogic, 
            uint16_t padId) {
  const CablingRPCBase::RDOmap& pad_map = m_cabling->give_RDOs();
  int key = side*10000 + slogic*100 + padId;
  RDOindex index = (*pad_map.find(key)).second;
  uint16_t rodId = (uint16_t) index.RODid();
  return rodId;
}


Identifier RPC_Hid2RESrcID::findPadOfflineId(uint16_t side, uint16_t slogic, 
               uint16_t padId) {

  // Identifier elements
  int name         = 0;
  int eta          = 0;
  int phi          = 0;
  int doublet_r    = 0;
  int doublet_z    = 0;
  int doublet_phi  = 0;
  int gas_gap      = 0;
  int measures_phi = 0;
  int strip        = 0;

  const CablingRPCBase::RDOmap& pad_map = m_cabling->give_RDOs();
  int key = side*10000 + slogic*100 + padId;
  RDOindex index = (*pad_map.find(key)).second;
  index.offline_indexes(name, eta, phi, doublet_r, 
      doublet_z, doublet_phi, gas_gap, 
      measures_phi, strip);

  // Build the identifier
  Identifier id = m_rpcHelper->padID(name, eta, phi, doublet_r, 
             doublet_z, doublet_phi);


  return id;

}



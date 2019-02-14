/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_HID2RESRCID_H
#define LARBYTESTREAM_HID2RESRCID_H

// #include "ByteStreamData/RESrcID.h" 
#include "LArIdentifier/LArOnlineID.h" 
#include "LArCabling/LArCablingLegacyService.h" 


#include <stdint.h> 
#include <map>

#include "LArIdentifier/LArReadoutModuleService.h"



// this class provides conversion between HWIdentifier and RESrcID
// HWIdentifier is used in LArRawChannel to identify each channel 
// RESrcID is used for identifying each ROD. 
// 
// It also provides conversion from ROC RESrcID to a LArFEB_ID. 
//  LArFEB_ID is currently used as ID for LArRawChannelCollection. 
//  each Collection corresponds to a ROC. 
//

// Revision: July 11, 2002
//  Modified for the new RE source ID, to be used with eformat package.

// Revision: Aug 11, 2006
//  move code from constructor to initialize() method.
//  only use the object after initialize() is called.

class Hid2RESrcID {

public:

  typedef HWIdentifier COLLECTION_ID; 

  /** constrcutor 
  */ 
  Hid2RESrcID(); 

  StatusCode initialize(); 

  bool isInitialized(){ return m_initialized;} 

  /** make a ROD SrcID for a HWIdentifier 
  */ 
  uint32_t getRodID  (const HWIdentifier& hid); 

  /** make a ROD SrcID for a COLLECTION_ID
  */ 
  uint32_t getRodIDFromROM  (const COLLECTION_ID& hid); 

  /** Make a ROB Source ID from a ROD source ID
  */ 
  uint32_t getRobID  ( uint32_t rod_id); 

  /** Make a ROS Source ID from a ROB source ID
  */ 
  uint32_t getRosID  ( uint32_t rob_id); 

  /** Make a SubDetector ID from ROS source ID 
  */
  uint32_t getDetID  ( uint32_t ros_id); 

private: 
  bool m_initialized;
  LArCablingLegacyService* m_cablingSvc; 
  const LArOnlineID* m_onlineHelper; 
  typedef std::map<HWIdentifier, uint32_t> COLL_MAP ; 
  COLL_MAP m_coll2ROD ; 

  LArReadoutModuleService m_readoutModuleSvc;

};

#endif 

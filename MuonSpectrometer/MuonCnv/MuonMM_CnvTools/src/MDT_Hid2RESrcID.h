/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MDT_HID2RESRCID__
#define __MDT_HID2RESRCID__

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "Identifier/Identifier.h"


#include <stdint.h> 
#include <map>

// this class provides conversion between Rpc pad Id and RESrcID 
// RESrcID is used for identifying each ROD. 
// Ketevi A. Assamagan
// Jan-16-2003, BNL
// ------------------------- 
// Adapted for MDT 
// Michela Biglietti 
// March 2003, Naples

class MDT_Hid2RESrcID {

public:

  /** constrcutor 
   */ 
  MDT_Hid2RESrcID (); 

  /** intialize the cabling service
   */
  void set(MuonMDT_CablingSvc* p_cabling, const MdtIdHelper* m_mdtIdHelper); 

  /** TMP method for sector 13 data */
  void setSpecialROBNumber(int specialROBNumber) {m_specialROBNumber=specialROBNumber;}
  /**
   */
  uint32_t getRodID(const Identifier& offlineId);

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

  MuonMDT_CablingSvc* m_cabling;
  const MdtIdHelper* m_mdtIdHelper;

  int m_specialROBNumber;

};

#endif 



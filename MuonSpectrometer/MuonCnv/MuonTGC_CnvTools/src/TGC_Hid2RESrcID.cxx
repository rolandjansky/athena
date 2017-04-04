/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_Hid2RESrcID.h" 

#include "MuonRDO/TgcRdo.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

#include "MuonRDO/TgcRdoIdHash.h"

#include "eformat/SourceIdentifier.h" 
using eformat::helper::SourceIdentifier; 

// Initialize the cabling service. 
void Muon::TGC_Hid2RESrcID::set(const ITGCcablingSvc* p_cabling) {
  m_cabling=p_cabling;
  m_robIDs.clear();
  this->fillAllRobIds();
}

// get source ID for a RDO
uint32_t Muon::TGC_Hid2RESrcID::getRodID(const TgcRdo *rdo)
{
  // get Source ID
  SourceIdentifier sid(static_cast<eformat::SubDetector>(rdo->subDetectorId()),
			static_cast<uint16_t>(rdo->rodId()));

  // return Source ID
  return sid.code();
}

// get source ID for a RDO 
uint32_t Muon::TGC_Hid2RESrcID::getRodID(uint16_t subDetectorId, uint16_t rodId)
{
  // get Source ID
  SourceIdentifier sid(static_cast<eformat::SubDetector>(subDetectorId), rodId);

  // return Source ID
  return sid.code();
}

// get source ID for an TgcDigitCollection 
uint32_t Muon::TGC_Hid2RESrcID::getRodID(const Identifier& offlineId)
{
  int subDetectorId;
  int rodId;
  m_cabling->getReadoutIDfromElementID(offlineId,subDetectorId,rodId);

  // get Source ID
  SourceIdentifier sid(static_cast<eformat::SubDetector>(subDetectorId),
			static_cast<uint16_t>(rodId));

  // return Source ID
  return sid.code();
}

// mapping SrcID from ROD to ROB 
uint32_t Muon::TGC_Hid2RESrcID::getRobID(uint32_t rod_id) 
{
  // return Source ID. 
  // ROD source ID and ROB source ID are identical for TGC. 
  return rod_id;
}

// mapping SrcID from ROB to ROS  
uint32_t Muon::TGC_Hid2RESrcID::getRosID(uint32_t rob_id) 
{
  //  Change Module Type to ROS, moduleid = 0  
  SourceIdentifier id(rob_id);
  SourceIdentifier id2(id.subdetector_id(),0);

  // return Source ID
  return id2.code(); 
}

// mapping SrcID from ROS to Det  
uint32_t Muon::TGC_Hid2RESrcID::getDetID(uint32_t ros_id) 
{
  //  ROS to DET
  SourceIdentifier id(ros_id);
  SourceIdentifier id2(id.subdetector_id(),0);

  // return Source ID
  return id2.code(); 
}

// fill all the ROB IDs 
void Muon::TGC_Hid2RESrcID::fillAllRobIds() 
{
  TgcRdoIdHash rdoIdHash;
  int rdoIdHash_max = rdoIdHash.max();
  for(int i=0; i<rdoIdHash_max; i++) {
    SourceIdentifier sid(static_cast<eformat::SubDetector>(rdoIdHash.subDetectorId(i)), 
			 rdoIdHash.rodId(i));
    
    m_robIDs.push_back(getRobID(sid.simple_code()));
  }
}

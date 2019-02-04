/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CSC_Hid2RESrcID.h" 
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "eformat/SourceIdentifier.h" 

using eformat::helper::SourceIdentifier; 

#include <cassert>

/** get source ID for a RDO */
uint32_t CSC_Hid2RESrcID::getRodID(const CscRawDataCollection *rdo) const
{

  uint16_t rodId = rdo->rodId();
  uint16_t subDetectorId = rdo->subDetectorId();

  return this->getRodID(subDetectorId, rodId);
}

/** get source ID for a RDO */
uint32_t CSC_Hid2RESrcID::getRodID(uint16_t subDetectorId, uint16_t rodId) const
{
  // get Source ID
  assert ( subDetectorId == eformat::MUON_CSC_ENDCAP_C_SIDE || 
	   subDetectorId == eformat::MUON_CSC_ENDCAP_A_SIDE );

  assert ( m_cabling->is_rodId(rodId) );

  SourceIdentifier sid (static_cast<eformat::SubDetector>(subDetectorId),
			static_cast<uint16_t>(rodId));
  return sid.code();
}

/** get source ID for a CscRdoCollection */
uint32_t CSC_Hid2RESrcID::getRodID(const Identifier& offlineId) const
{

  int eta                = m_cscIdHelper->stationEta(offlineId);
  int phi                = m_cscIdHelper->stationPhi(offlineId);
  uint16_t subDetectorId = (eta == -1) ? eformat::MUON_CSC_ENDCAP_C_SIDE : eformat::MUON_CSC_ENDCAP_A_SIDE ;
  uint16_t rodId         = uint16_t (phi-1);
  //if(offlineId >= 0x10 && offlineId <= 0x1F) rodId = 0x10 + 
  uint16_t onlineRodId   = 0x0;
  bool check = m_cabling->onlineId(rodId, onlineRodId);
  assert ( check );
  if ( !check ) onlineRodId = 0xFFFF;
  return this->getRodID(subDetectorId, onlineRodId);
}

/** mapping SrcID from ROD to ROB */ 
uint32_t CSC_Hid2RESrcID::getRobID (uint32_t rod_id) const
{

  uint32_t rob_id = rod_id;

  if ( m_isCosmic && m_isOldCosmic ) {
    if ( rod_id == 0x0069000a ) rob_id = 0x0;
    else if ( rod_id == 0x0069000c ) rob_id = 0x1;
  }

  return rob_id;

}

/** fill all the ROB IDs */
void CSC_Hid2RESrcID::fillAllRobIds()
{
  uint16_t sub_detector[] = { eformat::MUON_CSC_ENDCAP_C_SIDE, eformat::MUON_CSC_ENDCAP_A_SIDE };

  m_robIDs.clear();

  for (uint16_t i=0; i<m_cabling->nSide(); ++i) {
    uint16_t rodId = (m_cabling->nROD() == 8) ? 0 : 16; 
    //for (uint16_t rodId=0; rodId<m_cabling->nROD(); ++rodId) {
    for (unsigned int j=0; j<m_cabling->nROD(); ++j) {
       uint16_t onlineRodId   = 0x0;
       bool check = m_cabling->onlineId(rodId, onlineRodId);
       assert ( check );
       if ( !check ) onlineRodId = 0xFFFF;
       uint32_t theROD = this->getRodID(sub_detector[i], onlineRodId);

       if ( m_isCosmic ) {
           int jj= i*8+rodId;
           if ( jj==8 )      theROD = 0x0069000a;
           else if ( jj==9 ) theROD = 0x0069000c;
       }

       uint32_t theROB = this->getRobID( theROD );

       m_robIDs.push_back( theROB );
       ++rodId;
    }
  }
}

/** mapping SrcID from ROB to ROS */ 
uint32_t CSC_Hid2RESrcID::getRosID (uint32_t rob_id)  const
{
  //  Change Module Type to ROS, moduleid = 0  
  SourceIdentifier id (rob_id);
  SourceIdentifier id2(id.subdetector_id(),0);

  // return Source ID
  return id2.code(); 
}

/** mapping SrcID from ROS to Det */ 
uint32_t CSC_Hid2RESrcID::getDetID (uint32_t ros_id)  const
{
  //  ROS to DET
  SourceIdentifier id (ros_id);
  SourceIdentifier id2 (id.subdetector_id(),0);

  // return Source ID
  return id2.code(); 
}


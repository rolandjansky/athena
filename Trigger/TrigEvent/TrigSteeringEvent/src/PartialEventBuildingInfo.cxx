/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"


PartialEventBuildingInfo::PartialEventBuildingInfo() {
}
PartialEventBuildingInfo::~PartialEventBuildingInfo() {
}

void PartialEventBuildingInfo::add( uint32_t rob_id ) {
  addROB(rob_id);
}

void PartialEventBuildingInfo::addROB( uint32_t rob_id ) {
  m_robs.insert(rob_id);
}
void PartialEventBuildingInfo::add( const std::vector<uint32_t>& rob_ids ) {
  addROBs(rob_ids);
}

void PartialEventBuildingInfo::addROBs( const std::vector<uint32_t>& rob_ids ) {

  for ( std::vector<uint32_t>::const_iterator i = rob_ids.begin(); i != rob_ids.end(); ++i )
    m_robs.insert(*i);
}

void PartialEventBuildingInfo::get( std::vector<uint32_t>& rob_ids ) const {
  getROBs(rob_ids);
}

void PartialEventBuildingInfo::getROBs( std::vector<uint32_t>& rob_ids ) const {

  rob_ids.reserve(rob_ids.size()+m_robs.size()); // optimization, it is believed that m_robs can be big

  for ( std::set<uint32_t>::const_iterator i = m_robs.begin(); i != m_robs.end(); ++i ) 
    rob_ids.push_back(*i);
}

const std::set<uint32_t>& PartialEventBuildingInfo::getROBs() const {
  return m_robs;
}


void PartialEventBuildingInfo::addSubDetector( eformat::SubDetector id ) {
  m_subDetectors.insert(id);
}

void PartialEventBuildingInfo::addSubDetector( std::vector<eformat::SubDetector> ids ) {
  for ( std::vector<eformat::SubDetector>::const_iterator i = ids.begin(); i != ids.end(); ++i )
    m_subDetectors.insert(*i);
}


void PartialEventBuildingInfo::addSubDetector( eformat::SubDetectorGroup id ) {
  using namespace eformat;
  switch(id) {
    case PIXEL:  
    m_subDetectors.insert(PIXEL_BARREL);
    m_subDetectors.insert(PIXEL_DISK);
    m_subDetectors.insert(PIXEL_B_LAYER);    
    break;
   
    case SCT:
    m_subDetectors.insert(SCT_BARREL_A_SIDE);
    m_subDetectors.insert(SCT_BARREL_C_SIDE);
    m_subDetectors.insert(SCT_ENDCAP_A_SIDE);
    m_subDetectors.insert(SCT_ENDCAP_C_SIDE);    
    break;
    
  case TRT:    
    m_subDetectors.insert(TRT_ANCILLARY_CRATE);
    m_subDetectors.insert(TRT_BARREL_A_SIDE);
    m_subDetectors.insert(TRT_BARREL_C_SIDE);
    m_subDetectors.insert(TRT_ENDCAP_A_SIDE);
    m_subDetectors.insert(TRT_ENDCAP_C_SIDE);
    break;

  case LAR: 
    m_subDetectors.insert(LAR_EM_BARREL_A_SIDE); 
    m_subDetectors.insert(LAR_EM_BARREL_C_SIDE);
    m_subDetectors.insert(LAR_EM_ENDCAP_A_SIDE);
    m_subDetectors.insert(LAR_EM_ENDCAP_C_SIDE);
    m_subDetectors.insert(LAR_HAD_ENDCAP_A_SIDE);
    m_subDetectors.insert(LAR_HAD_ENDCAP_C_SIDE);
    m_subDetectors.insert(LAR_FCAL_A_SIDE);
    m_subDetectors.insert(LAR_FCAL_C_SIDE);
    break;

  case TILECAL: 
    m_subDetectors.insert(TILECAL_LASER_CRATE);
    m_subDetectors.insert(TILECAL_BARREL_A_SIDE);
    m_subDetectors.insert(TILECAL_BARREL_C_SIDE);
    m_subDetectors.insert(TILECAL_EXT_A_SIDE); 
    m_subDetectors.insert(TILECAL_EXT_C_SIDE);
    break;

  case MUON: 
    m_subDetectors.insert(MUON_ANCILLARY_CRATE);
    m_subDetectors.insert(MUON_MDT_BARREL_A_SIDE);
    m_subDetectors.insert(MUON_MDT_BARREL_C_SIDE);
    m_subDetectors.insert(MUON_MDT_ENDCAP_A_SIDE);
    m_subDetectors.insert(MUON_MDT_ENDCAP_C_SIDE);
    m_subDetectors.insert(MUON_RPC_BARREL_A_SIDE);
    m_subDetectors.insert(MUON_RPC_BARREL_C_SIDE);
    m_subDetectors.insert(MUON_TGC_ENDCAP_A_SIDE);
    m_subDetectors.insert(MUON_TGC_ENDCAP_C_SIDE);
    m_subDetectors.insert(MUON_CSC_ENDCAP_A_SIDE);
    m_subDetectors.insert(MUON_CSC_ENDCAP_C_SIDE);
    break;

  case TDAQ:
    m_subDetectors.insert(TDAQ_BEAM_CRATE);
    m_subDetectors.insert(TDAQ_CALO_PREPROC);
    m_subDetectors.insert(TDAQ_CALO_CLUSTER_PROC_DAQ);
    m_subDetectors.insert(TDAQ_CALO_CLUSTER_PROC_ROI);
    m_subDetectors.insert(TDAQ_CALO_JET_PROC_DAQ);
    m_subDetectors.insert(TDAQ_CALO_JET_PROC_ROI);
    m_subDetectors.insert(TDAQ_MUON_CTP_INTERFACE);
    m_subDetectors.insert(TDAQ_CTP); 
    m_subDetectors.insert(TDAQ_L2SV);
    m_subDetectors.insert(TDAQ_SFI);
    m_subDetectors.insert(TDAQ_SFO);
    m_subDetectors.insert(TDAQ_LVL2);
    m_subDetectors.insert(TDAQ_EVENT_FILTER);
    break;

  case FORWARD:
    m_subDetectors.insert(FORWARD_BCM);
    m_subDetectors.insert(FORWARD_LUCID);
    m_subDetectors.insert(FORWARD_ZDC);
    m_subDetectors.insert(FORWARD_ALPHA);
    break;

  default:
    // do nothing
    break;
  }

}

void PartialEventBuildingInfo::getSubDetectors( std::vector<eformat::SubDetector>& ids ) const {
  for ( std::set<uint32_t>::const_iterator i = m_subDetectors.begin(); i != m_subDetectors.end(); ++i ) 
    ids.push_back( eformat::helper::SourceIdentifier(*i).subdetector_id() ); 
}

const std::set<uint32_t>&  PartialEventBuildingInfo::getSubDetectors(  ) const {
  return m_subDetectors;
}

void PartialEventBuildingInfo::clear() {
  m_robs.clear();
  m_subDetectors.clear();
}

void PartialEventBuildingInfo::merge(const PartialEventBuildingInfo& peb) {
  m_robs.insert(peb.m_robs.begin(), peb.m_robs.end());
  m_subDetectors.insert(peb.m_subDetectors.begin(), peb.m_subDetectors.end());
}
#endif //XAOD_ANALYSIS

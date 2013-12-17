/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostExec/RobService.h"
#include "eformat/SourceIdentifier.h"

using namespace std;

//------------------------------------------------------------------------------------------
// return static instance
//------------------------------------------------------------------------------------------
Anp::RobService& Anp::RobService::Instance()
{
  static Anp::RobService rob;
  return rob;
}

namespace Anp
{
  //
  // Due to bugs saving ROB id information put copy of SourceIdentfier functions here
  //
  enum SubDetectorGroup {
    ANY_DETECTOR = 0x0,
    PIXEL = 0x1,
    SCT = 0x2,
    TRT = 0x3,
    LAR = 0x4,
    TILECAL = 0x5,
    MUON = 0x6,
    TDAQ = 0x7,
    FORWARD = 0x8,
    OTHER_DETECTORS = 0xf
  };

  enum SubDetector { 
    FULL_SD_EVENT = 0x00,
    PIXEL_BARREL = 0x11,
    PIXEL_DISK = 0x12,
    PIXEL_B_LAYER = 0x13,
    SCT_BARREL_A_SIDE = 0x21,
    SCT_BARREL_C_SIDE = 0x22,
    SCT_ENDCAP_A_SIDE = 0x23,
    SCT_ENDCAP_C_SIDE = 0x24,
    TRT_ANCILLARY_CRATE = 0x30,
    TRT_BARREL_A_SIDE = 0x31,
    TRT_BARREL_C_SIDE = 0x32,
    TRT_ENDCAP_A_SIDE = 0x33,
    TRT_ENDCAP_C_SIDE = 0x34,
    LAR_EM_BARREL_A_SIDE = 0x41,
    LAR_EM_BARREL_C_SIDE = 0x42,
    LAR_EM_ENDCAP_A_SIDE = 0x43,
    LAR_EM_ENDCAP_C_SIDE = 0x44,
    LAR_HAD_ENDCAP_A_SIDE = 0x45,
    LAR_HAD_ENDCAP_C_SIDE = 0x46,
    LAR_FCAL_A_SIDE = 0x47,
    LAR_FCAL_C_SIDE = 0x48,
    TILECAL_LASER_CRATE = 0x50,
    TILECAL_BARREL_A_SIDE = 0x51,
    TILECAL_BARREL_C_SIDE = 0x52,
    TILECAL_EXT_A_SIDE = 0x53,
    TILECAL_EXT_C_SIDE = 0x54,
    MUON_ANCILLARY_CRATE = 0x60,
    MUON_MDT_BARREL_A_SIDE = 0x61,
    MUON_MDT_BARREL_C_SIDE = 0x62,
    MUON_MDT_ENDCAP_A_SIDE = 0x63,
    MUON_MDT_ENDCAP_C_SIDE = 0x64,
    MUON_RPC_BARREL_A_SIDE = 0x65,
    MUON_RPC_BARREL_C_SIDE = 0x66,
    MUON_TGC_ENDCAP_A_SIDE = 0x67,
    MUON_TGC_ENDCAP_C_SIDE = 0x68,
    MUON_CSC_ENDCAP_A_SIDE = 0x69,
    MUON_CSC_ENDCAP_C_SIDE = 0x6a,
    TDAQ_BEAM_CRATE = 0x70,
    TDAQ_CALO_PREPROC = 0x71,
    TDAQ_CALO_CLUSTER_PROC_DAQ = 0x72,
    TDAQ_CALO_CLUSTER_PROC_ROI = 0x73,
    TDAQ_CALO_JET_PROC_DAQ = 0x74,
    TDAQ_CALO_JET_PROC_ROI = 0x75,
    TDAQ_MUON_CTP_INTERFACE = 0x76,
    TDAQ_CTP = 0x77,
    TDAQ_L2SV = 0x78,
    TDAQ_SFI = 0x79,
    TDAQ_SFO = 0x7a,
    TDAQ_LVL2 = 0x7b,
    TDAQ_EVENT_FILTER = 0x7c,
    TDAQ_MET_LAR = 0x7d,
    TDAQ_MET_TIL = 0x7e,
    FORWARD_BCM = 0x81,
    FORWARD_LUCID = 0x82,
    FORWARD_ZDC = 0x83,
    FORWARD_ALPHA = 0x84,
    OTHER = 0xff
  };
  
  std::vector<SubDetectorGroup> all_subdet_groups() 
  {
    std::vector<SubDetectorGroup> groups;
    groups.push_back(ANY_DETECTOR);
    groups.push_back(PIXEL);
    groups.push_back(SCT);
    groups.push_back(TRT);
    groups.push_back(LAR);
    groups.push_back(TILECAL);
    groups.push_back(MUON);
    groups.push_back(TDAQ);
    groups.push_back(FORWARD);
    
    return groups;
  }

  std::vector<SubDetector> all_subdets() 
  {
  std::vector<SubDetector> subdets;
  subdets.push_back(FULL_SD_EVENT);
  subdets.push_back(PIXEL_BARREL);
  subdets.push_back(PIXEL_DISK);
  subdets.push_back(PIXEL_B_LAYER);
  subdets.push_back(SCT_BARREL_A_SIDE);
  subdets.push_back(SCT_BARREL_C_SIDE);
  subdets.push_back(SCT_ENDCAP_A_SIDE);
  subdets.push_back(SCT_ENDCAP_C_SIDE);
  subdets.push_back(TRT_ANCILLARY_CRATE);
  subdets.push_back(TRT_BARREL_A_SIDE);
  subdets.push_back(TRT_BARREL_C_SIDE);
  subdets.push_back(TRT_ENDCAP_A_SIDE);
  subdets.push_back(TRT_ENDCAP_C_SIDE);
  subdets.push_back(LAR_EM_BARREL_A_SIDE);
  subdets.push_back(LAR_EM_BARREL_C_SIDE);
  subdets.push_back(LAR_EM_ENDCAP_A_SIDE);
  subdets.push_back(LAR_EM_ENDCAP_C_SIDE);
  subdets.push_back(LAR_HAD_ENDCAP_A_SIDE);
  subdets.push_back(LAR_HAD_ENDCAP_C_SIDE);
  subdets.push_back(LAR_FCAL_A_SIDE);
  subdets.push_back(LAR_FCAL_C_SIDE);
  subdets.push_back(TILECAL_LASER_CRATE);
  subdets.push_back(TILECAL_BARREL_A_SIDE);
  subdets.push_back(TILECAL_BARREL_C_SIDE);
  subdets.push_back(TILECAL_EXT_A_SIDE);
  subdets.push_back(TILECAL_EXT_C_SIDE);
  subdets.push_back(MUON_ANCILLARY_CRATE);
  subdets.push_back(MUON_MDT_BARREL_A_SIDE);
  subdets.push_back(MUON_MDT_BARREL_C_SIDE);
  subdets.push_back(MUON_MDT_ENDCAP_A_SIDE);
  subdets.push_back(MUON_MDT_ENDCAP_C_SIDE);
  subdets.push_back(MUON_RPC_BARREL_A_SIDE);
  subdets.push_back(MUON_RPC_BARREL_C_SIDE);
  subdets.push_back(MUON_TGC_ENDCAP_A_SIDE);
  subdets.push_back(MUON_TGC_ENDCAP_C_SIDE);
  subdets.push_back(MUON_CSC_ENDCAP_A_SIDE);
  subdets.push_back(MUON_CSC_ENDCAP_C_SIDE);
  subdets.push_back(TDAQ_BEAM_CRATE);
  subdets.push_back(TDAQ_CALO_PREPROC);
  subdets.push_back(TDAQ_CALO_CLUSTER_PROC_DAQ);
  subdets.push_back(TDAQ_CALO_CLUSTER_PROC_ROI);
  subdets.push_back(TDAQ_CALO_JET_PROC_DAQ);
  subdets.push_back(TDAQ_CALO_JET_PROC_ROI);
  subdets.push_back(TDAQ_MUON_CTP_INTERFACE);
  subdets.push_back(TDAQ_CTP);
  subdets.push_back(TDAQ_L2SV);
  subdets.push_back(TDAQ_SFI);
  subdets.push_back(TDAQ_SFO);
  subdets.push_back(TDAQ_LVL2);
  subdets.push_back(TDAQ_EVENT_FILTER);
  subdets.push_back(TDAQ_MET_LAR);
  subdets.push_back(TDAQ_MET_TIL);
  subdets.push_back(FORWARD_BCM);
  subdets.push_back(FORWARD_LUCID);
  subdets.push_back(FORWARD_ZDC);
  subdets.push_back(FORWARD_ALPHA);
  return subdets;
  }
  
  std::string human_detector(SubDetector sdet)
  {
    switch (sdet) {
    case FULL_SD_EVENT:
      return "FULL_SD_EVENT";
    case PIXEL_BARREL:
      return "PIXEL_BARREL";
    case PIXEL_DISK:
      return "PIXEL_DISK";
    case PIXEL_B_LAYER:
      return "PIXEL_B_LAYER";
    case SCT_BARREL_A_SIDE:
      return "SCT_BARREL_A_SIDE";
    case SCT_BARREL_C_SIDE:
      return "SCT_BARREL_C_SIDE";
    case SCT_ENDCAP_A_SIDE:
      return "SCT_ENDCAP_A_SIDE";
    case SCT_ENDCAP_C_SIDE:
      return "SCT_ENDCAP_C_SIDE";
    case TRT_ANCILLARY_CRATE:
      return "TRT_ANCILLARY_CRATE";
    case TRT_BARREL_A_SIDE:
      return "TRT_BARREL_A_SIDE";
    case TRT_BARREL_C_SIDE:
      return "TRT_BARREL_C_SIDE";
    case TRT_ENDCAP_A_SIDE:
      return "TRT_ENDCAP_A_SIDE";
    case TRT_ENDCAP_C_SIDE:
      return "TRT_ENDCAP_C_SIDE";
    case LAR_EM_BARREL_A_SIDE:
      return "LAR_EM_BARREL_A_SIDE";
    case LAR_EM_BARREL_C_SIDE:
      return "LAR_EM_BARREL_C_SIDE";
    case LAR_EM_ENDCAP_A_SIDE:
      return "LAR_EM_ENDCAP_A_SIDE";
    case LAR_EM_ENDCAP_C_SIDE:
      return "LAR_EM_ENDCAP_C_SIDE";
    case LAR_HAD_ENDCAP_A_SIDE:
      return "LAR_HAD_ENDCAP_A_SIDE";
    case LAR_HAD_ENDCAP_C_SIDE:
      return "LAR_HAD_ENDCAP_C_SIDE";
    case LAR_FCAL_A_SIDE:
      return "LAR_FCAL_A_SIDE";
    case LAR_FCAL_C_SIDE:
      return "LAR_FCAL_C_SIDE";
    case TILECAL_LASER_CRATE:
      return "TILECAL_LASER_CRATE";
    case TILECAL_BARREL_A_SIDE:
      return "TILECAL_BARREL_A_SIDE";
    case TILECAL_BARREL_C_SIDE:
      return "TILECAL_BARREL_C_SIDE";
    case TILECAL_EXT_A_SIDE:
      return "TILECAL_EXT_A_SIDE";
    case TILECAL_EXT_C_SIDE:
      return "TILECAL_EXT_C_SIDE";
    case MUON_ANCILLARY_CRATE:
      return "MUON_ANCILLARY_CRATE";
    case MUON_MDT_BARREL_A_SIDE:
      return "MUON_MDT_BARREL_A_SIDE";
    case MUON_MDT_BARREL_C_SIDE:
      return "MUON_MDT_BARREL_C_SIDE";
    case MUON_MDT_ENDCAP_A_SIDE:
      return "MUON_MDT_ENDCAP_A_SIDE";
    case MUON_MDT_ENDCAP_C_SIDE:
      return "MUON_MDT_ENDCAP_C_SIDE";
    case MUON_RPC_BARREL_A_SIDE:
      return "MUON_RPC_BARREL_A_SIDE";
    case MUON_RPC_BARREL_C_SIDE:
      return "MUON_RPC_BARREL_C_SIDE";
    case MUON_TGC_ENDCAP_A_SIDE:
      return "MUON_TGC_ENDCAP_A_SIDE";
    case MUON_TGC_ENDCAP_C_SIDE:
      return "MUON_TGC_ENDCAP_C_SIDE";
    case MUON_CSC_ENDCAP_A_SIDE:
      return "MUON_CSC_ENDCAP_A_SIDE";
    case MUON_CSC_ENDCAP_C_SIDE:
      return "MUON_CSC_ENDCAP_C_SIDE";
    case TDAQ_BEAM_CRATE:
      return "TDAQ_BEAM_CRATE";
    case TDAQ_CALO_PREPROC:
      return "TDAQ_CALO_PREPROC";
    case TDAQ_CALO_CLUSTER_PROC_DAQ:
      return "TDAQ_CALO_CLUSTER_PROC_DAQ";
    case TDAQ_CALO_CLUSTER_PROC_ROI:
      return "TDAQ_CALO_CLUSTER_PROC_ROI";
    case TDAQ_CALO_JET_PROC_DAQ:
      return "TDAQ_CALO_JET_PROC_DAQ";
    case TDAQ_CALO_JET_PROC_ROI:
      return "TDAQ_CALO_JET_PROC_ROI";
    case TDAQ_MUON_CTP_INTERFACE:
      return "TDAQ_MUON_CTP_INTERFACE";
    case TDAQ_CTP:
      return "TDAQ_CTP";
    case TDAQ_L2SV:
      return "TDAQ_L2SV";
    case TDAQ_SFI:
      return "TDAQ_SFI";
    case TDAQ_SFO:
      return "TDAQ_SFO";
    case TDAQ_LVL2:
      return "TDAQ_LVL2";
    case TDAQ_EVENT_FILTER:
      return "TDAQ_EVENT_FILTER";
    case TDAQ_MET_LAR:
      return "TDAQ_MET_LAR";
    case TDAQ_MET_TIL:
      return "TDAQ_MET_TIL";
    case FORWARD_BCM:
      return "FORWARD_BCM";
    case FORWARD_LUCID:
      return "FORWARD_LUCID";
    case FORWARD_ZDC:
      return "FORWARD_ZDC";
    case FORWARD_ALPHA:
      return "FORWARD_ALPHA";
    case OTHER:
      return "OTHER";
    }

    return "UNKNOWN_SUBDETCTOR";
  }
  
  std::string human_group(SubDetectorGroup sdet)
  {
    switch (sdet) {
    case ANY_DETECTOR:
      return "ANY_DETECTOR";
    case PIXEL:
      return "PIXEL";
    case SCT:
      return "SCT";
    case TRT:
      return "TRT";
    case LAR:
      return "LAR";
    case TILECAL:
      return "TILECAL";
    case MUON:
      return "MUON";
    case TDAQ:
      return "TDAQ";
    case FORWARD:
      return "FORWARD";
    case OTHER_DETECTORS:
      return "OTHER_DETECTORS";
    }
    
    return "UNKNOWN_SUBDETCTOR_GROUP";
  }
}

//-----------------------------------------------------------------------------------------      
Anp::RobService::RobService()
  :fDebug(false),
   fUseLocal(true)
{
}

//-----------------------------------------------------------------------------------------      
void Anp::RobService::Config(const Registry &reg)
{
  //
  // Read configuration
  // 
  reg.Get("RobService", "Debug",  fDebug);
  reg.Get("RobService::UseLocal", fUseLocal);

  //
  // Read ROS/ROB xml
  //
  string ros_xml;
  if(reg.Get("ROSXML", ros_xml) && !ros_xml.empty()) {
    log() << "Config - reading ROS-ROB map from: " << ros_xml << endl;

    Anp::ReadROSXML(ros_xml, fRosVec, fDebug);

    //
    // Build map from ROB to ROS and ROB to ROBIN
    //
    for(unsigned i = 0; i < fRosVec.size(); ++i) {
      const ROS &ros = fRosVec.at(i);
      
      for(set<unsigned>::const_iterator rit = ros.robs.begin(); rit != ros.robs.end(); ++rit) {
	if(!fRosMap.insert(map<unsigned, ROS>::value_type(*rit, ros)).second) {
	  log() << "Config - duplicate ROB id in ROS map: " << *rit << endl;
	}
      }

      for(vector<ROBIN>::const_iterator rit_robin = ros.robins.begin(); rit_robin != ros.robins.end(); ++rit_robin) {
	for(set<unsigned>::const_iterator rit = rit_robin->robs.begin(); rit != rit_robin->robs.end(); ++rit) {
	  if(!fRobinMap.insert(map<unsigned, ROBIN>::value_type(*rit, *rit_robin)).second) {
	    log() << "Config - duplicate ROB id in ROBIN map: " << *rit << endl;
	  }
	}
      }
    }
  }
  else {
    log() << "Config - missing ROSXML..." << endl;
  }
}

//-----------------------------------------------------------------------------------------      
void Anp::RobService::AddConfig(const TrigMonConfig &config)
{
  //
  // Decode mapping for subdetectors and ROB ids
  //  - this depends on some definitions from TrigNtPostTool
  //
  
  const std::vector<std::string> &keys = config.getPairKey();
  const std::vector<std::string> &vars = config.getPairVal();

  if(keys.size() == vars.size()) {
    if(fDebug) {
      cout << "RobService::Set - config has " << keys.size() << " variable(s)" << endl;
      
      for(unsigned i = 0; i < keys.size(); ++i) {
	cout << "   " << keys.at(i) << " value=" << vars.at(i) << endl;
      }
    }
    
    for(unsigned i = 0; i < keys.size(); ++i) {
      ReadOldKey(keys.at(i), vars.at(i));
    }
  }
  else {
    cout << "RobService::ReadRob - string keys/values size error" << endl;
    return;
  }

  const std::vector<std::string> &int_keys = config.getVarName();
  const std::vector<uint32_t>    &int_vars = config.getVarId();
  
  if(int_keys.size() == int_vars.size()) {
    for(unsigned i = 0; i < int_keys.size(); ++i) {
      //
      // Decone mapping between ids and names
      //
      ReadNewKey(int_keys.at(i), int_vars.at(i));
    }
  }
  else {
    cout << "RobService::ReadRob - int keys/values size error" << endl;
    return;
  }
}

//-----------------------------------------------------------------------------------------      
void Anp::RobService::ReadOldKey(const std::string &key, const std::string &val)
{
  //
  // Read ROB id key using "old" format (before TrigCostMonitor-00-00-44)
  //
  if(key.size() < 6) return;

  const unsigned rob = std::atoi(key.substr(6, key.size()-6).c_str());
  if(rob < 1) return;
  
  std::map<unsigned, SubDet>::iterator rit = fRobMap.find(rob);
  if(rit != fRobMap.end()) return;
  
  std::vector<std::string> groups;
  Anp::StringTok(groups, val, ":");
  
  if(groups.size() != 4) return;

  SubDet det;   
  if(groups.at(0) == "SUBDET") det.subdet = groups.at(1);
  if(groups.at(2) == "GROUP")  det.group  = groups.at(3);
  
  det.subdet_id = GetSubDetId(rob);
  det.group_id  = GetGroupId(rob);
  
  fRobMap.insert(std::map<unsigned, SubDet>::value_type(rob, det));
}

//-----------------------------------------------------------------------------------------      
void Anp::RobService::ReadNewKey(const std::string &key, const uint32_t id)
{
  //
  // Read ROB id key using "new" format (TrigCostMonitor-00-00-44 or later)
  //
  //cout << "Reading Key: " << key << " and id: " << id << endl;
  if(key.size() < 10) {
  
    std::vector<std::string> groups;
    Anp::StringTok(groups, key, ":");
    
    if(groups.size() != 5){
      //cout << "RETURNING - Reading Key: " << key << " and id: " << id << endl;
      return;
    }
    
    if(fDebug) {
      cout << "RobService::ReadNewKey - id=" << id << ", key=" << key << endl;
    }
    
    SubDet det;
    if(groups.at(1) == "SUBDET") det.subdet = groups.at(2);
    if(groups.at(3) == "GROUP")  det.group  = groups.at(4);
    
    if(groups.at(0) == "ROB") {
      //
      // id value is ROB id
      //
      det.subdet_id = GetSubDetId(id);
      det.group_id  = GetGroupId(id);
    }
    else if(groups.at(0) == "DET") {
      //
      // id value is SubDetector id
      //
      det.subdet_id = id;
      det.group_id  = id & 0xf0;
    }
  }
  
  //
  // Extract HLT application name to id map
  //
  if(key.find("xpu") != string::npos || key.find("APP") != string::npos) {
    
    Exec::FarmNodeMap::iterator it = fHltMap.find(id);
    if(it == fHltMap.end()){
      fHltMap.insert(Exec::FarmNodeMap::value_type(id, key));
    }
  }
  //cout << "NXPU:" << fHltMap.size() << endl;
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetSubDet(unsigned rob_id) const
{
  //
  // Convert ROB id to subdetector name
  //
  if(fUseLocal) {
    Anp::human_detector(static_cast<Anp::SubDetector>((rob_id >> 16) & 0xff));
  }
  else {
    std::map<unsigned, SubDet>::const_iterator rit = fRobMap.find(rob_id);
    if(rit != fRobMap.end()) {
      return rit->second.subdet;
    }
  }

  return "";
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetGroup(unsigned rob_id) const
{
  //
  // Convert ROB id to group name
  //
  if(fUseLocal) {
    Anp::human_group(static_cast<Anp::SubDetectorGroup>((rob_id >> 16) & 0xf));
  }
  else {
    std::map<unsigned, SubDet>::const_iterator rit = fRobMap.find(rob_id);
    if(rit != fRobMap.end()) {
      return rit->second.group;
    }
  }
    return "";
}

//-----------------------------------------------------------------------------------------      
uint32_t Anp::RobService::GetGroupIdFromSubDetId(uint32_t subdet_id) const
{
  // 
  // Convert subdet_id to group_id
  //
  if(fUseLocal) {
    if(fDebug) cout << "subdet_id: " << subdet_id << " found group id: " << ((subdet_id >> 16) & 0xf) << endl;
    return ((subdet_id >> 4) & 0xf);
  }
  else {
    for(std::map<unsigned, SubDet>::const_iterator rit = fRobMap.begin(); rit != fRobMap.end(); ++rit){
      if(rit->second.subdet_id == subdet_id){
	return rit->second.group_id;
      }
    }
  }
    return ((subdet_id >> 4) & 0xf);
}

//-----------------------------------------------------------------------------------------      
uint32_t Anp::RobService::GetSubDetId(unsigned rob_id) const
{
  //
  // Convert ROB id to subdetector name
  //
  std::map<unsigned, SubDet>::const_iterator rit = fRobMap.find(rob_id);
  if(rit != fRobMap.end()) {
    return rit->second.subdet_id;
  }
  
  return ((rob_id >> 16) & 0xff);
}

//-----------------------------------------------------------------------------------------      
uint32_t Anp::RobService::GetGroupId(uint32_t rob_id) const
{
  //
  // Convert ROB id to subdetector name
  //
  const std::map<unsigned, SubDet>::const_iterator rit = fRobMap.find(rob_id);
  if(rit != fRobMap.end()) {
    return rit->second.group_id;
  }
  
  return ((rob_id >> 16) & 0xf);
}

//-----------------------------------------------------------------------------------------      
uint32_t Anp::RobService::GetRosId(uint32_t rob_id) const
{
  //
  // Convert ROB id to ROS id
  //
  const std::map<unsigned, ROS>::const_iterator rit = fRosMap.find(rob_id);
  if(rit != fRosMap.end()) {
    return rit->second.ros_id;
  }
  
  return 0;
}

//-----------------------------------------------------------------------------------------      
uint32_t Anp::RobService::GetRobinId(uint32_t rob_id) const
{
  //
  // Convert ROBIN id to Rob id
  //
  const std::map<unsigned, ROBIN>::const_iterator rit = fRobinMap.find(rob_id);
  if(rit != fRobinMap.end()) {
    return rit->second.robin_id;
  }
  
  return 0;
}

//-----------------------------------------------------------------------------------------      
uint32_t Anp::RobService::GetNumberROBsInDet(uint32_t subdet_id) const
{
  //
  // Find number of robs in subdetector
  //
  unsigned nrobs   = 0;
  unsigned t_nrobs = 0; 

  for(unsigned i = 0; i<fRosVec.size(); ++i){

    t_nrobs=fRosVec.at(i).robs.size();
    if(t_nrobs == 0) continue;
    if(GetSubDetId(*(fRosVec.at(i).robs.begin())) == subdet_id) nrobs += t_nrobs;

  }
  if(fDebug) cout << "subdet_id: " << subdet_id << " number of robs: " << nrobs << endl;

  return nrobs;
}

//-----------------------------------------------------------------------------------------      
uint32_t Anp::RobService::GetNumberROBsInGroup(uint32_t group_id) const
{
  //
  // Get number of robs in sub detector group
  //
  unsigned nrobs   = 0;
  unsigned t_nrobs = 0; 

  for(unsigned i = 0; i<fRosVec.size(); ++i){

    t_nrobs=fRosVec.at(i).robs.size();
    if(t_nrobs == 0) continue;
    if(GetGroupId(*(fRosVec.at(i).robs.begin())) == group_id) nrobs += t_nrobs;

  }

  return nrobs;
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetGrpFromSubDet(uint32_t subdet_id)  const
{
  //
  // Get sub detector name from subdetector id
  //
  return GetGroupFromId(GetGroupIdFromSubDetId(subdet_id));
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetSubDetFromId(uint32_t id) const
{
  //
  // Convert subdetector id to name
  //
  static std::vector<SubDetector> subdets = Anp::all_subdets();
  
  for(unsigned i = 0; i < subdets.size(); ++i) {    
    if(subdets.at(i) == int(id)) {
      return Anp::human_detector(subdets.at(i));
    }
  }

  return "UNKNOWN_SUBDET";
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetGroupFromId(uint32_t id) const
{
  //
  // Convert subdetector id to name
  //
  static std::vector<SubDetectorGroup> groups = Anp::all_subdet_groups();
  
  for(unsigned i = 0; i < groups.size(); ++i) {    
    if(groups.at(i) == int(id)) {
      return Anp::human_group(groups.at(i));
    }
  }

  return "UNKNOWN_GROUP";
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetRosFromId(uint32_t id) const
{
  //
  // Convert ROS id to name
  //
  for(unsigned i = 0; i < fRosVec.size(); ++i) {
    const ROS &ros = fRosVec.at(i);
    if(ros.ros_id == id) {
      return ros.ros;
    }
  }
  cout << "UNKNOWN_ROS: " << id << endl;
  return "UNKNOWN_ROS";
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetRobinFromId(uint32_t id) const
{
  //
  // Convert ROS id to name
  //
  for(unsigned i = 0; i < fRosVec.size(); ++i) {
    const ROS &ros = fRosVec.at(i);
    for(unsigned j=0; j < ros.robins.size(); ++j) {
      const ROBIN &robin = ros.robins.at(j);
      if(robin.robin_id == id) {
	return robin.robin;
      }
    }
  }

  return "UNKNOWN_ROBIN";
}

//-----------------------------------------------------------------------------------------      
std::string Anp::RobService::GetHLTFromId(uint32_t id) const
{
  //
  // Convert HLT application id to name
  //
  for(Exec::FarmNodeMap::const_iterator hit = fHltMap.begin(); hit != fHltMap.end(); ++hit) {
    if(hit->first == id) {
      return hit->second;
    }
  }

  return "UNKNOWN_HLT";
}

//-----------------------------------------------------------------------------------------      
std::vector<int> Anp::RobService::GetAllSubDet() const
{
  //
  // Get all subdetectors
  //
  vector<int> svec;
  std::vector<SubDetector> subdets = Anp::all_subdets(); 
  
  for(unsigned i = 0; i < subdets.size(); ++i) {
    svec.push_back(subdets.at(i));
  }
  
  return svec;
}

//-----------------------------------------------------------------------------------------      
std::set<uint32_t> Anp::RobService::GetSubDetFromROB(const TrigMonROB &rob) const
{
  //
  // Return list of sub-detectors within one ROB request
  //
  std::set<uint32_t> subdets;

  const std::vector<TrigMonROBData> &rvec = rob.getData();
  const std::vector<TrigMonROBSum>   svec = rob.getSum();

  for(unsigned int i = 0; i < rvec.size(); ++i) {
    const TrigMonROBData &data = rvec.at(i);
    const uint32_t subdet = GetSubDetId(data.getROBId());
    
    subdets.insert(subdet);
  }

  //
  // Fill data from ROB summaries
  //
  for(unsigned int i = 0; i < svec.size(); ++i) {
    const TrigMonROBSum &data = svec.at(i);

    subdets.insert(data.getSubDet());
  }
  
  return subdets;
}

//-----------------------------------------------------------------------------------------      
std::set<uint32_t> Anp::RobService::GetROSFromROB(const TrigMonROB &rob) const
{
  //
  // Return list of sub-detectors within one ROB request
  //
  std::set<uint32_t> rosids;

  const std::vector<TrigMonROBData> &rvec = rob.getData();
  const std::vector<TrigMonROBSum>   svec = rob.getSum();

  for(unsigned int i = 0; i < rvec.size(); ++i) {
    const TrigMonROBData &data = rvec.at(i);
    const uint32_t rosid = GetRosId(data.getROBId());
    
    rosids.insert(rosid);
  }

  return rosids;
}

//-----------------------------------------------------------------------------------------      
std::ostream& Anp::RobService::log() const  
{
  cout << "RobService::";
  return cout;
}

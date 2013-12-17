/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <sstream>

// Local
#include "TrigCostExec/ExecData.h"

using namespace std;

//-----------------------------------------------------------------------------------------      
void Anp::Exec::AlgCall::Print(ostream &os) const
{ 
  os << "chn: "  << chn_conf->getName() 
     << " seq: " << seq_conf->getName() 
     << " alg: " << alg_conf->getName();
  
  for(unsigned i = 0; i < roi_data.size(); ++i) {
    const ROILoad &roi = roi_data.at(i);
    os << " roi[" << roi.roi_id << "] eta=" << roi.roi_eta << " phi=" << roi.roi_phi;
  }
}

//-----------------------------------------------------------------------------------------      
std::string Anp::Exec::AlgCall::GetDetName(uint32_t det_id) const
{
  //
  // Find detector name
  //
  for(unsigned i = 0; i < rob_data.size(); ++i) {
    const ROBCall &rob = rob_data.at(i);

    for(unsigned r = 0; r < rob.rob_data.size(); ++r) {
      const ROBData &data = rob.rob_data.at(r);
      
      if(data.rob_conf.valid() && data.rob_conf->det_id == det_id) {
	return data.rob_conf->det_name;
      }
    }
  }

  return "UNKNOWN_DET";
}

//-----------------------------------------------------------------------------------------      
std::string Anp::Exec::AlgCall::GetROSName(uint32_t ros_id) const
{
  //
  // Find ROS name
  //
  for(unsigned i = 0; i < rob_data.size(); ++i) {
    const ROBCall &rob = rob_data.at(i);

    for(unsigned r = 0; r < rob.rob_data.size(); ++r) {
      const ROBData &data = rob.rob_data.at(r);
      
      if(data.rob_conf.valid() && data.rob_conf->ros_id == ros_id) {
	return data.rob_conf->ros_name;
      }
    }
  }

  return "UNKNOWN_ROS";
}

//-----------------------------------------------------------------------------------------      
std::string Anp::Exec::AlgCall::GetROBINName(uint32_t robin_id) const
{
  //
  // Find ROBIN name
  //
  for(unsigned i = 0; i < rob_data.size(); ++i) {
    const ROBCall &rob = rob_data.at(i);

    for(unsigned r = 0; r < rob.rob_data.size(); ++r) {
      const ROBData &data = rob.rob_data.at(r);
      
      if(data.rob_conf.valid() && data.rob_conf->robin_id == robin_id) {
	return data.rob_conf->robin_name;
      }
    }
  }

  return "UNKNOWN_ROBIN";
}

//-----------------------------------------------------------------------------------------      
void Anp::Exec::AlgCall::FillTEs(std::vector<TrigMonTE> &TEs)
{
  //
  // Loop over all TEs in the event and attach all the TEs to the RoI
  //

  for(unsigned int k = 0; k < this->roi_data.size(); ++k){
    for(unsigned i = 0; i < TEs.size(); ++i){
      TrigMonTE TE = TEs[i];
      if(TE.getRoiId()[0] == 255 || TE.getRoiId()[0] == 253){ 
	this->roi_data[k].isPassed = true;
	break;
      }

      if(!TE.getId() == this->seq_conf->getId()) continue;
      std::vector<uint8_t> rois = TE.getRoiId();  

      for(unsigned int j = 0; j < rois.size(); ++j){
	if(rois[j] == this->roi_data[k].roi_id){
          this->roi_data[k].isPassed = true;
        }
      } 
    }
  }	 
} 

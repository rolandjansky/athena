/*                                                                                                        
													
													  
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                   
 
*/
///////////////////////////////////////////////////////////////////////////                                  
//Utils for the main MMRawDataMonAlg.cxx                                                                    
        
//Part of MMRawDataMonAlg.h                                                                                 
         
//Authors                                                                                                   
         
//   see MMRawDataMonAlg.cxx                                                                                
        
///////////////////////////////////////////////////////////////////////////                                 
         

#include "MMRawDataMonitoring/MMRawDataMonAlg.h"
#include <string>
#include <stdexcept>

int MMRawDataMonAlg::get_PCB_from_channel(int channel) const {

  if (channel>0 && channel<=1024) return 1;
  if (channel>1024 && channel<=2048) return 2;
  if (channel>2048 && channel<=3072) return 3;
  if (channel>3072 && channel<=4096) return 4;
  if (channel>4096 && channel<=5120) return 5;

  throw std::invalid_argument( "channel is not valid!" );
}

int MMRawDataMonAlg::get_sectorPhi_from_stationPhi_stName(int stationPhi,const std::string & stName) const {
  
  if (stationPhi==1 && stName=="MMS") return 1;
  if (stationPhi==1 && stName=="MML") return 2;
  if (stationPhi==2 && stName=="MMS") return 3;
  if (stationPhi==2 && stName=="MML") return 4;
  if (stationPhi==3 && stName=="MMS") return 5;
  if (stationPhi==3 && stName=="MML") return 6;
  if (stationPhi==4 && stName=="MMS") return 7;
  if (stationPhi==4 && stName=="MML") return 8;
  if (stationPhi==5 && stName=="MMS") return 9;
  if (stationPhi==5 && stName=="MML") return 10;
  if (stationPhi==6 && stName=="MMS") return 11;
  if (stationPhi==6 && stName=="MML") return 12;
  if (stationPhi==7 && stName=="MMS") return 13;
  if (stationPhi==7 && stName=="MML") return 14;
  if (stationPhi==8 && stName=="MMS") return 15;
  if (stationPhi==8 && stName=="MML") return 16;

  throw std::invalid_argument( "stationPhi and stName are not valid!" );

}

int MMRawDataMonAlg::get_sectorEta_from_stationEta(int stationEta) const {
 
  //  1<-0  0-> 1
  if (std::abs(stationEta==1)) return 0;                                                            
  if (std::abs(stationEta==2)) return 1;

  return -1;

}

int MMRawDataMonAlg::get_bin_for_occ_CSide_hist(int stationEta, int multiplet, int gas_gap) const {

  static const int max_gas_gap = 4;
  static const int max_multiplet = 2;

  return (stationEta+2)*(max_gas_gap*max_multiplet)+(multiplet-1)*max_gas_gap +(gas_gap-1);

}

int MMRawDataMonAlg::get_bin_for_occ_ASide_hist(int stationEta, int multiplet, int gas_gap) const {

  static const int max_gas_gap = 4;
  static const int max_multiplet = 2;

  return (stationEta-1)*(max_gas_gap*max_multiplet)+(multiplet-1)*max_gas_gap +(gas_gap-1);

}


int MMRawDataMonAlg::get_bin_for_occ_CSide_pcb_eta2_hist(int stationEta, int multiplet, int gas_gap, int PCB) const {

  static const int max_pcb = 3;
  static const int max_gas_gap = 4;
  if (stationEta != -2) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb+ (gas_gap-1)*max_pcb + (PCB-1);

}

int MMRawDataMonAlg::get_bin_for_occ_CSide_pcb_eta1_hist(int stationEta, int multiplet, int gas_gap, int PCB) const {

  static const int max_pcb = 5;
  static const int max_gas_gap = 4;
  if (stationEta != -1) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb+ (gas_gap-1)*max_pcb + (PCB-1);

}


int MMRawDataMonAlg::get_bin_for_occ_ASide_pcb_eta2_hist(int stationEta, int multiplet, int gas_gap, int PCB) const {

  static const int max_pcb = 3;
  static const int max_gas_gap = 4;
  if (stationEta != 2) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb+ (gas_gap-1)*max_pcb + (PCB-1);

}


int MMRawDataMonAlg::get_bin_for_occ_ASide_pcb_eta1_hist(int stationEta, int multiplet, int gas_gap, int PCB) const {

  static const int max_pcb = 5;
  static const int max_gas_gap = 4;
  if (stationEta != 1) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb+ (gas_gap-1)*max_pcb + (PCB-1);
}


int MMRawDataMonAlg::get_bin_for_occ_lb_CSide_pcb_eta2_hist(int stationEta, int multiplet, int gas_gap, int PCB,int isector) const {

  static const int max_pcb = 3;
  static const int max_gas_gap = 4;
  static const int max_isector = 2;
  if (stationEta != -2) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb*max_isector+ (gas_gap-1)*max_pcb*max_isector + isector*max_pcb+ (PCB-1);

}


int MMRawDataMonAlg::get_bin_for_occ_lb_CSide_pcb_eta1_hist(int stationEta, int multiplet, int gas_gap, int PCB,int isector) const {

  static const int max_pcb = 5;
  static const int max_gas_gap = 4;
  static const int max_isector = 2;
  if (stationEta != -1) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb*max_isector+ (gas_gap-1)*max_pcb*max_isector + isector*max_pcb + (PCB-1);

}

int MMRawDataMonAlg::get_bin_for_occ_lb_ASide_pcb_eta1_hist(int stationEta, int multiplet, int gas_gap, int PCB,int isector) const {

  static const int max_pcb = 5;
  static const int max_gas_gap = 4;
  static const int max_isector = 2;
  if (stationEta != 1) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb*max_isector+ (gas_gap-1)*max_pcb*max_isector + isector*max_pcb + (PCB-1);
}


int MMRawDataMonAlg::get_bin_for_occ_lb_ASide_pcb_eta2_hist(int stationEta, int multiplet, int gas_gap, int PCB,int isector) const {

  static const int max_pcb = 3;
  static const int max_gas_gap = 4;
  static const int max_isector = 2;
  if (stationEta != 2) return -1;

  return  (multiplet-1)*max_gas_gap*max_pcb*max_isector+ (gas_gap-1)*max_pcb*max_isector + isector*max_pcb + (PCB-1);

}


/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/MuonChambersRange.h"

bool selectChambersRange(const std::string& f_hardware_name, const std::string& f_chamberName, 
			 int f_stationEta, int f_StationEta, 
			 int f_stationPhi, int f_StationPhi,
			 const std::string& f_StationSize)
{
  bool pass=false;
  std::string size="XXX";
  int f_StationPhi_check=f_StationPhi;
  int f_StationEta_check=f_StationEta;	
  std::string::size_type small_chamber = f_hardware_name.find( 'S', 0 );
  std::string::size_type feet_chamber = f_hardware_name.find( 'F', 0 );
  std::string::size_type ground_chamber = f_hardware_name.find( 'G', 0 );
  std::string::size_type large_chamber = f_hardware_name.find( 'L', 0 );
  //BIM and BIR still not in!!!!

  if (small_chamber != std::string::npos) {
    size="S"; 
    if (f_StationPhi!=-100 && f_StationPhi!=1 && (f_StationPhi%2)==0) {f_StationPhi_check=(f_StationPhi/2);} 
    else if (f_StationPhi==-100) {f_StationPhi_check=-100;}
    else {f_StationPhi_check=-99;}
  }
  if (feet_chamber != std::string::npos) {
    size="S"; 
    if (f_StationPhi!=-100 && f_StationPhi!=1 && (f_StationPhi%2)==0) {f_StationPhi_check=(f_StationPhi/2);} 
    else if (f_StationPhi==-100) {f_StationPhi_check=-100;}
    else {f_StationPhi_check=-99;}
    if (f_StationEta!=-100 && (f_StationEta%2)!=0) {f_StationEta_check=(f_StationEta/2)+1;}
    else if (f_StationEta==-100) {f_StationEta_check=-100;} 
    else {f_StationEta_check=-99;}
  }
  if (ground_chamber != std::string::npos) {
    size="S"; 
    if (f_StationPhi!=-100 && f_StationPhi!=1 && (f_StationPhi%2)==0) {f_StationPhi_check=(f_StationPhi/2);} 
    else if (f_StationPhi==-100) {f_StationPhi_check=-100;}
    else {f_StationPhi_check=-99;}
    if (f_StationEta!=-100 && (f_StationEta%2)==0) {f_StationEta_check=(f_StationEta/2);}
    else if (f_StationEta==-100) {f_StationEta_check=-100;} 
    else {f_StationEta_check=-99;}	
  }		  	
  if (large_chamber != std::string::npos) {
    size="L";
    if (f_StationPhi==1) {f_StationPhi_check=1;}
    else if (f_StationPhi!=-100 && f_StationPhi!=1 && (f_StationPhi%2)!=0) {f_StationPhi_check=(f_StationPhi/2)+1;} 
    else if (f_StationPhi==-100) {f_StationPhi_check=-100;}
    else {f_StationPhi_check=-99;}	
  }
  if ((f_chamberName=="XXX" || f_chamberName==f_hardware_name) 
      && (f_StationEta_check==f_stationEta || f_StationEta_check==-100) 
      && (f_StationPhi_check==f_stationPhi || f_StationPhi_check==-100) 
      && (f_StationSize=="XXX" || f_StationSize==size) ){pass=true;}

  return pass;
}

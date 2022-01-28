/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include  "MuonRPC_Cabling/hardwareId.h"

hardwareId::hardwareId(){
  m_stName=-999;
  m_stEta=-999;
  m_stPhi=-999;
  m_dbR=-999;
  m_dbPhi=-999;
  m_dbZ=-999;
  m_gasGap=-999;
}

bool hardwareId::SetHwId(const std::string& x_HwId){
  // READ A STRING WITH FORMAT:
  // BML6C03.CO.DP2.Ly1.DZ1

  // DECODE THE STRING USING FIXED FIELDS

  m_HwId=x_HwId;
  m_HwStationName=x_HwId.substr(0,3);
  std::istringstream StNum(x_HwId.substr(3,1)); StNum >> m_HwStationNumber;

  m_HwSide = x_HwId.substr(4,1);

  std::istringstream Sect(x_HwId.substr(5,2));Sect >> m_HwSector; 
  if (m_HwSector<1||m_HwSector>16) return false;
  m_HwPICO = x_HwId.substr(8,2);
  if (x_HwId.substr(13,1)=="1"){
    m_HwDP=1;
  }else if (x_HwId.substr(13,1)=="2"){
    m_HwDP=2;
  }else{
    return false;
  }

  if (x_HwId.substr(17,1)=="0"){
    m_HwLy=0;
  }else if (x_HwId.substr(17,1)=="1"){
    m_HwLy=1;
  }else{
    return false;
  }


  if (x_HwId.substr(21,1)=="1"){
    m_HwDZ=1;
  }else if (x_HwId.substr(21,1)=="2"){
    m_HwDZ=2;
  }else{
    return false;
  }

  
  // convert Hw to offline ids
  return convert_HW2offline();
  
}


bool hardwareId::setOfflineID(int x_stName, int x_stEta, int x_stPhi, int x_dbR, int x_dbPhi, int x_dbZ, int x_gasGap){
  
  m_stName=x_stName;
  m_stEta=x_stEta;
  m_stPhi=x_stPhi;
  m_dbR=x_dbR;
  m_dbPhi=x_dbPhi;
  m_dbZ=x_dbZ; 
  m_gasGap=x_gasGap;
  
  return convert_offline2HW();
  
}


bool hardwareId::convert_HW2offline(){
  
  // convert from HW to offline
  if (m_HwStationName=="BML") {
    m_stName=2; 
    m_stEta=m_HwStationNumber; 
    // exception: Hw jumps BML4 in sector 13
    if (m_HwSector==13&&m_HwStationNumber>=5) m_stEta=m_HwStationNumber-1;
  }else if (m_HwStationName=="BMS"){
    m_stName=3;
    m_stEta=m_HwStationNumber; 
  }else if (m_HwStationName=="BOL"){
    m_stName=4;
    m_stEta=m_HwStationNumber; 
  } else if (m_HwStationName=="BOS"){
    m_stName=5;
    m_stEta=m_HwStationNumber; 
  } else if (m_HwStationName=="BMF"){
    m_stName=8;
    m_stEta=m_HwStationNumber; 
  } else if (m_HwStationName=="BOF"){ 
    m_stName=9;
    // exception: BOF/BOG numbering in sectors 12,14
    m_stEta=(m_HwStationNumber+1)/2;
  } else if (m_HwStationName=="BOG") {
    m_stName=10;
    m_stEta=m_HwStationNumber/2;
  } else if (m_HwStationName=="BME"){
    m_stName=53;
    m_stEta=1; 
  } else if (m_HwStationName=="BOE"){
    m_stName=4;
    m_stEta=8; 
  } else {
    return false;
  }
  
  if (m_HwSide=="C") {
    m_stEta = -m_stEta;
  }
  
  if (m_HwPICO=="PI"){
    m_dbR=2;
  }else{
    m_dbR=1;
  }
 
  m_stPhi = (m_HwSector+1)/2;
  m_dbPhi = m_HwDP; // not 100% sure
  m_gasGap = m_HwLy+1;
  m_dbZ = m_HwDZ;
  
  return true;
  
}

bool hardwareId::convert_offline2HW(){
  // convert from HW to offline
  
  m_HwSector = 2*m_stPhi-1;
  if (m_stName==3||m_stName==5||m_stName==8||m_stName==9||m_stName==10) m_HwSector +=1; // Small Sectors
  
  int absEta=m_stEta;  
  if (m_stEta<0) absEta=-m_stEta;
  
  if (m_stName==2){
    m_HwStationName="BML";
    m_HwStationNumber=absEta;
    if (m_HwSector==13&&absEta>=4) m_HwStationNumber=absEta+1; 
  } else if (m_stName==3){
    m_HwStationName="BMS";
    m_HwStationNumber=absEta;
  } else if (m_stName==4){
    m_HwStationName="BOL";
    m_HwStationNumber=absEta;
  } else if (m_stName==5){
    m_HwStationName="BOS";
    m_HwStationNumber=absEta;
  } else if (m_stName==8){
    m_HwStationName="BMF";
    m_HwStationNumber=absEta;
  } else if (m_stName==9){
    m_HwStationName="BOF";
    m_HwStationNumber=(absEta*2)-1;
  } else if (m_stName==10){
    m_HwStationName="BOG";
    m_HwStationNumber=absEta*2;
  } else if (m_stName==53){
    m_HwStationName="BME";
    m_HwStationNumber=4;
  }
  
  m_HwSide="A";
  if (m_stEta<0)   m_HwSide="C";
  if (m_stEta==0)  m_HwSide="B"; 
  
  m_HwDP=m_dbPhi ;
  m_HwLy=m_gasGap-1; 
  m_HwDZ=m_dbZ;
  
  if (m_dbR==1){
    m_HwPICO="CO";
  }else{
    m_HwPICO="PI";
  }

  // BML6C03.CO.DP2.Ly1.DZ1
  std::ostringstream  HW;
  HW << m_HwStationName 
     << m_HwStationNumber
     << m_HwSide 
     << std::setw(2) << std::setfill('0') <<m_HwSector<< std::setw(1)
     <<"."<<   m_HwPICO
     <<".DP"<< m_HwDP
     <<".Ly"<< m_HwLy
     <<".DZ"<< m_HwDZ;
  m_HwId = HW.str();

  return true;
  
}


void hardwareId::dump(){
  std::cout <<" Hardware ID " << m_HwId << std::endl;
  std::cout <<" offline ID  " 
       << " stName="<<m_stName 
       << " stEta="<<m_stEta 
       << " oct="<<m_stPhi 
       << " dbR="<<m_dbR
       << " dbPhi="<<m_dbPhi
       << " dbZ="<<m_dbZ
       << " gasGap="<<m_gasGap << std::endl;
}






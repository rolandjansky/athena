/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "TgcChamber.h"
#include "TString.h"
#include <iostream>

void TGC::TgcChamber::initChamber(const std::string& cham_name){
  // example: A01M01f01E01
  TGCSIDE iSide = (cham_name.substr(0,1).compare("A")==0)?(TGC::TGCSIDE::TGCASIDE):(TGC::TGCSIDE::TGCCSIDE);
  int iSec = std::stoi(cham_name.substr(1,2));
  int iM = std::stoi(cham_name.substr(4,2));
  int iPhi = std::stoi(cham_name.substr(7,2));
  int iEta = std::stoi(cham_name.substr(10,2));
  initChamber(iSide,iSec,iM,iPhi,iEta);
}
void TGC::TgcChamber::initChamber(TGCSIDE side,int sec,int m, int phi, int eta)
{
  m_iSide = side;
  m_iSec = sec;
  m_iM = m;
  m_iPhi = phi;
  m_iEta = eta;
  if(m_iM==1 && m_iEta==0){
    m_stationName = 41;
    m_stationEta = (m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1);
    m_stationPhi = (m_iSec-1)*2 + m_iPhi/2+1 - 1;
    if( m_stationPhi <= 0 ) m_stationPhi += 24;
    m_detId = TGC::TGCDETECTOR::TGCM1;
  }else if(m_iM==1 && m_iEta!=0){
    m_stationName = 42;
    m_stationEta = ((m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1)) * (5 - m_iEta);
    m_stationPhi = (m_iSec-1)*4 + m_iPhi - 1;
    if( m_stationPhi <= 0 ) m_stationPhi += 48;
    m_detId = TGC::TGCDETECTOR::TGCM1;
  }else if(m_iM==2 && m_iEta==0){
    m_stationName = 43;
    m_stationEta = (m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1);
    m_stationPhi = (m_iSec-1)*2 + m_iPhi/2+1 - 1;
    if( m_stationPhi <= 0 ) m_stationPhi += 24;
    m_detId = TGC::TGCDETECTOR::TGCM2;
  }else if(m_iM==2 && m_iEta!=0){
    m_stationName = 44;
    m_stationEta = ((m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1)) * (6 - m_iEta);
    m_stationPhi = (m_iSec-1)*4 + m_iPhi - 1;
    if( m_stationPhi <= 0 ) m_stationPhi += 48;
    m_detId = TGC::TGCDETECTOR::TGCM2;
  }else if(m_iM==3 && m_iEta==0){
    m_stationName = 45;
    m_stationEta = (m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1);
    m_stationPhi = (m_iSec-1)*2 + m_iPhi/2+1 - 1;
    if( m_stationPhi <= 0 ) m_stationPhi += 24;
    m_detId = TGC::TGCDETECTOR::TGCM3;
  }else if(m_iM==3 && m_iEta!=0){
    m_stationName = 46;
    m_stationEta = ((m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1)) * (6 - m_iEta);
    m_stationPhi = (m_iSec-1)*4 + m_iPhi - 1;
    if( m_stationPhi <= 0 ) m_stationPhi += 48;
    m_detId = TGC::TGCDETECTOR::TGCM3;
  }else if(m_iM==4 && m_iEta==0){
    m_stationName = 47;
    m_stationEta = (m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1);
    m_stationPhi = m_iPhi;
    m_detId = TGC::TGCDETECTOR::TGCFI;
  }else if(m_iM==4 && m_iEta!=0){
    m_stationName = 48;
    m_stationEta = (m_iSide==TGC::TGCSIDE::TGCASIDE)?(+1):(-1);
    if(m_iSec==1){
      m_stationPhi = m_iPhi;
    }else if(m_iSec==3){
      m_stationPhi = m_iPhi + 3;
    }else if(m_iSec==5){
      m_stationPhi = m_iPhi + 6;
    }else if(m_iSec==7){
      m_stationPhi = m_iPhi + 8;
    }else if(m_iSec==9){
      m_stationPhi = m_iPhi + 11;
    }else if(m_iSec==11){
      m_stationPhi = m_iPhi + 13;
    }else if(m_iSec==13){
      m_stationPhi = m_iPhi + 16;
    }else if(m_iSec==15){
      m_stationPhi = m_iPhi + 18;
    }
    if( m_stationPhi <= 0 ) m_stationPhi += 21;
    m_detId = TGC::TGCDETECTOR::TGCEI;
  }
  m_cham_name = Form("%s%02dM%02df%02d%s%02d",(m_iSide==TGC::TGCSIDE::TGCASIDE)?("A"):("C"),m_iSec,m_iM,m_iPhi,(m_iEta==0)?("F"):("E"),m_iEta);
}
void TGC::TgcChamber::initChamber(TGCSIDE side,int stationEta ,int stationPhi, int stationName){
  initChamber(std::abs(stationEta) * ((side==TGC::TGCSIDE::TGCASIDE) ? (+1.0) : (-1.0)) , stationPhi, stationName);
}
void TGC::TgcChamber::initChamber(int stationEta ,int stationPhi, int stationName){
  TGCSIDE iSide = TGC::TGCSIDE::TGCSIDEUNDEF;
  int iSec = 0;
  int iM = 0;
  int iPhi = 0;
  int iEta = 0;
  iSide = (stationEta > 0) ? (TGC::TGCSIDE::TGCASIDE) : (TGC::TGCSIDE::TGCCSIDE);
  iM = 0;
  if (stationName == 41 || stationName == 42) iM = 1;
  else if (stationName == 43 || stationName == 44) iM = 2;
  else if (stationName == 45 || stationName == 46) iM = 3;
  else if (stationName == 47 || stationName == 48) iM = 4; // EIFI
  
  if (iM != 4) { // Big Wheel, (M1,M2,M3)
    if (stationName % 2 == 0) { // Endcap
      int iphi2 = stationPhi + 1; // 2,3,4,..,49
      if (iphi2 >= 48) iphi2 -= 48; // 0,1,2,3 ..., 47
      iSec = int(iphi2 / 4) + 1; // 1,2,3,,,12
      iPhi = iphi2 - (iSec - 1) * 4; // 0,1,2,3
      iEta = (iM == 1) ? (5 - std::abs(stationEta)) : (6 - std::abs(stationEta));
    } else { // Forward
      int iphi2 = stationPhi; // 1,2,3,4,..,25
      if (iphi2 >= 24) iphi2 -= 24; // 0,1,2,3 ...,23
      iSec = int(iphi2 / 2) + 1; // 1,2,3,,,12
      iPhi = iphi2 - (iSec - 1) * 2; // 0,1
      if (iPhi == 1) iPhi = 2; //0,2
      iEta = 0; // F
    }
  } else { // Small Wheel (M4)
    if (stationName == 47) { // FI
      iSec = 0;
      iPhi = stationPhi; // 1,2,3..24
      iEta = 0;
    } else if (stationName == 48) { // EI
      int iphi2 = (stationPhi >= 21) ? (stationPhi - 21) : (stationPhi); // 0,1,2,..,20
      if (iphi2 >= 0 && iphi2 <= 2) {
	iSec = 1;
	iPhi = iphi2; // 0,1,2
      } else if (iphi2 >= 3 && iphi2 <= 5) {
	iSec = 3;
	iPhi = iphi2 - 3; // 0,1,2
      } else if (iphi2 >= 6 && iphi2 <= 8) {
	iSec = 5;
	iPhi = iphi2 - 6; // 0,1,2
      } else if (iphi2 >= 9 && iphi2 <= 10) {
	iSec = 7;
	iPhi = iphi2 - 9 + 1; // 1,2
      } else if (iphi2 >= 11 && iphi2 <= 13) {
	iSec = 9;
	iPhi = iphi2 - 11; // 0,1,2
      } else if (iphi2 >= 14 && iphi2 <= 15) {
	iSec = 11;
	iPhi = iphi2 - 13; // 1,2
      } else if (iphi2 >= 16 && iphi2 <= 18) {
	iSec = 13;
	iPhi = iphi2 - 16; // 0,1,2
      } else if (iphi2 >= 19 && iphi2 <= 20) {
	iSec = 15;
	iPhi = iphi2 - 19 + 1; // 1,2
      }
      iEta = 1;
    }
  }
  initChamber(iSide,iSec,iM,iPhi,iEta);
}



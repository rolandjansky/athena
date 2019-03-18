/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *********************************************************************
//
// NAME:        EFMissingETHelper.cxx
// PACKAGE:     Trigger/TrigAlgorithms/TrigEFMissingET
//
// AUTHOR:      Diego Casadei
// CREATED:     May 6, 2008
//
// Description: Transient-only helper class (companion to TrigMissingET)
// *********************************************************************


#include "TrigEFMissingET/EFMissingETHelper.h"

#include <iostream>
#include <cstdio>
#include <cmath>


TrigEFMissingEtComponent::TrigEFMissingEtComponent(){
  std::snprintf(m_name,10,"--empty--");
  m_skip=false;
  m_status=0;
  m_calib0=0; 
  m_calib1=1; 
  Reset();
}

//---

void TrigEFMissingEtComponent::Reset(){
  m_status=0;
  m_usedChannels=0;
  m_sumOfSigns=0; 
  m_ex = m_ey = m_ez = m_sumEt = m_sumBadEt = m_sumE = 0;
  m_minTime = m_maxTime = 0;
  m_maxE = m_minE = 0;
  m_maxQlty = 0;
}

//---

std::string TrigEFMissingEtComponent::getFormattedHeader(){
  std:: string s="__name____skip_status__usedCh._sumOfSigns__calib1_calib0/MeV";
  s+="___Ex/MeV_____Ey/MeV_____Ez/MeV___SumE/MeV__SumEt/MeV";
  s+="__tMin/ns__tMax/ns__minE/MeV__maxE/MeV__maxQlty__sumBadEt/MeV";
  return s;
}

//---

std::string TrigEFMissingEtComponent::getFormattedValues(){
  char buff[1024];
  const char* c="N";
  if (m_skip) c="Y";
  std::snprintf(buff,1024,
      "%s   %s 0x%04x %8d %11d %7.2f %8.2f %10.2f %10.2f %10.2f %10.2f %10.2f %8.2f %8.2f %9.1f %9.1f %8.1f %10.2f", 
      m_name, c,
      (unsigned short) m_status, m_usedChannels, m_sumOfSigns, m_calib1, m_calib0,
      m_ex, m_ey, m_ez, m_sumE, m_sumEt,
      m_minTime, m_maxTime, m_minE, m_maxE, m_maxQlty, m_sumBadEt);
  return std::string(buff);
}

std::string TrigEFMissingEtComponent::ComponentToName(const Component c)
{
  std::string name;
  switch(c)
  {
    // 00-03 LAr barrel 
    case PreSamplB:  name="PreSamplB";    break;
    case EMB1:       name="EMB1";         break;
    case EMB2:       name="EMB2";         break;
    case EMB3:       name="EMB3";         break;

    // 04-07 LAr EM endcap 
    case PreSamplE:  name="PreSamplE";    break;
    case EME1:       name="EME1";         break;
    case EME2:       name="EME2";         break;
    case EME3:       name="EME3";         break;

    // 08-11 Hadronic end cap cal.
    case HEC0:       name="HEC0";         break;
    case HEC1:       name="HEC1";         break;
    case HEC2:       name="HEC2";         break;
    case HEC3:       name="HEC3";         break;

    // 12-14 Tile barrel
    case TileBar0:   name="TileBar0";     break;
    case TileBar1:   name="TileBar1";     break;
    case TileBar2:   name="TileBar2";     break;

    // 15-17 Tile gap (ITC & scint)  
    case TileGap1:   name="TileGap1";     break;
    case TileGap2:   name="TileGap2";     break;
    case TileGap3:   name="TileGap3";     break;

    // 18-20 Tile extended barrel 
    case TileExt0:   name="TileExt0";     break;
    case TileExt1:   name="TileExt1";     break;
    case TileExt2:   name="TileExt2";     break;

    // 21-23 Forward cal endcap  
    case FCalEM:     name="FCalEM";       break;
    case FCalHad1:   name="FCalHad1";     break;
    case FCalHad2:   name="FCalHad2";     break;

    // 24    Topo. clusters Had  
    case TCLCW:      name="TCLCW";        break;

    // 25-26 pos. and neg. eta barrel                         
    case TCLCWB1:    name="TCLCWB1";      break;
    case TCLCWB2:    name="TCLCWB2";      break;

    // 27-28 pos. and neg. eta endcap             
    case TCLCWE1:    name="TCLCWE1";      break;
    case TCLCWE2:    name="TCLCWE2";      break;

    // 29    Topo. clusters EM             
    case TCEM:       name="TCEM";         break;

    // 30-31 pos. and neg. eta barrel                         
    case TCEMB1:     name="TCEMB1";       break;
    case TCEMB2:     name="TCEMB2";       break;

    // 32-33 pos. and neg. eta endcap              
    case TCEME1:     name="TCEME1";       break;
    case TCEME2:     name="TCEME2";       break;

    // 34-38 Jet              
    case JET:        name="JET";          break;
    case JETB1:      name="JETB1";        break;
    case JETB2:      name="JETB2";        break;
    case JETE1:      name="JETE1";        break;
    case JETE2:      name="JETE2";        break;

    // 39    Topo. cluster Fit              
    case TCPUC:      name="TCPUC";        break;

    // 40    Topo. cluster Fit -- uncorrected                         
    case TCPUCUnc:   name="TCPUCUnc";     break;

    // 41    Muons                         
    case Muons:      name="Muons";        break;

    default:         name="Unknown";
  }
  return name;                                    
}

/////////////////////////////////////////////////////////////////////


TrigEFMissingEtHelper::TrigEFMissingEtHelper(unsigned char len){
  m_status=0;
  m_ex=m_ey=m_ez=m_sumEt=m_sumE=0;

  len>REASONABLE_MAX ? m_elements=REASONABLE_MAX : m_elements=len;
  m_vecOfComponents.resize(m_elements);

  // Initialize component names and calibration constants (c0, c1) = (0, 1)
  for (unsigned char i=0; i<m_elements; ++i){
    if(m_elements==42){ // finest granularity
      std::snprintf(m_vecOfComponents[i].m_name,10, "%s", TrigEFMissingEtComponent::ComponentToName(i).c_str());
      m_vecOfComponents[i].m_calib0 = 0.0;
      m_vecOfComponents[i].m_calib1 = 1.0;
    } else {
      std::snprintf(m_vecOfComponents[i].m_name,10, "comp%03d  ", i);
      m_vecOfComponents[i].m_calib0 = 0;
      m_vecOfComponents[i].m_calib1 = 1;
    }
  }

  Reset();
}

//---

void TrigEFMissingEtHelper::Reset(){
  m_ex=m_ey=m_ez=m_sumEt=m_sumE=0;
  m_status=0;
  for (unsigned char i=0; i<m_elements; ++i){
    m_vecOfComponents[i].Reset();
    if (m_vecOfComponents[i].m_skip) 
      m_vecOfComponents[i].m_status=1;
    else
      m_vecOfComponents[i].m_status=0;
  }
}

//---

TrigEFMissingEtComponent* TrigEFMissingEtHelper::GetComponent(unsigned char c){
  if (c<m_elements)
    return &(m_vecOfComponents[c]);
  else
    return 0;
}

//---

std::string TrigEFMissingEtHelper::getFormattedValues(){
  char buff[256];
  std::snprintf(buff,256,"status = 0x%08x,\telements = %d", (unsigned)m_status, (int)m_elements);
  std::string s(buff);
  if (m_elements==0) return s; // basic info only

  s += ":\n\t\t";
  s += m_vecOfComponents[0].getFormattedHeader();
  for (unsigned char i=0; i<m_elements; ++i){
    s += "\n\t\t";
    s += m_vecOfComponents[i].getFormattedValues();
  }
  return s;
}

//---

MsgStream& operator<<(MsgStream& out, TrigEFMissingEtHelper& obj){
  std::string s;
  std::string start("REGTEST: (EF) ");
  char buff[380];

  std::snprintf(buff,sizeof(buff),"Ex    = %10.2f MeV", obj.m_ex);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"Ey    = %10.2f MeV", obj.m_ey);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"Ez    = %10.2f MeV", obj.m_ez);
  s = start;
  s += buff;
  out << s << endmsg;

  float et = std::sqrt(obj.m_ex*obj.m_ex+obj.m_ey*obj.m_ey);
  std::snprintf(buff,sizeof(buff),"Et    = %10.2f MeV", et);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"SumEt = %10.2f MeV", obj.m_sumEt);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"SumE  = %10.2f MeV", obj.m_sumE);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"Event status = 0x%08x;   auxiliary components = %4d", 
	       (unsigned)obj.m_status, (int)obj.m_elements);
  s = start;
  s += buff;
  out << s << endmsg;

  if (obj.m_elements==0) return out; // basic info only

  s = start;
  s += obj.m_vecOfComponents[0].getFormattedHeader();
  out << s << endmsg;

  for (unsigned char i=0; i<obj.m_elements; ++i){
    s = start;
    s += obj.m_vecOfComponents[i].getFormattedValues();
    out << s << endmsg;
  }

  return out;
}

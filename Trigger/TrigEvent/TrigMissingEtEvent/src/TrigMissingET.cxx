/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingEtComponent.h"

#include <string>
#include <stdio.h>
#include <string.h>
#include <cmath>

TrigMissingET::TrigMissingET() :
  m_ex(0), m_ey(0), m_ez(0), m_sum_et(0), m_sum_e(0),
  m_flag(0), m_roiWord(0)
{
}


TrigMissingET::TrigMissingET(unsigned char n) :
  m_ex(0), m_ey(0), m_ez(0), m_sum_et(0), m_sum_e(0),
  m_flag(0), m_roiWord(0)
{
  /// definition of auxiliary information
  // 1. muons + all calo samplings in barrel + calo samplings in both end-caps
  char names25[25][6] = {"EMB0", "EMB1", "EMB2", "EMB3",
			 "EME0", "EME1", "EME2", "EME3",
			 "HEC0", "HEC1", "HEC2", "HEC3",
			 "TiB0", "TiB1", "TiB2",
			 "TiG1", "TiG2", "TiG3",
			 "TiE0", "TiE1", "TiE2",
			 "FCa0", "FCa1", "FCa2",
			 "Muon"};

  // 2. only muons
  char names1[1][6] = {"Muon"};

  // 3. muons + sum of all EM + sum of all HAD
  char names3[3][6] = {"Muon", "E.M.", "Had."};

  // 4. muons + EM_barrel + EM_endcaps + HAD_barrel + HAD_endcaps
  char names5[5][6] = {"Muon", "EM_B", "EM_E", "HadB", "HadE"};

  if (n>0) m_compVec.reserve(n);
  for (int i=0; i<n; ++i) {
    TrigMissingEtComponent c;
    if (n==25) {
      c.setName(names25[i]);
    } else if (n==1) {
      c.setName(names1[i]);
    } else if (n==3) {
      c.setName(names3[i]);
    } else if (n==5) {
      c.setName(names5[i]);
    } else {
      c.setName("????");
    }
    m_compVec.push_back(c);
  }
}


TrigMissingET::TrigMissingET(std::vector< TrigMissingEtComponent >&& compVec)
  : m_ex(0), m_ey(0), m_ez(0), m_sum_et(0), m_sum_e(0),
    m_flag(0), m_roiWord(0),
    m_compVec (std::move(compVec))
{
}


TrigMissingET::~TrigMissingET(){}


void TrigMissingET::clear()
{

  m_ex=0;
  m_ey=0;
  m_ez=0;
  m_sum_et=0;
  m_sum_e=0;
  m_flag=0;

  for (unsigned int i=0; i<m_compVec.size(); ++i) {
    m_compVec[i].reset();
  }

}


std::string TrigMissingET::getNameOfComponent(unsigned int index) const
{
  if (index < m_compVec.size())
    return m_compVec[index].name();
  else
    return std::string("WRONG INDEX!");
}


float TrigMissingET::getExComponent(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].ex();
  else
    return 0; //<! safe but silent!
}


float TrigMissingET::getEyComponent(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].ey();
  else
    return 0; //<! safe but silent!
}


float TrigMissingET::getEzComponent(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].ez();
  else
    return 0; //<! safe but silent!
}


float TrigMissingET::getSumEtComponent(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].sumEt();
  else
    return 0; //<! safe but silent!
}


float TrigMissingET::getSumEComponent(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].sumE();
  else
    return 0; //<! safe but silent!
}


short TrigMissingET::getSumOfSigns(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].sumOfSigns();
  else
    return 0; //<! safe but silent!
}


unsigned short TrigMissingET::getUsedChannels(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].usedChannels();
  else
    return 0; //<! safe but silent!
}


short TrigMissingET::getStatus(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].status();
  else
    return 0; //<! safe but silent!
}


float TrigMissingET::getComponentCalib0(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].calib0();
  else
    return 0; //<! safe but silent!
}


float TrigMissingET::getComponentCalib1(unsigned int index) const{
  if (index < m_compVec.size())
    return m_compVec[index].calib1();
  else
    return 0; //<! safe but silent!
}




void TrigMissingET::setNameOfComponent(unsigned int index, const char* str){
  if (index < m_compVec.size()) m_compVec[index].setName(str);
}


void TrigMissingET::setExComponent(unsigned int index, float value){
  if (index < m_compVec.size()) m_compVec[index].setEx(value);
}


void TrigMissingET::setEyComponent(unsigned int index, float value){
  if (index < m_compVec.size()) m_compVec[index].setEy(value);
}


void TrigMissingET::setEzComponent(unsigned int index, float value){
  if (index < m_compVec.size()) m_compVec[index].setEz(value);
}


void TrigMissingET::setSumEtComponent(unsigned int index, float value){
  if (index < m_compVec.size()) m_compVec[index].setSumEt(value);
}


void TrigMissingET::setSumEComponent(unsigned int index, float value){
  if (index < m_compVec.size()) m_compVec[index].setSumE(value);
}


void TrigMissingET::setComponentCalib0(unsigned int index, float value){
  if (index < m_compVec.size()) m_compVec[index].setCalib0(value);
}


void TrigMissingET::setComponentCalib1(unsigned int index, float value){
  if (index < m_compVec.size()) m_compVec[index].setCalib1(value);
}


void TrigMissingET::setSumOfSigns(unsigned int index, short value){
  if (index < m_compVec.size()) m_compVec[index].setSumOfSigns(value);
}


void TrigMissingET::setUsedChannels(unsigned int index, unsigned short value){
  if (index < m_compVec.size()) m_compVec[index].setUsedChannels(value);
}


void TrigMissingET::setStatus(unsigned int index, short value){
  if (index < m_compVec.size()) m_compVec[index].setStatus(value);
}





bool operator== ( const TrigMissingET& a, const TrigMissingET& b ) {

   const double DELTA=1e-3; // distance used to compare floats

   if( a.getNumOfComponents() != b.getNumOfComponents() )
      return false;
   if( a.getFlag() != b.getFlag() )
      return false;
   if( a.RoIword() != b.RoIword() )
      return false;

   if( fabsf(a.ex() - b.ex()) > DELTA )
      return false;
   if( fabsf(a.ey() - b.ey()) > DELTA )
      return false;
   if( fabsf(a.ez() - b.ez()) > DELTA )
      return false;
   if( fabsf(a.sumEt() - b.sumEt()) > DELTA )
      return false;
   if( fabsf(a.sumE() - b.sumE()) > DELTA )
      return false;

   for (unsigned u=0; u<a.getNumOfComponents(); ++u) {
     if( a.getNameOfComponent(u) != b.getNameOfComponent(u) )
       return false;
     if( a.getStatus(u) != b.getStatus(u) )
       return false;
     if( a.getSumOfSigns(u) != b.getSumOfSigns(u) )
       return false;
     if( a.getUsedChannels(u) != b.getUsedChannels(u) )
       return false;
     if( fabsf(a.getExComponent(u) - b.getExComponent(u)) > DELTA )
       return false;
     if( fabsf(a.getEyComponent(u) - b.getEyComponent(u)) > DELTA )
       return false;
     if( fabsf(a.getEzComponent(u) - b.getEzComponent(u)) > DELTA )
       return false;
     if( fabsf(a.getSumEtComponent(u) - b.getSumEtComponent(u)) > DELTA )
       return false;
     if( fabsf(a.getSumEComponent(u) - b.getSumEComponent(u)) > DELTA )
       return false;
     if( fabsf(a.getComponentCalib0(u) - b.getComponentCalib0(u)) > DELTA )
       return false;
     if( fabsf(a.getComponentCalib1(u) - b.getComponentCalib1(u)) > DELTA )
       return false;
   }

   return true;
}


std::string str (const TrigMissingET& a){
  std::string s="";
  char buff[1024];

  std::sprintf(buff,"MEx   = %10.2f MeV\n", a.ex());    s += buff;
  std::sprintf(buff,"MEy   = %10.2f MeV\n", a.ey());    s += buff;
  std::sprintf(buff,"MEz   = %10.2f MeV\n", a.ez());    s += buff;

  float et = std::sqrt(a.ex()*a.ex()+a.ey()*a.ey());
  std::sprintf(buff,"MEt   = %10.2f MeV\n", et);        s += buff;
  std::sprintf(buff,"SumEt = %10.2f MeV\n", a.sumEt()); s += buff;
  std::sprintf(buff,"SumE  = %10.2f MeV\n", a.sumE());  s += buff;

  std::sprintf(buff,"Event status = 0x%08x\n", (unsigned)a.getFlag());  s += buff;
  unsigned int N = a.getNumOfComponents();
  std::sprintf(buff,"Auxiliary components = %4d\n", N); s += buff;

  if (N==0) return s; // basic info only

  // header
  s += "__name_____status__usedCh.__sumOfSigns__calib1_calib0";
  s += "/MeV___Ex/MeV_____Ey/MeV_____Ez/MeV___SumE/MeV__SumEt/MeV\n";

  for (unsigned int i=0; i<N; ++i){
    std::sprintf(buff,
      "%10s 0x%04x %8d %11d %7.2f %8.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n", 
		 a.getNameOfComponent(i).c_str(), 
		 (unsigned)a.getStatus(i),
		 a.getUsedChannels(i),
		 a.getSumOfSigns(i),
		 a.getComponentCalib1(i),
		 a.getComponentCalib0(i),
		 a.getExComponent(i),
		 a.getEyComponent(i),
		 a.getEzComponent(i),
		 a.getSumEComponent(i),
		 a.getSumEtComponent(i));
    s += buff;
  }

  return s;
}


MsgStream& operator<< (MsgStream& m, const TrigMissingET& d) {
  return ( m << str( d ) );
}



void diff(const TrigMissingET& a, const TrigMissingET& b,
	  std::map<std::string, double>& variableChange){

  const double DELTA=1e-3; // distance used to compare floats

  int Na = a.getNumOfComponents();
  int Nb = b.getNumOfComponents();
  if( Na != Nb )
    variableChange[ "NumOfComponents" ] = Na - Nb;

  unsigned aFlag = (unsigned) a.getFlag();
  unsigned bFlag = (unsigned) b.getFlag();
  if( aFlag != bFlag )
    variableChange[ "FlagsXOR" ] = aFlag ^ bFlag;

  unsigned long aRoI = (unsigned long) a.RoIword();
  unsigned long bRoI = (unsigned long) b.RoIword();
  if( aRoI != bRoI )
    variableChange[ "RoIwordsXOR" ] = aRoI ^ bRoI;

  float aEx = a.ex();
  float bEx = b.ex();
  if( fabsf(aEx - bEx) > DELTA )
     variableChange[ "Ex" ] = aEx - bEx;

  float aEy = a.ey();
  float bEy = b.ey();
  if( fabsf(aEy - bEy) > DELTA )
     variableChange[ "Ey" ] = aEy - bEy;

  float aEz = a.ez();
  float bEz = b.ez();
  if( fabsf(aEz - bEz) > DELTA )
     variableChange[ "Ez" ] = aEz - bEz;

  float aSumEt = a.sumEt();
  float bSumEt = b.sumEt();
  if( fabsf(aSumEt - bSumEt) > DELTA )
     variableChange[ "sumEt" ] = aSumEt - bSumEt;

  float aSumE = a.sumE();
  float bSumE = b.sumE();
  if( fabsf(aSumE - bSumE) > DELTA )
     variableChange[ "sumE" ] = aSumE - bSumE;

  for (unsigned u=0; u<a.getNumOfComponents(); ++u) {
    char num[4];
    std::sprintf(num, "%02d", u);
    std::string key;

    if( a.getNameOfComponent(u) != b.getNameOfComponent(u) ) {
      key="CompNameStrCmp";  key+=num;
      variableChange[ key.c_str() ] = 
	strcmp(a.getNameOfComponent(u).c_str(),
	       b.getNameOfComponent(u).c_str());
    }

    unsigned aFlag = (unsigned) a.getStatus(u);
    unsigned bFlag = (unsigned) b.getStatus(u);
    if( aFlag != bFlag ) {
      key="CompStatusXOR";  key+=num;
      variableChange[ key.c_str() ] = aFlag ^ bFlag;
    }

    int aSigns = a.getSumOfSigns(u);
    int bSigns = b.getSumOfSigns(u);
    if( aSigns != bSigns ) {
      key="CompSumSigns";  key+=num;
      variableChange[ key.c_str() ] = aSigns - bSigns;
    }

    int aChans = a.getUsedChannels(u);
    int bChans = b.getUsedChannels(u);
    if( aChans != bChans ) {
      key="CompUsedChans";  key+=num;
      variableChange[ key.c_str() ] = aChans - bChans;
    }

    float aEx = a.getExComponent(u);
    float bEx = b.getExComponent(u);
    if( fabsf(aEx - bEx) > DELTA ) {
      key="CompEx";  key+=num;
      variableChange[ key.c_str() ] = aEx - bEx;
    }

    float aEy = a.getEyComponent(u);
    float bEy = b.getEyComponent(u);
    if( fabsf(aEy - bEy) > DELTA ) {
      key="CompEy";  key+=num;
      variableChange[ key.c_str() ] = aEy - bEy;
    }

    float aEz = a.getEzComponent(u);
    float bEz = b.getEzComponent(u);
    if( fabsf(aEz - bEz) > DELTA ) {
      key="CompEz";  key+=num;
      variableChange[ key.c_str() ] = aEz - bEz;
    }

    float aSumEt = a.getSumEtComponent(u);
    float bSumEt = b.getSumEtComponent(u);
    if( fabsf(aSumEt - bSumEt) > DELTA ) {
      key="CompSumEt";  key+=num;
      variableChange[ key.c_str() ] = aSumEt - bSumEt;
    }

    float aSumE = a.getSumEComponent(u);
    float bSumE = b.getSumEComponent(u);
    if( fabsf(aSumE - bSumE) > DELTA ) {
      key="CompSumE";  key+=num;
      variableChange[ key.c_str() ] = aSumE - bSumE;
    }

    float aCalib0 = a.getComponentCalib0(u);
    float bCalib0 = b.getComponentCalib0(u);
    if( fabsf(aCalib0 - bCalib0) > DELTA ) {
      key="CompCalib0_";  key+=num;
      variableChange[ key.c_str() ] = aCalib0 - bCalib0;
    }

    float aCalib1 = a.getComponentCalib1(u);
    float bCalib1 = b.getComponentCalib1(u);
    if( fabsf(aCalib1 - bCalib1) > DELTA ) {
      key="CompCalib1_";  key+=num;
      variableChange[ key.c_str() ] = aCalib1 - bCalib1;
    }
  }

  return;
}


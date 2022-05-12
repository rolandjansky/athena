/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMaps/HTTSectorMap.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <AsgMessaging/MessageCheck.h>

using namespace std;
using namespace asg::msgUserCode;

std::map<int,int> HTTSectorMap::makeLookup(const char* fname)
{
  std::map<int,int> secLookup;
  ifstream inFile(fname);

  if (inFile.is_open()) {
    ANA_MSG_FATAL("Unable to open file");
    return secLookup;
  }
  int evt,reg;
  while (inFile >> evt >> reg) {
    secLookup[evt]=reg;
  }
  inFile.close();
  return secLookup;
}

void HTTSectorMap::SetSector(int sec1, int sec2, int sec) {
  if (m_data.find(sec1)==m_data.end()) m_data[sec1] = mapint();
  m_data[sec1][sec2]=sec;
}

int HTTSectorMap::GetSector(int sec1, int sec2) {
  if (m_data.find(sec1)!=m_data.end())
    if (m_data[sec1].find(sec2)!=m_data[sec1].end())
      return m_data[sec1][sec2];
  return -1;
}

void HTTSectorMap::LoadFromFile(const char *fname) {

  ifstream inFile(fname);
  if (inFile.is_open()) {
    ANA_MSG_FATAL("Unable to open file");
    return;
  }    
  ANA_MSG_DEBUG("Reading sector map data: " << fname);
  int sec1,sec2,sec3;
  while(inFile >> sec1 >> sec2 >> sec3) SetSector(sec1,sec2,sec3);
  inFile.close();
}

void HTTSectorMap::CreateFile(const char *fname4,const char *fname8, const char *fname11) {
  std::map<int,int> map4=HTTSectorMap::makeLookup(fname4);
  std::map<int,int> map8=HTTSectorMap::makeLookup(fname8);

  ifstream inFile(fname11);
  if (inFile.is_open()) {
    ANA_MSG_FATAL("Unable to open file");
    return;
  }    
  int ev11, sec11;
  while(inFile >> ev11 >> sec11) {
    if(map4.find(ev11)!=map4.end() && map8.find(ev11)!=map8.end())
      SetSector(map4[ev11], map8[ev11], sec11);
  }
  inFile.close();
}

void HTTSectorMap::Dump() {
  for (mapint2::iterator i = m_data.begin(); i != m_data.end();i++) {
    for (mapint::iterator j = (*i).second.begin(); j != (*i).second.end();j++) {
      ANA_MSG_DEBUG((*i).first << " " << (*j).first << " " << (*j).second);
    }
  }
}

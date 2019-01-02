/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TGCcabling/TGCDatabaseInPP.h"

namespace LVL1TGCCabling8 
{

void TGCDatabaseInPP::readDB (void) {
  std::ifstream file(m_filename.c_str());
  std::string buf;

  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    m_maxIndexIn[iIndexIn] = 0; 
    m_minIndexIn[iIndexIn] = 9999; 
  }

  while(getline(file,buf)){
    if(buf.substr(0,m_blockname.size())==m_blockname) break;
  }

  while(getline(file,buf)){
    if(buf.substr(0,1)=="E"||buf.substr(0,1)=="F") break;
    std::istringstream line(buf);
    std::vector<int> entry;
    for(int i=0; i<6; i++){
      int temp=-1;
      line >> temp; 
      entry.push_back(temp);

      if(IndexInMin<=i && i<=IndexInMax) {
	int j = i-IndexInMin;
	if(m_maxIndexIn[j]<temp) {
	  m_maxIndexIn[j] = temp;
	} 
	if(m_minIndexIn[j]>temp) {
	  m_minIndexIn[j] = temp;
	}
      }
    }
    for(int i=0; i<3; i++){
      int temp=-1;
      line >> temp; 
      if(temp<0) break;
      entry.push_back(temp);
    }
    m_database.push_back(entry);
  }

  file.close();

  makeIndexDBIn(); 
}

void TGCDatabaseInPP::makeIndexDBIn (void) 
{
  m_NIndexDBIn = 1;
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    m_NIndexDBIn *= (m_maxIndexIn[iIndexIn]-m_minIndexIn[iIndexIn]+1);
  }
  for(int iIndexDBIn=0; iIndexDBIn<m_NIndexDBIn; iIndexDBIn++) {
    m_indexDBIn.push_back(-1); 
  }

  const int size = m_database.size();
  for(int i=0; i<size; i++) {
    int tmpValIndexIn[NIndexIn];
    for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
      tmpValIndexIn[iIndexIn] = m_database.at(i).at(iIndexIn+IndexInMin);
    }
    m_indexDBIn.at(convertIndexDBIn(tmpValIndexIn)) = i;
  }
}

int TGCDatabaseInPP::convertIndexDBIn(int* indexIn) const 
{
  int converted = indexIn[0]-m_minIndexIn[0];  
  for(int iIndexIn=1; iIndexIn<NIndexIn; iIndexIn++) {
    converted *= (m_maxIndexIn[iIndexIn]-m_minIndexIn[iIndexIn]+1); 
    converted += indexIn[iIndexIn]-m_minIndexIn[iIndexIn];
  }
  return converted;
}

int TGCDatabaseInPP::getIndexDBIn(int* indexIn) 
{
  if(!indexIn) return -1;

  if(m_database.size()==0) readDB();
  
  int converted = convertIndexDBIn(indexIn);
  if(converted<0 || converted>=m_NIndexDBIn) return -1;

  return m_indexDBIn.at(converted);
}

} // end of namespace

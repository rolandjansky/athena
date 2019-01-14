/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabaseInPP.h"

#include <fstream>
#include <sstream>

namespace MuonTGC_Cabling 
{

TGCDatabaseInPP::TGCDatabaseInPP(std::string filename, std::string blockname) 
  : TGCDatabase(TGCDatabase::InPP, filename, blockname), m_NIndexDBIn(0) 
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    m_maxIndexIn[iIndexIn] = 0;
    m_minIndexIn[iIndexIn] = 9999;
  }

  // read out ascii file and fill database
  if(m_database.size()==0) readDB();
}

TGCDatabaseInPP::TGCDatabaseInPP(const TGCDatabaseInPP& right)
  : TGCDatabase(right)
{
  right.getindexDBVectorIn(m_indexDBIn);
  right.getNIndexDBIn(m_NIndexDBIn);
  right.getmaxIndexIn(m_maxIndexIn);
  right.getminIndexIn(m_minIndexIn);
}

TGCDatabaseInPP::~TGCDatabaseInPP(void)
{
}

bool TGCDatabaseInPP::update(const std::vector<int>& input)
{
  int ip = find(input);
  if(ip<0) return false;

  const unsigned int input_size = input.size();

  const unsigned int database_ip_size = m_database[ip].size();
  for(unsigned int i=3; i<database_ip_size; i++){
    if( i< input.size()){
      m_database[ip].at(i) = input.at(i);
    } else {
      m_database[ip].at(i) = -1;
    }
  }
  if(database_ip_size < input_size){
    for(unsigned int i=database_ip_size; i<input_size; i++){
      m_database[ip].push_back(input.at(i));
    }    
  }  
  return true;
}
 
int  TGCDatabaseInPP::find(const std::vector<int>& channel) const
{
  int index=-1;
  const unsigned int size = m_database.size();
  for(unsigned int i=0; i<size; i++){
    if(m_database[i].at(2) == channel.at(2) &&
       m_database[i].at(1) == channel.at(1) &&
       m_database[i].at(0) == channel.at(0)) {
      index = i;
      break;
    }
  }
  return index;
}

int TGCDatabaseInPP::getIndexDBIn(int* indexIn) 
{
  if(!indexIn) return -1;

  if(m_database.size()==0) readDB();
  
  int converted = convertIndexDBIn(indexIn);
  if(converted<0 || converted>=m_NIndexDBIn) return -1;
  
  return m_indexDBIn.at(converted);
}

void TGCDatabaseInPP::getindexDBVectorIn(std::vector<int>& tmpindexDBIn) const
{
  tmpindexDBIn = m_indexDBIn;
}

void TGCDatabaseInPP::getNIndexDBIn(int& tmpNIndexDBIn) const
{
  tmpNIndexDBIn = m_NIndexDBIn;
}

void TGCDatabaseInPP::getmaxIndexIn(int* tmpmaxIndexIn) const
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    tmpmaxIndexIn[iIndexIn] = m_maxIndexIn[iIndexIn];
  }
}

void TGCDatabaseInPP::getminIndexIn(int* tmpminIndexIn) const
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    tmpminIndexIn[iIndexIn] = m_minIndexIn[iIndexIn];
  }
}

void TGCDatabaseInPP::readDB(void) {
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

void TGCDatabaseInPP::makeIndexDBIn(void) {
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

} // end of namespace

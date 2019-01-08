/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabaseASDToPP.h"

#include <fstream>
#include <sstream>

namespace MuonTGC_Cabling 
{

const int TGCDatabaseASDToPP::IndexIn[NIndexIn] = {0, 1, 6}; 
const int TGCDatabaseASDToPP::ReverseIndexIn[DATABASESIZE] = {0, 1, -1, -1, -1, -1, 2, -1};  
const int TGCDatabaseASDToPP::IndexOut[NIndexOut] = {3, 4, 5}; 
const int TGCDatabaseASDToPP::ReverseIndexOut[DATABASESIZE] = {-1, -1, -1, 0, 1, 2, -1, -1};  

TGCDatabaseASDToPP::TGCDatabaseASDToPP(std::string filename, 
				       std::string blockname,
				       bool v_isCommon)
  : TGCDatabase(TGCDatabase::ASDToPP, filename, blockname), 
    m_NIndexDBIn(0), m_NIndexDBOut(0), m_isCommon(v_isCommon) 
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    m_maxIndexIn[iIndexIn] = 0;
    m_minIndexIn[iIndexIn] = 9999;
  }
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    m_maxIndexOut[iIndexOut] = 0;
    m_minIndexOut[iIndexOut] = 9999;
  }

  // read out ascii file and fill database
  if(m_database.size()==0) readDB();
}
  
TGCDatabaseASDToPP::TGCDatabaseASDToPP(const TGCDatabaseASDToPP& right, bool v_isCommon)
  : TGCDatabase(right), m_isCommon(v_isCommon)
{
  right.getindexDBVectorIn(m_indexDBIn);
  right.getNIndexDBIn(m_NIndexDBIn);
  right.getmaxIndexIn(m_maxIndexIn);
  right.getminIndexIn(m_minIndexIn);

  right.getindexDBVectorOut(m_indexDBOut);
  right.getNIndexDBOut(m_NIndexDBOut);
  right.getmaxIndexOut(m_maxIndexOut);
  right.getminIndexOut(m_minIndexOut);
}

TGCDatabaseASDToPP::~TGCDatabaseASDToPP(void)
{
}

bool TGCDatabaseASDToPP::update(const std::vector<int>& input)
{
  int ip = find(input);
  if(ip<0) return false;
 
  int tmpValIndexOut[NIndexOut];
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    tmpValIndexOut[iIndexOut] = input.at(IndexOut[iIndexOut]);
  }

  bool over_range = false;
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    if(m_maxIndexOut[iIndexOut]<tmpValIndexOut[iIndexOut]) {
      m_maxIndexOut[iIndexOut] = tmpValIndexOut[iIndexOut];
      over_range = true;
    }
    if(m_minIndexOut[iIndexOut]>tmpValIndexOut[iIndexOut]) {
      m_minIndexOut[iIndexOut] = tmpValIndexOut[iIndexOut];
      over_range = true;
    }
  }
  if(over_range) {
    m_indexDBOut.clear();
    makeIndexDBOut();
  }

  int converted = convertIndexDBOut(tmpValIndexOut);
  if(converted<0 || converted>=m_NIndexDBOut) return false;

  m_indexDBOut.at(converted) = ip;

  m_database[ip].at(3) = input.at(3);
  m_database[ip].at(4) = input.at(4);
  m_database[ip].at(5) = input.at(5);

  return true;
}
 
int  TGCDatabaseASDToPP::find(const std::vector<int>& channel) const
{
  int index=-1;
  const size_t size = m_database.size();
  for(size_t i=0; i<size; i++){
    if(m_database[i].at(2) == channel.at(2) &&
       m_database[i].at(1) == channel.at(1) &&
       m_database[i].at(0) == channel.at(0)) {
      index = i;
      break;
    }
  }
  return index;
}

int TGCDatabaseASDToPP::getIndexDBIn(int* indexIn) 
{
  if(!indexIn) return -1;

  if(m_database.size()==0) readDB();

  int converted = convertIndexDBIn(indexIn);
  if(converted<0 || converted>=m_NIndexDBIn) return -1;

  return m_indexDBIn.at(converted);
}

void TGCDatabaseASDToPP::getindexDBVectorIn(std::vector<int>& tmpindexDBIn) const
{
  tmpindexDBIn = m_indexDBIn;
}

void TGCDatabaseASDToPP::getNIndexDBIn(int& tmpNIndexDBIn) const
{
  tmpNIndexDBIn = m_NIndexDBIn;
}

void TGCDatabaseASDToPP::getmaxIndexIn(int* tmpmaxIndexIn) const
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    tmpmaxIndexIn[iIndexIn] = m_maxIndexIn[iIndexIn];
  }
}

void TGCDatabaseASDToPP::getminIndexIn(int* tmpminIndexIn) const
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    tmpminIndexIn[iIndexIn] = m_minIndexIn[iIndexIn];
  }
}

int TGCDatabaseASDToPP::getIndexDBOut(int* indexOut) 
{
  if(!indexOut) return -1;

  if(m_database.size()==0) readDB();

  int converted = convertIndexDBOut(indexOut);
  if(converted<0 || converted>=m_NIndexDBOut) return -1;

  return m_indexDBOut.at(converted);
}

void TGCDatabaseASDToPP::getindexDBVectorOut(std::vector<int>& tmpindexDBOut) const
{
  tmpindexDBOut = m_indexDBOut;
}

void TGCDatabaseASDToPP::getNIndexDBOut(int& tmpNIndexDBOut) const
{
  tmpNIndexDBOut = m_NIndexDBOut;
}

void TGCDatabaseASDToPP::getmaxIndexOut(int* tmpmaxIndexOut) const
{
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    tmpmaxIndexOut[iIndexOut] = m_maxIndexOut[iIndexOut];
  }
}

void TGCDatabaseASDToPP::getminIndexOut(int* tmpminIndexOut) const
{
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    tmpminIndexOut[iIndexOut] = m_minIndexOut[iIndexOut];
  }
}

bool TGCDatabaseASDToPP::isCommon() const  
{ 
  return m_isCommon; 
} 

void TGCDatabaseASDToPP::readDB(void) 
{
  std::ifstream file(m_filename.c_str());
  std::string buf;

  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    m_maxIndexIn[iIndexIn] = 0;
    m_minIndexIn[iIndexIn] = 9999;
  }
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    m_maxIndexOut[iIndexOut] = 0;
    m_minIndexOut[iIndexOut] = 9999;
  }

  while(getline(file,buf)){
    if(buf.substr(0,m_blockname.size())==m_blockname) break;
  }

  while(getline(file,buf)){
    if(buf.substr(0,1)=="E"||buf.substr(0,1)=="F") break;
    std::istringstream line(buf);
    std::vector<int> entry;
    for(int i=0; i<DATABASESIZE-2; i++){
      int temp=-1;
      line >> temp; 
      entry.push_back(temp);
    }
    m_database.push_back(entry);
  }

  file.close();

  int nline = 0;
  const unsigned int database_size = m_database.size();
  for(unsigned int i=0; i<database_size; i++){
    // line is defined in whole sector. [0..n]
    if(i>0&&m_database[i].at(0)!=m_database[i-1].at(0)) nline=0;
    m_database[i].push_back(nline++);

    if(i==database_size-1||
       m_database[i].at(0)!=m_database[i+1].at(0)||
       m_database[i].at(1)!=m_database[i+1].at(1)){
      // increase with R in chamber    [0..n]
      int totline = m_database[i].at(2)+1;
      for(int j=0; j<totline; j++){
	m_database[i-j].push_back(j);
      }
    }
  }
  
  
  for(unsigned int i=0; i<database_size; i++){ 
    for(int j=0; j<DATABASESIZE; j++){ 
      int temp = m_database[i].at(j);  
      int k = ReverseIndexOut[j]; 
      if(k>=0) { 
	if(m_maxIndexOut[k]<temp) { 
	  m_maxIndexOut[k] = temp; 
	}  
	if(m_minIndexOut[k]>temp) { 
	  m_minIndexOut[k] = temp; 
	} 
      }  
      
      k = ReverseIndexIn[j]; 
      if(k>=0) { 
	if(m_maxIndexIn[k]<temp) { 
	  m_maxIndexIn[k] = temp; 
	} 
	if(m_minIndexIn[k]>temp) { 
	  m_minIndexIn[k] = temp; 
	} 
      } 
    } 
  } 

  makeIndexDBIn();
  makeIndexDBOut();
}

void TGCDatabaseASDToPP::makeIndexDBIn(void) 
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
      tmpValIndexIn[iIndexIn] = m_database.at(i).at(IndexIn[iIndexIn]);
    }
    m_indexDBIn.at(convertIndexDBIn(tmpValIndexIn)) = i;
  }
}

int TGCDatabaseASDToPP::convertIndexDBIn(int* indexIn) const
{
  int converted = indexIn[0]-m_minIndexIn[0];
  for(int iIndexIn=1; iIndexIn<NIndexIn; iIndexIn++) {
    converted *= (m_maxIndexIn[iIndexIn]-m_minIndexIn[iIndexIn]+1);
    converted += indexIn[iIndexIn]-m_minIndexIn[iIndexIn];
  }
  return converted;
}

void TGCDatabaseASDToPP::makeIndexDBOut(void) 
{
  m_NIndexDBOut = 1;
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    m_NIndexDBOut *= (m_maxIndexOut[iIndexOut]-m_minIndexOut[iIndexOut]+1);
  }
  for(int iIndexDBOut=0; iIndexDBOut<m_NIndexDBOut; iIndexDBOut++) {
    m_indexDBOut.push_back(-1);
  }

  const int size = m_database.size();
  for(int i=0; i<size; i++) {
    int tmpValIndexOut[NIndexOut];
    for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
      tmpValIndexOut[iIndexOut] = m_database.at(i).at(IndexOut[iIndexOut]);
    }
    m_indexDBOut.at(convertIndexDBOut(tmpValIndexOut)) = i;
  }
}

int TGCDatabaseASDToPP::convertIndexDBOut(int* indexOut) const
{
  int converted = indexOut[0]-m_minIndexOut[0];
  for(int iIndexOut=1; iIndexOut<NIndexOut; iIndexOut++) {
    converted *= (m_maxIndexOut[iIndexOut]-m_minIndexOut[iIndexOut]+1);
    converted += indexOut[iIndexOut]-m_minIndexOut[iIndexOut];
  }
  return converted;
}

} // end of namespace

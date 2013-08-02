/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabaseInPP.h"

#include <fstream>
#include <sstream>

namespace MuonTGC_Cabling 
{

TGCDatabaseInPP::TGCDatabaseInPP(std::string filename, std::string blockname) 
  : TGCDatabase(TGCDatabase::InPP, filename, blockname), NIndexDBIn(0) 
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    maxIndexIn[iIndexIn] = 0;
    minIndexIn[iIndexIn] = 9999;
  }

  // read out ascii file and fill database
  if(database.size()==0) readDB();
}

TGCDatabaseInPP::TGCDatabaseInPP(const TGCDatabaseInPP& right)
  : TGCDatabase(right)
{
  right.getindexDBVectorIn(indexDBIn);
  right.getNIndexDBIn(NIndexDBIn);
  right.getmaxIndexIn(maxIndexIn);
  right.getminIndexIn(minIndexIn);
}

TGCDatabaseInPP::~TGCDatabaseInPP(void)
{
}

bool TGCDatabaseInPP::update(const std::vector<int>& input)
{
  int ip = find(input);
  if(ip<0) return false;

  const unsigned int input_size = input.size();

  const unsigned int database_ip_size = database[ip].size();
  for(unsigned int i=3; i<database_ip_size; i++){
    if( i< input.size()){
      database[ip].at(i) = input.at(i);
    } else {
      database[ip].at(i) = -1;
    }
  }
  if(database_ip_size < input_size){
    for(unsigned int i=database_ip_size; i<input_size; i++){
      database[ip].push_back(input.at(i));
    }    
  }  
  return true;
}
 
int  TGCDatabaseInPP::find(const std::vector<int>& channel) const
{
  int index=-1;
  const unsigned int size = database.size();
  for(unsigned int i=0; i<size; i++){
    if(database[i].at(2) == channel.at(2) &&
       database[i].at(1) == channel.at(1) &&
       database[i].at(0) == channel.at(0)) {
      index = i;
      break;
    }
  }
  return index;
}

int TGCDatabaseInPP::getIndexDBIn(int* indexIn) 
{
  if(!indexIn) return -1;

  if(database.size()==0) readDB();
  
  int converted = convertIndexDBIn(indexIn);
  if(converted<0 || converted>=NIndexDBIn) return -1;
  
  return indexDBIn.at(converted);
}

void TGCDatabaseInPP::getindexDBVectorIn(std::vector<int>& tmpindexDBIn) const
{
  tmpindexDBIn = indexDBIn;
}

void TGCDatabaseInPP::getNIndexDBIn(int& tmpNIndexDBIn) const
{
  tmpNIndexDBIn = NIndexDBIn;
}

void TGCDatabaseInPP::getmaxIndexIn(int* tmpmaxIndexIn) const
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    tmpmaxIndexIn[iIndexIn] = maxIndexIn[iIndexIn];
  }
}

void TGCDatabaseInPP::getminIndexIn(int* tmpminIndexIn) const
{
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    tmpminIndexIn[iIndexIn] = minIndexIn[iIndexIn];
  }
}

void TGCDatabaseInPP::readDB(void) {
  std::ifstream file(filename.c_str());
  std::string buf;

  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    maxIndexIn[iIndexIn] = 0;
    minIndexIn[iIndexIn] = 9999;
  }

  while(getline(file,buf)){
    if(buf.substr(0,blockname.size())==blockname) break;
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
        if(maxIndexIn[j]<temp) {
          maxIndexIn[j] = temp;
        } 
	if(minIndexIn[j]>temp) {
          minIndexIn[j] = temp;
        }
      }
    }
    for(int i=0; i<3; i++){
      int temp=-1;
      line >> temp; 
      if(temp<0) break;
      entry.push_back(temp);
    }
    database.push_back(entry);
  }

  file.close();

  makeIndexDBIn();
}

void TGCDatabaseInPP::makeIndexDBIn(void) {
  NIndexDBIn = 1;
  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    NIndexDBIn *= (maxIndexIn[iIndexIn]-minIndexIn[iIndexIn]+1);
  }
  for(int iIndexDBIn=0; iIndexDBIn<NIndexDBIn; iIndexDBIn++) {
    indexDBIn.push_back(-1);
  }
  
  const int size = database.size();
  for(int i=0; i<size; i++) {
    int tmpValIndexIn[NIndexIn];
    for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
      tmpValIndexIn[iIndexIn] = database.at(i).at(iIndexIn+IndexInMin);
    }
    indexDBIn.at(convertIndexDBIn(tmpValIndexIn)) = i;
  }
}

int TGCDatabaseInPP::convertIndexDBIn(int* indexIn) const
{
  int converted = indexIn[0]-minIndexIn[0];
  for(int iIndexIn=1; iIndexIn<NIndexIn; iIndexIn++) {
    converted *= (maxIndexIn[iIndexIn]-minIndexIn[iIndexIn]+1);
    converted += indexIn[iIndexIn]-minIndexIn[iIndexIn];
  }
  return converted;
}

} // end of namespace

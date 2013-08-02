/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TGCcabling/TGCDatabaseInPP.h"

namespace LVL1TGCCabling8 
{

void TGCDatabaseInPP::readDB (void) {
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

void TGCDatabaseInPP::makeIndexDBIn (void) 
{
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

int TGCDatabaseInPP::getIndexDBIn(int* indexIn) 
{
  if(!indexIn) return -1;

  if(database.size()==0) readDB();
  
  int converted = convertIndexDBIn(indexIn);
  if(converted<0 || converted>=NIndexDBIn) return -1;

  return indexDBIn.at(converted);
}

} // end of namespace

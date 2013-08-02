/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TGCcabling/TGCDatabaseASDToPP.h"

namespace LVL1TGCCabling8 
{

const int TGCDatabaseASDToPP::IndexIn[NIndexIn] = {0, 1, 6};
const int TGCDatabaseASDToPP::ReverseIndexIn[DATABASESIZE] = {0, 1, -1, -1, -1, -1, 2, -1}; 
const int TGCDatabaseASDToPP::IndexOut[NIndexOut] = {3, 4, 5};
const int TGCDatabaseASDToPP::ReverseIndexOut[DATABASESIZE] = {-1, -1, -1, 0, 1, 2, -1, -1}; 

void TGCDatabaseASDToPP::readDB (void) {
  std::ifstream file(filename.c_str());
  std::string buf;

  for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) {
    maxIndexIn[iIndexIn] = 0;
    minIndexIn[iIndexIn] = 9999;
  }
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    maxIndexOut[iIndexOut] = 0;
    minIndexOut[iIndexOut] = 9999;
  }

  while(getline(file,buf)){
    if(buf.substr(0,blockname.size())==blockname) break;
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
    database.push_back(entry);
  }
  file.close();

  int nline = 0;
  unsigned int size = database.size();
  for(unsigned int i=0; i<size; i++){
    // line is defined in whole sector. [0..n]
    if(i>0&&database[i].at(0)!=database[i-1].at(0)) nline=0;
    database[i].push_back(nline++);

    if(i==database.size()-1||
       database[i].at(0)!=database[i+1].at(0)||
       database[i].at(1)!=database[i+1].at(1)){
      // increase with R in chamber    [0..n]
      int totline = database[i].at(2)+1;
      for(int j=0; j<totline; j++){
	database[i-j].push_back(j);
      }
    }
  }

  for(unsigned int i=0; i<size; i++){
    for(int j=0; j<DATABASESIZE; j++){
      int temp = database[i].at(j); 
      int k = ReverseIndexOut[j];
      if(k>=0) {
        if(maxIndexOut[k]<temp) {
          maxIndexOut[k] = temp;
        } 
	if(minIndexOut[k]>temp) {
          minIndexOut[k] = temp;
        }
      } 

      k = ReverseIndexIn[j];
      if(k>=0) {
	if(maxIndexIn[k]<temp) {
          maxIndexIn[k] = temp;
        }
        if(minIndexIn[k]>temp) {
          minIndexIn[k] = temp;
        }
      }
    }
  }

  makeIndexDBIn();
  makeIndexDBOut();
}

void TGCDatabaseASDToPP::makeIndexDBIn (void) 
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
      tmpValIndexIn[iIndexIn] = database.at(i).at(IndexIn[iIndexIn]);
    }
    indexDBIn.at(convertIndexDBIn(tmpValIndexIn)) = i;
  }
}

int TGCDatabaseASDToPP::convertIndexDBIn(int* indexIn) const
{
  int converted = indexIn[0]-minIndexIn[0];  
  for(int iIndexIn=1; iIndexIn<NIndexIn; iIndexIn++) {
    converted *= (maxIndexIn[iIndexIn]-minIndexIn[iIndexIn]+1); 
    converted += indexIn[iIndexIn]-minIndexIn[iIndexIn];
  }
  return converted;
}

int TGCDatabaseASDToPP::getIndexDBIn(int* indexIn) {
  if(!indexIn) return -1;

  if(database.size()==0) readDB();
  
  int converted = convertIndexDBIn(indexIn);
  if(converted<0 || converted>=NIndexDBIn) return -1;

  return indexDBIn.at(converted);
}

void TGCDatabaseASDToPP::makeIndexDBOut (void) 
{
  NIndexDBOut = 1;
  for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
    NIndexDBOut *= (maxIndexOut[iIndexOut]-minIndexOut[iIndexOut]+1);
  }
  for(int iIndexDBOut=0; iIndexDBOut<NIndexDBOut; iIndexDBOut++) {
    indexDBOut.push_back(-1); 
  }

  const int size = database.size();
  for(int i=0; i<size; i++) {
    int tmpValIndexOut[NIndexOut];
    for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) {
      tmpValIndexOut[iIndexOut] = database.at(i).at(IndexOut[iIndexOut]);
    }
    indexDBOut.at(convertIndexDBOut(tmpValIndexOut)) = i;
  }
}

int TGCDatabaseASDToPP::convertIndexDBOut(int* indexOut) const
{
  int converted = indexOut[0]-minIndexOut[0];  
  for(int iIndexOut=1; iIndexOut<NIndexOut; iIndexOut++) {
    converted *= (maxIndexOut[iIndexOut]-minIndexOut[iIndexOut]+1); 
    converted += indexOut[iIndexOut]-minIndexOut[iIndexOut];
  }
  return converted;
}

int TGCDatabaseASDToPP::getIndexDBOut(int* indexOut) {
  if(!indexOut) return -1;

  if(database.size()==0) readDB();
  
  int converted = convertIndexDBOut(indexOut);
  if(converted<0 || converted>=NIndexDBOut) return -1;

  return indexDBOut.at(converted);
}

} // end of namespace

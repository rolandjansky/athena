/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////
// TRTStrawStatusData.cxx
////////////////////////////

#include "TRT_ConditionsData/TRTStrawStatusData.h"
#include "Identifier/Identifier.h"
#include <iostream>

//constructor
TRTStrawStatusData::TRTStrawStatusData()
{}

//destructor
TRTStrawStatusData::~TRTStrawStatusData()
{ }

//add map entries
void TRTStrawStatusData::setStatus(const Identifier offId, const int stat)
{

 m_statMap[offId]=stat;
 

}

int TRTStrawStatusData::findStatus(const Identifier offID) const
{
  int stat;


  statusMap::const_iterator it(m_statMap.find(offID));
  if (it == m_statMap.end()){
        stat = 0;
 return stat;   
  }
  else{
    stat = (*it).second;
    return stat;
 }


}

//scan the map
std::vector<Identifier>  TRTStrawStatusData::scan_entries(int stat) const
{
  //this scans the map for all entries with status = stat(1/2/3)
  //these channels are returned in a vector


  std::vector<Identifier> badChan;
  Identifier offID;
  int status;
  statusMap::const_iterator it;

  for(it=m_statMap.begin(); it!=m_statMap.end(); ++it){
   
    offID   = it->first;
    status  = it->second;
   
 if(status==stat){
      badChan.push_back(offID);
    }
  }
  return badChan;
}


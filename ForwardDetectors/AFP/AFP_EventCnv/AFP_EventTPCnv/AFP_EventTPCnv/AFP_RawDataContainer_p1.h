/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPATHENAPOOL_AFPCONTAINER_P1_H
#define AFPATHENAPOOL_AFPCONTAINER_P1_H
#include <vector>
#include "AFP_RawDataCollection_p1.h"
	
class AFP_RawDataContainer_p1: public std::vector<AFP_RawDataCollection_p1> 
{
	 
  uint32_t TimeStamp;
 //Time Stamp ns
 uint32_t TimeStampns;
  //Lumi Block ID
 uint32_t LumiBlock;
//Bunch Crossing ID
  uint32_t bcId;
// Lvl1 ID
  uint32_t lvl1Id;
 //Lvl1 trigger pattern (fired items in lvl1 trigger)
  std::vector<bool> lvl1Pattern;
       //Lvl2 trigger pattern (fired items in lvl2 trigger)
 std::vector<bool> lvl2Pattern;
        //EF trigger pattern (fired items in EF)
  std::vector<bool> efPattern;
 
 public:
	 
 AFP_RawDataContainer_p1()
 {
        TimeStamp=0;
        TimeStampns=0;
        LumiBlock=0;
        bcId=0;
 };
 
 friend class AFP_RawDataContainerCnv_p1;
 
 };
 
 #endif

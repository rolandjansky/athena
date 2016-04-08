/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFAATHENAPOOL_ALFACONTAINER_P1_H
#define ALFAATHENAPOOL_ALFACONTAINER_P1_H

#include <vector>
#include "ALFA_RawDataCollection_p1.h"

class ALFA_RawDataContainer_p1: public std::vector<ALFA_RawDataCollection_p1> 
{
  
  uint32_t TimeStamp;
  //Time Stamp ns
  uint32_t TimeStampns;
  //Lumi Block ID
  uint32_t LumiBlock;
  //Bunch Crossing ID
  uint32_t BCId;
  //Lvl1 trigger pattern (fired items in lvl1 trigger)
  std::vector<bool> lvl1Pattern;
	//Lvl2 trigger pattern (fired items in lvl2 trigger)
  std::vector<bool> lvl2Pattern;
	//EF trigger pattern (fired items in EF)
  std::vector<bool> efPattern;
  
 public:
 
 ALFA_RawDataContainer_p1()
 {
	TimeStamp=0;
	TimeStampns=0;
	LumiBlock=0;
	BCId=0;
 };
 
 friend class ALFA_RawDataContainerCnv_p1;
 
 };
 
 #endif

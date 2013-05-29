/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACOLLECTION_CHARGE_P1_H
#define ALFA_RAWDATACOLLECTION_CHARGE_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "ALFA_RawData_charge_p1.h"
#include <vector>
#include <string>
#include <inttypes.h>

class ALFA_RawDataCollection_charge_p1: public std::vector<ALFA_RawData_charge_p1> {
  
 public:
  friend class ALFA_RawDataCollectionCnv_charge_p1;
  ALFA_RawDataCollection_charge_p1():MBId_POT(0), EventCount_POT(0), TimeStamp_POT(0),BCId_POT(0){}
  
  
// private:
 
  
 //Motherboard id
uint16_t MBId_POT;
//Event counter
uint32_t EventCount_POT;
//Time Stamp
uint32_t TimeStamp_POT;
//Bunch Crossing ID
uint32_t BCId_POT;

 
 
};

#endif

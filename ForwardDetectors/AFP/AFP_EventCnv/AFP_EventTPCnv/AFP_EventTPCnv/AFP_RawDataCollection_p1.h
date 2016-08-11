/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACOLLECTION_P1_H
#define AFP_RAWDATACOLLECTION_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "AFP_RawData_p1.h"
#include <vector>
#include <string>
#include <inttypes.h>
	
class AFP_RawDataCollection_p1: public std::vector<AFP_RawData_p1> {
	 
 public:
   friend class AFP_RawDataCollectionCnv_p1;
   AFP_RawDataCollection_p1():
   L1Id(0),
   frontend_flag(0),
   patternId(0),
   BCIdId(0),
   header_numberId(0),
   ADC2Id(0),
   TrigSyncErrId(0)
      {}
    //Motherboard id
    uint16_t L1Id;
    //Event counter
    uint32_t frontend_flag;
    //pattern id
    std::vector<bool> patternId;
    //BCId id
    uint16_t BCIdId;
    //header_number id Chan0
    uint16_t header_numberId;
    //header_number id Chan1
    uint16_t ADC2Id;
    //Time Stamp
    //syncronization error of PMF 24 (trigger info)
    bool TrigSyncErrId;
 
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACOLLECTION_P1_H
#define ALFA_RAWDATACOLLECTION_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "ALFA_RawData_p1.h"
#include <vector>
#include <string>
#include <inttypes.h>

class ALFA_RawDataCollection_p1: public std::vector<ALFA_RawData_p1> {
  
 public:
  friend class ALFA_RawDataCollectionCnv_p1;
  ALFA_RawDataCollection_p1():
    MBId_POT(0),
    EventCount_POT(0),
    patternId(0),
    scalerId(0),
    ADC1Id(0),
    ADC2Id(0),
    TrigSyncErrId(0)
      {}

    //Motherboard id
    uint16_t MBId_POT;
    //Event counter
    uint32_t EventCount_POT;
    //pattern id
    std::vector<bool> patternId;
    //scaler id
    uint16_t scalerId;
    //ADC1 id Chan0
    uint16_t ADC1Id;
    //ADC1 id Chan1
    uint16_t ADC2Id;
    //Time Stamp
    //syncronization error of PMF 24 (trigger info)
    bool TrigSyncErrId;
 
};

#endif

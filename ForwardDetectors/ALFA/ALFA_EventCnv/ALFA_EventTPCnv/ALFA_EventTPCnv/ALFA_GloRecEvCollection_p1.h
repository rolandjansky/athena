/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecEvCollection_p1_h
#define ALFA_GloRecEvCollection_p1_h

#include "ALFA_EventTPCnv/ALFA_GloRecEvent_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>
#include <string>

class ALFA_GloRecEvCollection_p1: public std::vector<ALFA_GloRecEvent_p1> 
{   
  public:  
  ALFA_GloRecEvCollection_p1() {};
};

#endif

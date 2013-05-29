/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecEvCollection_p1_h
#define ALFA_LocRecEvCollection_p1_h

#include "ALFA_EventTPCnv/ALFA_LocRecEvent_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>
#include <string>

class ALFA_LocRecEvCollection_p1: public std::vector<ALFA_LocRecEvent_p1> 
{   
  public:  
  ALFA_LocRecEvCollection_p1() {};
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrEvCollection_p1_h
#define ALFA_LocRecCorrEvCollection_p1_h

#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvent_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>
#include <string>

class ALFA_LocRecCorrEvCollection_p1: public std::vector<ALFA_LocRecCorrEvent_p1> 
{   
  public:  
  ALFA_LocRecCorrEvCollection_p1() {};
};

#endif

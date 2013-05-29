/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODDigitCollection_p1_h
#define ALFA_ODDigitCollection_p1_h

#include "ALFA_EventTPCnv/ALFA_ODDigit_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>
#include <string>


class ALFA_ODDigitCollection_p1: public std::vector<ALFA_ODDigit_p1> 
{   
  public:  
  ALFA_ODDigitCollection_p1() {};
};

#endif // ALFA_DigitCollection_p1_h

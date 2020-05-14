/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLocRecoEvCollection_p1_h
#define AFP_TDLocRecoEvCollection_p1_h

#include "AFP_EventTPCnv/AFP_TDLocRecoEvent_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>
#include <string>

class AFP_TDLocRecoEvCollection_p1: public std::vector<AFP_TDLocRecoEvent_p1> 
{   
  public:  
  AFP_TDLocRecoEvCollection_p1() {};
};

#endif

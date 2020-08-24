/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLocRecoEvCollection_p1_h
#define AFP_SIDLocRecoEvCollection_p1_h

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvent_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>
#include <string>

class AFP_SIDLocRecoEvCollection_p1: public std::vector<AFP_SIDLocRecoEvent_p1> 
{   
  public:  
  AFP_SIDLocRecoEvCollection_p1() {};
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBSimEvent/LArG4H6WarmTCHit.h"



LArG4H6WarmTCHit::LArG4H6WarmTCHit(const LArG4H6WarmTCHit &right) 
{
 addr = right.addr;
 energy = right.energy;
}

LArG4H6WarmTCHit& LArG4H6WarmTCHit::operator=(const LArG4H6WarmTCHit &right)
{
 if (this != &right) {
   addr = right.addr;
   energy = right.energy;
 }
 return *this;
}

int LArG4H6WarmTCHit::operator==(const LArG4H6WarmTCHit &right)
{
 if(addr != right.addr) return 0;
 if(energy != right.energy) return 0;
 return 1;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBSimEvent/LArG4H6LeakHit.h"


LArG4H6LeakHit::LArG4H6LeakHit(int d, int cod, float ek, float et)
{
 dir = d;
 pcode = cod;
 ekin = ek;
 etot = et;
}

LArG4H6LeakHit::LArG4H6LeakHit(const LArG4H6LeakHit &right)
{
 dir = right.dir;
 pcode = right.pcode;
 ekin = right.ekin;
 etot = right.etot;
}

LArG4H6LeakHit& LArG4H6LeakHit::operator=(const LArG4H6LeakHit &right)
{
 if (this != &right) {
   dir = right.dir;
   pcode = right.pcode;
   ekin = right.ekin;
   etot = right.etot;
 }
 return *this;
}

int LArG4H6LeakHit::operator==(const LArG4H6LeakHit &right)
{
 if(dir != right.dir) return 0;
 if(pcode != right.pcode) return 0;
 if(ekin != right.ekin) return 0;
 if(etot != right.etot) return 0;
 return 1;
}

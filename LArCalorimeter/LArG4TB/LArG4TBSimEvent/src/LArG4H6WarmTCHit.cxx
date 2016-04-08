/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBSimEvent/LArG4H6WarmTCHit.h"



int LArG4H6WarmTCHit::operator==(const LArG4H6WarmTCHit &right)
{
 if(m_addr != right.m_addr) return 0;
 if(m_energy != right.m_energy) return 0;
 return 1;
}

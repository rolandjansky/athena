/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBSimEvent/LArG4H6LeakHit.h"


LArG4H6LeakHit::LArG4H6LeakHit()
  : m_ekin(0),
    m_etot(0),
    m_pcode(0),
    m_dir(0)
{
}


LArG4H6LeakHit::LArG4H6LeakHit(int d, int cod, float ek, float et)
  : m_ekin(ek),
    m_etot(et),
    m_pcode (cod),
    m_dir(d)
{
}

LArG4H6LeakHit::LArG4H6LeakHit(const LArG4H6LeakHit &right)
  : m_ekin (right.m_ekin),
    m_etot (right.m_etot),
    m_pcode (right.m_pcode),
    m_dir (right.m_dir)
{
}

LArG4H6LeakHit& LArG4H6LeakHit::operator=(const LArG4H6LeakHit &right)
{
 if (this != &right) {
   m_dir = right.m_dir;
   m_pcode = right.m_pcode;
   m_ekin = right.m_ekin;
   m_etot = right.m_etot;
 }
 return *this;
}

int LArG4H6LeakHit::operator==(const LArG4H6LeakHit &right)
{
 if(m_dir != right.m_dir) return 0;
 if(m_pcode != right.m_pcode) return 0;
 if(m_ekin != right.m_ekin) return 0;
 if(m_etot != right.m_etot) return 0;
 return 1;
}

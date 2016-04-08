/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBSimEvent/LArG4H6FrontHit.h"


LArG4H6FrontHit::LArG4H6FrontHit():m_edep(0.),m_pos(),m_scnum(-1),m_mwpcnum(-1),m_x(-1),m_y(-1),m_track_id(-1),m_pcode(0)
{
}

LArG4H6FrontHit::LArG4H6FrontHit(int sc, double ed)
{
 m_scnum = sc;  m_edep = ed;
 m_mwpcnum=-1;  m_x=-1;  m_y=-1;
 m_pcode = 0; m_pos=CLHEP::Hep3Vector(0.,0.,0.);
 m_track_id = -1;
}

LArG4H6FrontHit::LArG4H6FrontHit(int mw, int x1, int y1)
{
 m_edep = 0;
 m_mwpcnum = mw;  m_x = x1;  m_y = y1;
 m_scnum = -1;
 m_pcode = 0;
 m_pos=CLHEP::Hep3Vector(0.,0.,0.);
 m_track_id = -1;
}

LArG4H6FrontHit::LArG4H6FrontHit(const LArG4H6FrontHit &right)
  
{
 m_edep = right.m_edep;
 m_pos = right.m_pos;
 m_scnum = right.m_scnum;
 m_mwpcnum = right.m_mwpcnum;
 m_x = right.m_x;  
 m_y = right.m_y;
 m_pcode = right.m_pcode;
 m_track_id = right.m_track_id;
 m_pos = right.m_pos;
}

LArG4H6FrontHit& LArG4H6FrontHit::operator=(const LArG4H6FrontHit &right)
{
 if (this != &right) {
   m_edep = right.m_edep;
   m_pos = right.m_pos;
   m_scnum = right.m_scnum;
   m_mwpcnum = right.m_mwpcnum;
   m_x = right.m_x;
   m_y = right.m_y;
   m_pcode = right.m_pcode;
   m_track_id = right.m_track_id;
   m_pos = right.m_pos;
 }
 return *this;
}

int LArG4H6FrontHit::operator==(const LArG4H6FrontHit &right)
{
 if(m_edep != right.m_edep) return 0;
 if(m_pos != right.m_pos) return 0;
 if(m_scnum != right.m_scnum) return 0;
 if(m_mwpcnum != right.m_mwpcnum) return 0;
 if(m_x != right.m_x) return 0;
 if(m_y != right.m_y) return 0;
 if(m_pcode != right.m_pcode) return 0;
 if(m_track_id != right.m_track_id) return 0;
 if(m_pos != right.m_pos) return 0;
 return 1;
}


/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBSimEvent/LArG4H6FrontHit.h"


LArG4H6FrontHit::LArG4H6FrontHit():m_edep(0.),m_pos(),m_scnum(-1),m_mwpcnum(-1),m_x(-1),m_y(-1),m_track_id(-1),m_pcode(0)
{
}

LArG4H6FrontHit::LArG4H6FrontHit(int sc, double ed)
  : m_edep(ed),
    m_pos(0,0,0),
    m_scnum(sc),
    m_mwpcnum(-1),
    m_x(-1),
    m_y(-1),
    m_track_id(-1),
    m_pcode(0)
{
}

LArG4H6FrontHit::LArG4H6FrontHit(int mw, int x1, int y1)
  : m_edep(0),
    m_pos(0,0,0),
    m_scnum(-1),
    m_mwpcnum(mw),
    m_x(x1),
    m_y(y1),
    m_track_id(-1),
    m_pcode(0)
{
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


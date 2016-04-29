/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBHit.h"

TBHit::TBHit()
  : m_beamdetect (nullptr),
    m_x(0),
    m_y(0),
    m_z(0),
    m_ex(0),
    m_ey(0),
    m_ez(0),
    m_IsValid(false)
{
}



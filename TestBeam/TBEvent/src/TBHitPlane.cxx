/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBHitPlane.h"

TBHitPlane::TBHitPlane()
  : m_beamdetect(0),
    m_u(0),
    m_w(0),
    m_eu(0),
    m_ew(0),
    m_IsValid(false)
{
}


TBHitPlane::~TBHitPlane(){}

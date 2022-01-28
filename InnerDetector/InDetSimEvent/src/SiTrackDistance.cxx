/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/SiTrackDistance.h"

SiTrackDistance::SiTrackDistance()
{   
}

SiTrackDistance::SiTrackDistance(float tanTheta, float tanPhi, float zInit) :
    m_tanTheta(tanTheta),
    m_tanPhi(tanPhi),
    m_zInit(zInit)
{   
}

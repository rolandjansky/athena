/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETPCNV_H
#define TRACKPARTICLETPCNV_H

#include <vector>
//-----------------------------------------------------------------------------
//
// file:   TrackParticleTPCnvDict.h
//
//-----------------------------------------------------------------------------
#include "TrackParticleTPCnv/TrackParticleContainer_tlp1.h"
#include "TrackParticleTPCnv/TrackParticleContainer_tlp2.h"
#include "TrackParticleTPCnv/TrackParticleContainer_tlp3.h"

//-----------------------------------------------------------------------------
// TrkParticleBase
//-----------------------------------------------------------------------------
#include "TrackParticleTPCnv/Particle/TrackParticle_p1.h"
#include "TrackParticleTPCnv/Particle/TrackParticle_p2.h"
#include "TrackParticleTPCnv/Particle/TrackParticle_p3.h"
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p1.h"
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p2.h"

namespace TrackParticleTPCnvDict 
{
    struct tmp {
      std::vector< Rec::TrackParticleContainer_p1 >       m_v1;
      std::vector< Rec::TrackParticleContainer_p2 >       m_v12;
      std::vector< Rec::TrackParticle_p1 >                m_v2;
      std::vector< Rec::TrackParticle_p2 >                m_v3;
      std::vector< Rec::TrackParticle_p3 >                m_v4;
   };
}

#endif // TRACKPARTICLETPCNV_H

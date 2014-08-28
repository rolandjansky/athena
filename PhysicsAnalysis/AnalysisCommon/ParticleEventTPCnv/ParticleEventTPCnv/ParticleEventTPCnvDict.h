///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// File: ParticleEventTPCnv/ParticleEventTPCnvDict.h
/// Dictionary file for persistent representation(s) of AOD containers
/// Author: Sebastien Binet <binet@cern.ch>
/// Date:   December 2006

#ifndef PARTICLEEVENTTPCNV_PARTICLEEVENTTPCNVDICT_H
#define PARTICLEEVENTTPCNV_PARTICLEEVENTTPCNVDICT_H 1

#include <vector>
#include "ParticleEventTPCnv/ParticleLinks_p1.h"
#include "ParticleEventTPCnv/ParticleBase_p1.h"
#include "ParticleEventTPCnv/ParticleBase_p2.h"
#include "ParticleEventTPCnv/Neutrino_p1.h"
#include "ParticleEventTPCnv/NeutrinoContainer_p1.h"
#include "ParticleEventTPCnv/Neutrino_p2.h"
#include "ParticleEventTPCnv/NeutrinoContainer_p2.h"


#include "ParticleEventTPCnv/CompositeParticle_p1.h"
#include "ParticleEventTPCnv/CompositeParticleContainer_p1.h"

#include "ParticleEventTPCnv/ParticleShallowClone_p1.h"
#include "ParticleEventTPCnv/ParticleShallowCloneContainer_p1.h"

#include "ParticleEventTPCnv/TrackParticleAssocs_p1.h"

#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocs_p1.h"

namespace ParticleEventTPCnvDict_tmp {

  struct temp {

    // not strictly needed for this package, but in order to rationalize
    // creation of dicts. we put that here so it is not scattered over clients
    std::vector<ParticleBase_p1>     m_particles_v1;
    std::vector<ParticleBase_p2>     m_particles_v2;
    TrackParticleAssocs_p1 m_assocs_p1;
    TrackParticleAssocs_p1::Assocs_t m_assocs_assocs_p1;
    TrackParticleAssocs_p1::AssocElem_t m_assocs_elems_p1;

    INav4MomToTrackParticleAssocs_p1 m_inavtotpassocs_p1;
    INav4MomToTrackParticleAssocs_p1::Assocs_t m_inavtotpassocs_assocs_p1;
    INav4MomToTrackParticleAssocs_p1::AssocElem_t m_inavtotpassocs_elems_p1;

  };
}


#endif // PARTICLEEVENTTPCNV_PARTICLEEVENTTPCNVDICT_H

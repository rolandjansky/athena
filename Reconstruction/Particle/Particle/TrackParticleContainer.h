/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                     TrackParticleContainer.h  -  Description
                             -------------------
    begin   : 27-02-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes : 

 ***************************************************************************/

#ifndef PARTICLE_TRACKPARTICLECONTAINER_H
#define PARTICLE_TRACKPARTICLECONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "Particle/TrackParticle.h"
// #include "EventKernel/INavigable4Momentum.h"
// #include "NavFourMom/INavigable4MomentumCollection.h"

#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"

// DATAVECTOR_VIRTBASES1( Rec::TrackParticle, INavigable4Momentum );
DATAVECTOR_BASE( Rec::TrackParticle, Trk::TrackParticleBase );

namespace Rec
{
  class TrackParticleContainer : public DataVector<TrackParticle> 
  {
  public: 
    TrackParticleContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<TrackParticle>(own) {} 
    virtual ~TrackParticleContainer() {};
  };
}

// auto-symlink feature:
SG_BASE(Rec::TrackParticleContainer, DataVector<Rec::TrackParticle>);

CLASS_DEF( Rec::TrackParticleContainer, 1334834594, 1)

#endif



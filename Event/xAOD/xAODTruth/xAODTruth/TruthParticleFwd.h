// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthParticleFwd.h 574253 2013-12-06 15:21:49Z krasznaa $
#ifndef XAODTRUTH_TRUTHPARTICLEFWD_H
#define XAODTRUTH_TRUTHPARTICLEFWD_H

// This is a forward declaration for the latest truth particle type.
// We need it because TruthParticle needs to use TruthVertex, and
// TruthVertex also needs to use TruthParticle.

namespace xAOD {
   class TruthParticle_v1;
   typedef TruthParticle_v1 TruthParticle;
}

#endif // XAODTRUTH_TRUTHPARTICLEFWD_H

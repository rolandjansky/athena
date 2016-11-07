/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XEXAMPLE_XEXAMPLEUTIL_H
#define XEXAMPLE_XEXAMPLEUTIL_H

// TruthParticle utilities for xExample
// Use forward references for xAOD classes

#include <vector>

namespace xAOD{
  class TruthParticle_v1;
  typedef TruthParticle_v1 TruthParticle;
  class TruthVertex_v1;
  typedef TruthVertex_v1 TruthVertex;
}

// Long-lived B hadron (5x1,5xx2)
bool isBHadron(const xAOD::TruthParticle* p);

// Implement HepMC isGenStable
bool isGenStable(const xAOD::TruthParticle* p);

// Emulate HepMC descendant iterator
// Multiple particles can give same descendant (string/cluster)
// Remove Geant descendants
int getDescendants(const xAOD::TruthParticle* p,
                   std::vector< const xAOD::TruthParticle* >& dscd );


#endif //XEXAMPLE_XEXAMPLEUTIL_H

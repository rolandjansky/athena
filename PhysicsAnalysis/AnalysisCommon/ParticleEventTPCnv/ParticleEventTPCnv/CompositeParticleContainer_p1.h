///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleContainer_p1.h 
// Header file for class CompositeParticleContainer_p1
// Author: S.Binet<binet@cern.ch>
// Date:   April 2008
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_COMPOSITEPARTICLECONTAINER_P1_H 
#define PARTICLEEVENTTPCNV_COMPOSITEPARTICLECONTAINER_P1_H 

// STL includes
#include <vector>

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/CompositeParticle_p1.h"

class CompositeParticleContainer_p1 : public std::vector<CompositeParticle_p1>
{};

#endif //> PARTICLEEVENTTPCNV_COMPOSITEPARTICLECONTAINER_P1_H

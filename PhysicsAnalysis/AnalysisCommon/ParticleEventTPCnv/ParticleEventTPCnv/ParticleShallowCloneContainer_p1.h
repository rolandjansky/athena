///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneContainer_p1.h 
// Header file for class ParticleShallowCloneContainer_p1
// Author: Karsten Koeneke
// Date:   February 2013
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECONTAINER_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECONTAINER_P1_H 

// STL includes
#include <vector>

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowClone_p1.h"

class ParticleShallowCloneContainer_p1 : public std::vector<ParticleShallowClone_p1>
{};

#endif //> PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECONTAINER_P1_H

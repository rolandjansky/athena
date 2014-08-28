///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoContainer_p2.h 
// Header file for class NeutrinoContainer_p2
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_NEUTRINOCONTAINER_P2_H 
#define PARTICLEEVENTTPCNV_NEUTRINOCONTAINER_P2_H 1

// STL includes
#include <vector>

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/Neutrino_p2.h"

class NeutrinoContainer_p2 : public std::vector<Neutrino_p2>
{};

#endif //> PARTICLEEVENTTPCNV_NEUTRINOCONTAINER_P2_H

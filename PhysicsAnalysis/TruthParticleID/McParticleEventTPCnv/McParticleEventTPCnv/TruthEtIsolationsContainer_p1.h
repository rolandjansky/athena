///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolationsContainer_p1.h 
// Header file for class TruthEtIsolationsContainer_p1
// Author: S.Binet<binet@cern.ch>
// Date:   July 2007
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCONTAINER_P1_H 
#define MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCONTAINER_P1_H 

// STL includes
#include <vector>

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthEtIsolations_p1.h"

class TruthEtIsolationsContainer_p1 : public std::vector<TruthEtIsolations_p1>
{};

#endif //> MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCONTAINER_P1_H

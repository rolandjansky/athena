///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleJetContainer_p1.h 
// Header file for class ParticleJetContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
// Author : P.A. Delsart, may 2008
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLEJETCONTAINER_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLEJETCONTAINER_P1_H 

// STL includes
#include <vector>

// ParticleEventTPCnv includes
#include "JetEventTPCnv/ParticleJet_p1.h"

class ParticleJetContainer_p1 : public std::vector<ParticleJet_p1>
{};

#endif //> PARTICLEEVENTTPCNV_PARTICLEJETCONTAINER_P1_H

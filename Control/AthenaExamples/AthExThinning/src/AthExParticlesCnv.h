///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticlesCnv.h 
// Header file for class AthExParticlesCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExPARTICLESCNV_H 
#define ATHEXTHINNING_AthExPARTICLESCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExParticles_p1.h"
#include "AthExParticlesCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<AthExParticles, AthExParticlesCnv_p1>
  AthExParticlesCnv;


#endif //> ATHEXTHINNING_AthExPARTICLESCNV_H


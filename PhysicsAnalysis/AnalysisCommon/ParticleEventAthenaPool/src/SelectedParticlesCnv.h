///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// SelectedParticlesCnv.h 
// Header file for class SelectedParticlesCnv
// Author: S.Protopopescu serban@bnl.gov
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_SELECTEDPARTICLESCNV_H
#define PARTICLEEVENTATHENAPOOL_SELECTEDPARTICLESCNV_H

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// ParticleEvent includes
#include "ParticleEvent/SelectedParticles.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/SelectedParticlesCnv_p1.h"

typedef T_AthenaPoolTPCnvCnv< SelectedParticles,
                              SelectedParticlesCnv_p1 >
  SelectedParticlesCnv;


#endif //>PARTICLEEVENTATHENAPOOL_SELECTEDPARTICLESCNV_H 

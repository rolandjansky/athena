///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrackPartilceAssocsCnv.h 
// Header file for class TrackParticleAssocsCnv
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_TRACKPARTICLEASSOCSCNV_H 
#define PARTICLEEVENTATHENAPOOL_TRACKPARTICLEASSOCSCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

// NavFourMom includes
#include "ParticleEvent/TrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/TrackParticleAssocs_p1.h"
#include "ParticleEventTPCnv/TrackParticleAssocsCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv< TrackParticleAssocs,
                              TrackParticleAssocsCnv_p1,
                              T_TPCnvNull<TrackParticleAssocs> >
  TrackParticleAssocsCnv;


#endif //> PARTICLEEVENTATHENAPOOL_TRACKPARTICLEASSOCSCNV_H 

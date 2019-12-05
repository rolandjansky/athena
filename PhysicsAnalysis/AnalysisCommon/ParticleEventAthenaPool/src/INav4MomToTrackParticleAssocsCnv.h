///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomToTrackParticleAssocsCnv.h 
// Header file for class INav4MomToTrackParticleAssocsCnv
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_INAV4MOMTOTRACKPARTICLEASSOCSCNV_H 
#define PARTICLEEVENTATHENAPOOL_INAV4MOMTOTRACKPARTICLEASSOCSCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

// NavFourMom includes
#include "ParticleEvent/INav4MomToTrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocs_p1.h"
#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocsCnv_p1.h"

// Forward declaration
class StoreGateSvc;

typedef T_AthenaPoolTPCnvCnv< INav4MomToTrackParticleAssocs,
                              INav4MomToTrackParticleAssocsCnv_p1,
                              T_TPCnvNull<INav4MomToTrackParticleAssocs> >
   INav4MomToTrackParticleAssocsCnv;


#endif //> PARTICLEEVENTATHENAPOOL_INAV4MOMTOTRACKPARTICLEASSOCSCNV_H 

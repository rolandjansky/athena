///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomToTrackParticleAssocsCnv.h 
// Header file for class INav4MomToTrackParticleAssocsCnv
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_INAV4MOMTOTRACKPARTICLEASSOCSCNV_H 
#define PARTICLEEVENTATHENAPOOL_INAV4MOMTOTRACKPARTICLEASSOCSCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// NavFourMom includes
#include "ParticleEvent/INav4MomToTrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocs_p1.h"
#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocsCnv_p1.h"

// Forward declaration
class StoreGateSvc;

// the latest persistent representation type of INav4MomToTrackParticleAssocs
typedef INav4MomToTrackParticleAssocs_p1  INav4MomToTrackParticleAssocs_PERS;

class INav4MomToTrackParticleAssocsCnv 
  : public T_AthenaPoolCustomCnv< INav4MomToTrackParticleAssocs, 
                                  INav4MomToTrackParticleAssocs_PERS 
                                  > 
{

  // make the factory for this converter our friend
  friend class CnvFactory<INav4MomToTrackParticleAssocsCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  INav4MomToTrackParticleAssocsCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual INav4MomToTrackParticleAssocs_PERS*
    createPersistent( INav4MomToTrackParticleAssocs* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual INav4MomToTrackParticleAssocs* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Pointer to the StoreGateSvc
   */
  StoreGateSvc* m_storeGate;

  INav4MomToTrackParticleAssocsCnv_p1 m_tpConverter_p1;

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> PARTICLEEVENTATHENAPOOL_INAV4MOMTOTRACKPARTICLEASSOCSCNV_H 

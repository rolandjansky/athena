///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackPartilceAssocsCnv.h 
// Header file for class TrackParticleAssocsCnv
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_TRACKPARTICLEASSOCSCNV_H 
#define PARTICLEEVENTATHENAPOOL_TRACKPARTICLEASSOCSCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// NavFourMom includes
#include "ParticleEvent/TrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/TrackParticleAssocs_p1.h"
#include "ParticleEventTPCnv/TrackParticleAssocsCnv_p1.h"

// Forward declaration
class StoreGateSvc;

// the latest persistent representation type of TrackParticleAssocs
typedef TrackParticleAssocs_p1  TrackParticleAssocs_PERS;

class TrackParticleAssocsCnv: public T_AthenaPoolCustomCnv<
                                          TrackParticleAssocs, 
                                          TrackParticleAssocs_PERS 
                                        > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<TrackParticleAssocsCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  TrackParticleAssocsCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TrackParticleAssocs_PERS*
    createPersistent( TrackParticleAssocs* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TrackParticleAssocs* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Pointer to the StoreGateSvc
   */
  StoreGateSvc* m_storeGate;

  TrackParticleAssocsCnv_p1 m_tpConverter_p1;

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> PARTICLEEVENTATHENAPOOL_TRACKPARTICLEASSOCSCNV_H 

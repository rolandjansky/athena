///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneContainerCnv.h 
// Header file for class ParticleShallowCloneContainerCnv
// Author: Karsten Koeneke - February 2013
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_PARTICLESHALLOWCLONECONTAINERCNV_H 
#define PARTICLEEVENTATHENAPOOL_PARTICLESHALLOWCLONECONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowCloneContainer_p1.h"

// ParticleEvent includes
#include "ParticleEvent/ParticleShallowCloneContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef ParticleShallowCloneContainer_p1  ParticleShallowCloneContainer_PERS;

class ParticleShallowCloneContainerCnv : public T_AthenaPoolCustomCnv< ParticleShallowCloneContainer, 
                                                                       ParticleShallowCloneContainer_PERS 
                                                                       > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<ParticleShallowCloneContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  ParticleShallowCloneContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual ParticleShallowCloneContainer_PERS*
    createPersistent( ParticleShallowCloneContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual ParticleShallowCloneContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H

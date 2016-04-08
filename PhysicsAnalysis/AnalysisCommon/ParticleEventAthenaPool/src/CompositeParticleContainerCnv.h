///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleContainerCnv.h 
// Header file for class CompositeParticleContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H 
#define PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/CompositeParticleContainer_p1.h"

// ParticleEvent includes
#include "ParticleEvent/CompositeParticleContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef CompositeParticleContainer_p1  CompositeParticleContainer_PERS;

class CompositeParticleContainerCnv: public T_AthenaPoolCustomCnv<
                                      CompositeParticleContainer, 
                                      CompositeParticleContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<CompositeParticleContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  CompositeParticleContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual CompositeParticleContainer_PERS*
    createPersistent( CompositeParticleContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual CompositeParticleContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCOMPOSITEPARTICLEAUXCONTAINERCNV_H
#define XAODCOMPOSITEPARTICLEAUXCONTAINERCNV_H

#include "xAODParticleEvent/CompositeParticleAuxContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"


typedef T_AthenaPoolCustomCnv<xAOD::CompositeParticleAuxContainer,
                              xAOD::CompositeParticleAuxContainer>
          xAODCompositeParticleAuxContainerCnvBase;


class xAODCompositeParticleAuxContainerCnv : public xAODCompositeParticleAuxContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODCompositeParticleAuxContainerCnv >;

 protected:

  /** The constructor */
  xAODCompositeParticleAuxContainerCnv( ISvcLocator* svcLoc );

  /** Create persistent container */
  xAOD::CompositeParticleAuxContainer* createPersistent( xAOD::CompositeParticleAuxContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::CompositeParticleAuxContainer* createTransient();

};
#endif //XAODCOMPOSITEPARTICLEAUXCONTAINERCNV_H

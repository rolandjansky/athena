/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPARTICLEAUXCONTAINERCNV_H
#define XAODPARTICLEAUXCONTAINERCNV_H

#include "xAODParticleEvent/ParticleAuxContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"


typedef T_AthenaPoolCustomCnv<xAOD::ParticleAuxContainer,
                              xAOD::ParticleAuxContainer>
          xAODParticleAuxContainerCnvBase;


class xAODParticleAuxContainerCnv : public xAODParticleAuxContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODParticleAuxContainerCnv >;

 protected:

  /** The constructor */
  xAODParticleAuxContainerCnv( ISvcLocator* svcLoc );

  /** Create persistent container */
  xAOD::ParticleAuxContainer* createPersistent( xAOD::ParticleAuxContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::ParticleAuxContainer* createTransient();

};
#endif //XAODPARTICLEAUXCONTAINERCNV_H

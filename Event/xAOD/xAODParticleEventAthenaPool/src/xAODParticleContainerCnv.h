/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPARTICLECONTAINERCNV_H
#define XAODPARTICLECONTAINERCNV_H

#include "xAODParticleEvent/Particle.h"
#include "xAODParticleEvent/ParticleContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



typedef T_AthenaPoolCustomCnv<xAOD::ParticleContainer,
                              xAOD::ParticleContainer>
          xAODParticleContainerCnvBase;

class xAODParticleContainerCnv : public xAODParticleContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODParticleContainerCnv >;

 public:

  /** The constructor */
  xAODParticleContainerCnv( ISvcLocator* svcLoc );

  StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

  /** Create persistent container */
  xAOD::ParticleContainer* createPersistent( xAOD::ParticleContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::ParticleContainer* createTransient();

 private:

  /** Prepates object for persistence */
  void toPersistent(xAOD::Particle* theParticle) const;

  /** Storegate key name */
  std::string m_keyName;

};
#endif // XAODPARTICLECONTAINERCNV_H

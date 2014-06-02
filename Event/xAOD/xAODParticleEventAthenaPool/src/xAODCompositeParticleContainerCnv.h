/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCOMPOSITEPARTICLECONTAINERCNV_H
#define XAODCOMPOSITEPARTICLECONTAINERCNV_H

#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



typedef T_AthenaPoolCustomCnv<xAOD::CompositeParticleContainer,
                              xAOD::CompositeParticleContainer>
          xAODCompositeParticleContainerCnvBase;

class xAODCompositeParticleContainerCnv : public xAODCompositeParticleContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODCompositeParticleContainerCnv >;

 public:

  /** The constructor */
  xAODCompositeParticleContainerCnv( ISvcLocator* svcLoc );

  StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

  /** Create persistent container */
  xAOD::CompositeParticleContainer* createPersistent( xAOD::CompositeParticleContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::CompositeParticleContainer* createTransient();

 private:

  /** Prepates object for persistence */
  void toPersistent(xAOD::CompositeParticle* theCompositeParticle) const;

  /** Storegate key name */
  std::string m_keyName;

};
#endif // XAODCOMPOSITEPARTICLECONTAINERCNV_H

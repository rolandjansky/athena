/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPFOAUXCONTAINERCNV_H
#define XAODPFOAUXCONTAINERCNV_H

//based on xAODMuonAuxContainerCnv

#include "xAODPFlow/PFOAuxContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

typedef T_AthenaPoolCustomCnv<xAOD::PFOAuxContainer, xAOD::PFOAuxContainer> xAODPFOAuxContainerCnvBase;

class xAODPFOAuxContainerCnv : public xAODPFOAuxContainerCnvBase {

  /** Other example classes do this */
  friend class CnvFactory < xAODPFOAuxContainerCnv >;

 protected:
  
  /** The constructor */
  xAODPFOAuxContainerCnv( ISvcLocator* svcLoc );

  /** Create persistent container */
  xAOD::PFOAuxContainer* createPersistent( xAOD::PFOAuxContainer* trans );

  /** Create transient object, when reading from disk */
  xAOD::PFOAuxContainer* createTransient();

};
#endif

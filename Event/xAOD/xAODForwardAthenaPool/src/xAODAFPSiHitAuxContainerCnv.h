/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPSIHITAUXCONTAINERCNV_H
#define AFPSIHITAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPSiHitAuxContainer.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPSiHitAuxContainer, xAOD::AFPSiHitAuxContainer> xAODAFPSiHitAuxContainerCnvBase;


class xAODAFPSiHitAuxContainerCnv :
  public xAODAFPSiHitAuxContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPSiHitAuxContainerCnv >;
 
protected:
  /// Converter constructor
  xAODAFPSiHitAuxContainerCnv( ISvcLocator* svcLoc );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPSiHitAuxContainer*
  createPersistent( xAOD::AFPSiHitAuxContainer* trans );
  
  /// Function reading in the object from the input file
  virtual xAOD::AFPSiHitAuxContainer* createTransient();
 
};


#endif


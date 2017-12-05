/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPToFHITAUXCONTAINERCNV_H
#define AFPToFHITAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPToFHitAuxContainer.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPToFHitAuxContainer, xAOD::AFPToFHitAuxContainer> xAODAFPToFHitAuxContainerCnvBase;


class xAODAFPToFHitAuxContainerCnv :
  public xAODAFPToFHitAuxContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPToFHitAuxContainerCnv >;
 
protected:
  /// Converter constructor
  xAODAFPToFHitAuxContainerCnv( ISvcLocator* svcLoc );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPToFHitAuxContainer*
  createPersistent( xAOD::AFPToFHitAuxContainer* trans );
  
  /// Function reading in the object from the input file
  virtual xAOD::AFPToFHitAuxContainer* createTransient();
 
};


#endif


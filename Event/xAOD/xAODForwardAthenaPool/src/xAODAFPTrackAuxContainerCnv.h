/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPTRACKAUXCONTAINERCNV_H
#define AFPTRACKAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPTrackAuxContainer.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPTrackAuxContainer, xAOD::AFPTrackAuxContainer> xAODAFPTrackAuxContainerCnvBase;


class xAODAFPTrackAuxContainerCnv :
  public xAODAFPTrackAuxContainerCnvBase {
 
public:
  /// Converter constructor
  xAODAFPTrackAuxContainerCnv( ISvcLocator* svcLoc );
 
protected:
  /// Function preparing the container to be written out
  virtual xAOD::AFPTrackAuxContainer*
  createPersistent( xAOD::AFPTrackAuxContainer* trans );
  
  /// Function reading in the object from the input file
  virtual xAOD::AFPTrackAuxContainer* createTransient();
 
};
 
#endif
